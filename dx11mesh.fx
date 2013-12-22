cbuffer perMaterial : register(b0)
{
	row_major float4x4 g_matW : packoffset(c0);
	row_major float4x4 g_matVP : packoffset(c4);
	float4 faceColor : packoffset(c8);
	float4 emissive : packoffset(c9);
	row_major float4x4 bones[50] : packoffset(c12);
};

float4 CalcColor(float3 normal)
{
	float4  d;
	d.xyz = emissive.xyz + saturate(dot( normalize(normal), normalize(float3(0,1,-1)))) * faceColor.xyz;
	d.w = 1.0f;
	return d;
}

struct VS_INPUT {
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float4 Col : COLOR;
	float2 Tex0: TEXCOORD;
	float3 weights : BLENDWEIGHTS;
	int4 indices : BLENDINDICES;
};

struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
	float2 Tex0: TEXCOORD0;
};
VS_OUTPUT mainVS( VS_INPUT _In ) {
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4x4 comb =
		bones[_In.indices[0]] * _In.weights[0] +
		bones[_In.indices[1]] * _In.weights[1] +
		bones[_In.indices[2]] * _In.weights[2] +
		bones[_In.indices[3]] * (1 - _In.weights[0] - _In.weights[1] - _In.weights[2]);
	Out.Pos = mul(float4( _In.Pos, 1 ), mul(comb, mul(g_matW,g_matVP)));
	_In.Normal = normalize(mul(_In.Normal, mul(comb, g_matW)));
	Out.Col = CalcColor( _In.Normal );
	Out.Col.w = 1;
	Out.Tex0 = _In.Tex0;
	return Out;
}
SamplerState gSampler : register(s0);
Texture2D gTexture : register(t0);
float4 mainPS( VS_OUTPUT _In ) : SV_TARGET
{
	float4 diffuse = gTexture.Sample( gSampler, _In.Tex0 );
	return _In.Col * diffuse;
}

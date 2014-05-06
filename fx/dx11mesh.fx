cbuffer perMaterial : register(b0)
{
	row_major float4x4 g_matW;
	row_major float4x4 g_matV;
	row_major float4x4 g_matP;
	float4 faceColor;
	float4 emissive;
	float4 padding1;
	float4 padding2;
	row_major float4x4 bones[70];
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
	float4 normalInView : NORMAL;
	float3 reflectDir : REFDIR;
	float4 Col : COLOR;
	float2 Tex0: TEXCOORD0;
};
VS_OUTPUT mainVS( VS_INPUT _In ) {
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float3 camDir = float3(g_matV._13, g_matV._23, g_matV._33);

	float4x4 comb =
		bones[_In.indices[0]] * _In.weights[0] +
		bones[_In.indices[1]] * _In.weights[1] +
		bones[_In.indices[2]] * _In.weights[2] +
		bones[_In.indices[3]] * (1 - _In.weights[0] - _In.weights[1] - _In.weights[2]);
	Out.Pos = mul(float4( _In.Pos, 1 ), mul(comb, mul(g_matW, mul(g_matV, g_matP))));
	Out.normalInView = normalize(mul(_In.Normal, mul(comb, mul(g_matW, g_matV))));
	Out.reflectDir = reflect(camDir, normalize(mul(_In.Normal, mul(comb, g_matW))));
	Out.Col = _In.Col;
	Out.Tex0 = _In.Tex0;
	return Out;
}
SamplerState gSampler : register(s0);
Texture2D gTexture : register(t0);
float4 mainPS( VS_OUTPUT _In ) : SV_TARGET
{
	return gTexture.Sample(gSampler, _In.normalInView.xy * float2(0.5, -0.5) + 0.5);
}

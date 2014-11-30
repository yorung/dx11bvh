cbuffer perMaterial : register(b0)
{
	row_major float4x4 matW;
	row_major float4x4 matV;
	row_major float4x4 matP;
	float4 faceColor;
	float4 emissive;
	float3 camPos;
	float padding1;
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
	float3 normalInView : NORMAL;
	float3 reflectDir : REFDIR;
	float3 refractDir : REFRDIR;
	float4 Col : COLOR;
	float2 Tex0 : TEXCOORD0;
};
VS_OUTPUT mainVS( VS_INPUT _In ) {
	VS_OUTPUT Out = (VS_OUTPUT)0;

//	float3 camDir = float3(g_matV._13, g_matV._23, g_matV._33);
	float3 camDir = normalize(mul(float4(_In.Pos, 1), matW).xyz - camPos);

	float4x4 comb =
		bones[_In.indices[0]] * _In.weights[0] +
		bones[_In.indices[1]] * _In.weights[1] +
		bones[_In.indices[2]] * _In.weights[2] +
		bones[_In.indices[3]] * (1 - _In.weights[0] - _In.weights[1] - _In.weights[2]);
//	float4x4 cw = mul(comb, matW);
	float4x4 cw = matW;
	float4x4 cwv = mul(cw, matV);
	float4x4 cwvp = mul(cwv, matP);
	Out.Pos = mul(float4(_In.Pos, 1), cwvp);
	Out.normalInView = normalize(mul(float4(_In.Normal, 0), cwv)).xyz;
	float3 normalInWorld = normalize(mul(float4(_In.Normal, 0), cw)).xyz;
	Out.reflectDir = reflect(camDir, normalInWorld);
	Out.refractDir = refract(camDir, normalInWorld, 1 / 1.3333);
	Out.Col = _In.Col;
	Out.Tex0 = _In.Tex0;
	return Out;
}
SamplerState gSampler : register(s0);
Texture2D gTexture : register(t0);
float4 mainPS(VS_OUTPUT _In) : SV_TARGET
{
	float3 samp1 = normalize(normalize(_In.reflectDir) - float3(0, 0, 1));
	float3 samp2 = normalize(normalize(_In.refractDir) - float3(0, 0, 1));
	return lerp(
		gTexture.Sample(gSampler, samp1.xy * float2(0.5, -0.5) + 0.5),
		gTexture.Sample(gSampler, samp2.xy * float2(0.5, -0.5) + 0.5),
		dot(_In.normalInView, -float3(0, 0, 1)));
}

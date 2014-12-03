cbuffer perMaterial : register(b0)
{
	row_major float4x4 matW;
	row_major float4x4 matV;
	row_major float4x4 matP;
	float4 faceColor;
	float4 emissive;
	float3 camPos;
	float padding1;
};

struct Vertex
{
	float3 pos;
	float3 normal;
};

StructuredBuffer<Vertex> vertexIn : register(t0);

float4 CalcColor(float3 normal)
{
	float4  d;
	d.xyz = emissive.xyz + saturate(dot( normalize(normal), normalize(float3(0,1,-1)))) * faceColor.xyz;
	d.w = 1.0f;
	return d;
}

struct VS_INPUT {
	float4 Col : COLOR;
	float2 Tex0: TEXCOORD;
};

struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float3 normalInView : NORMAL;
	float3 reflectDir : REFDIR;
	float3 refractDir : REFRDIR;
	float4 Col : COLOR;
	float2 Tex0 : TEXCOORD0;
};

VS_OUTPUT mainVS(VS_INPUT _In, uint id : SV_VertexID) {
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float3 pos = vertexIn[id].pos;
	float3 norm = vertexIn[id].normal;

	float4x4 w = matW;
	float4x4 wv = mul(w, matV);
	float4x4 wvp = mul(wv, matP);

//	float3 camDir = float3(g_matV._13, g_matV._23, g_matV._33);
	float3 camDir = normalize(mul(float4(pos, 1), w).xyz - camPos);

	Out.Pos = mul(float4(pos, 1), wvp);
	Out.normalInView = normalize(mul(float4(norm, 0), wv)).xyz;
	float3 normalInWorld = normalize(mul(float4(norm, 0), w)).xyz;
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

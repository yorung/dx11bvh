cbuffer perMaterial : register(b2)
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
	float4 Col : COLOR;
	float2 Tex0 : TEXCOORD0;
};

VS_OUTPUT mainVS(VS_INPUT _In, uint id : SV_VertexID) {
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float3 pos = vertexIn[id].pos;

	float4x4 w = matW;
	float4x4 wv = mul(w, matV);
	float4x4 wvp = mul(wv, matP);

	Out.Pos = mul(float4(pos, 1), wvp);
	Out.Col = _In.Col;
	Out.Tex0 = _In.Tex0;
	return Out;
}
SamplerState gSampler : register(s0);
Texture2D gTexture : register(t0);
float4 mainPS(VS_OUTPUT _In) : SV_TARGET
{
	return gTexture.Sample(gSampler, _In.Tex0) * _In.Col;
}

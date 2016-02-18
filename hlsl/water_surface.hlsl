cbuffer perFrame : register(b0)
{
	row_major float4x4 matW;
	row_major float4x4 matV;
	row_major float4x4 matP;
	float3 camPos;
	float padding;
};

Texture2D gTexture : register(t0);
SamplerState samplerState : register(s0);

struct VsInput {
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

struct VsToPs {
	float4 pos : SV_POSITION;
	float3 normalInView : NORMAL;
	float3 reflectDir : REFDIR;
	float3 refractDir : REFRDIR;
};

VsToPs mainVS(VsInput inp)
{
	VsToPs outp;


	float4x4 w = matW;
	float4x4 wv = mul(w, matV);
	float4x4 wvp = mul(wv, matP);

	float3 camDir = normalize(mul(float4(inp.pos, 1), matW).xyz - camPos);
	outp.normalInView = normalize(mul(float4(inp.normal, 0), wv)).xyz;
	float3 normalInWorld = normalize(mul(float4(inp.normal, 0), w)).xyz;

	outp.reflectDir = reflect(camDir, normalInWorld);
	outp.refractDir = refract(camDir, normalInWorld, 1 / 1.3333);

	outp.pos = mul(float4(inp.pos, 1), wvp);
	return outp;
}

float4 mainPS(VsToPs inp) : SV_TARGET
{
	float3 samp1 = normalize(normalize(inp.reflectDir) - float3(0, 0, 1));
	float3 samp2 = normalize(normalize(inp.refractDir) - float3(0, 0, 1));
	return lerp(
		gTexture.Sample(samplerState, samp1.xy * float2(0.5, -0.5) + 0.5),
		gTexture.Sample(samplerState, samp2.xy * float2(0.5, -0.5) + 0.5),
		dot(inp.normalInView, -float3(0, 0, 1)));
}

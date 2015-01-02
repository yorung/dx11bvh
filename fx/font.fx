struct Vertex
{
	float2 pos;
	float2 coord;
};

struct VsIn {
	float2 pos : POSITION;
	float2 coord: TEXCOORD;
};

struct VsToPs {
	float4 pos : SV_POSITION;
	float2 coord : TEXCOORD;
};

VsToPs mainVS(VsIn vsIn) {
	VsToPs vsOut;
	vsOut.pos = float4(vsIn.pos, 0, 1);
	vsOut.coord = vsIn.coord;
	return vsOut;
}

SamplerState gSampler : register(s0);
Texture2D gTexture : register(t0);
float4 mainPS(VsToPs psIn) : SV_TARGET
{
//	return 1;
//	return gTexture.Sample(gSampler, psIn.coord);
	float v = gTexture.Sample(gSampler, psIn.coord).w;
//	return float4(v, v, v, 1);
	return float4(0.5, 1, 1, v);
}

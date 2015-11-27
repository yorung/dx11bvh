cbuffer perObject : register(b0)
{
	row_major matrix invVP;
}

Texture2D gTexture : register(t0);
SamplerState samplerState : register(s0);

struct VsToPs
{
	float4 pos : SV_POSITION;
	float4 pos2 : POS2;
};

VsToPs mainVS(uint id : SV_VertexID)
{
	VsToPs ret;
	ret.pos = float4(id & 1 ? 1 : -1, id & 2 ? -1 : 1, 1, 1);
	ret.pos2 = ret.pos;
	return ret;
}

RWTexture2D<float4> uav : register(u1);

void mainPS(VsToPs inp)
{
	uint2 pos = (uint2)inp.pos.xy;
	float4 color = gTexture.Sample(samplerState, inp.pos2.xy * float2(0.5, -0.5) + 0.5);
	uav[pos] = color * 2;
}

/*
RWTexture2D<float4> uav : register(u1);
void mainPS(VsToPs inp)
{
	uint2 pos = (uint2)inp.pos.xy;
	float4 color = gTexture.Sample(samplerState, inp.pos2.xy * float2(0.5, -0.5) + 0.5);
	color.x = pos.x & 4;
	uint4 uc = (uint4)color * 255;
//	uav[pos] = (uc.w << 24) | (uc.z << 16) | (uc.y << 8) | uc.x;
	uav[pos] = 0xff0000ff;
}

*/

/*
RWTexture2D<uint> uav : register(u0);
void mainPS(VsToPs inp) : SV_Target
{
	uint2 pos = (uint2)inp.pos.xy;
	float4 color = gTexture.Sample(samplerState, inp.pos2.xy * float2(0.5, -0.5) + 0.5);
	uint4 u = (uint4)color;
}
*/
cbuffer perObject : register(b0)
{
	row_major matrix invVP;
}

TextureCube texCube : register(t0);
SamplerState samplerState : register(s0);

struct VsToPs
{
	float4 pos : SV_POSITION;
	float4 screenPos : POS2;
};

VsToPs mainVS(uint id : SV_VertexID)
{
	VsToPs ret;
	ret.pos = float4(id & 2 ? 1 : -1, id & 1 ? -1 : 1, 1, 1);
	ret.screenPos = ret.pos;
	return ret;
}

float4 mainPS(VsToPs inp) : SV_Target
{
	float2 scale = float2(4.0f / 3.0f, 1) * 3;	// scale & aspect ratio
	float2 plane = inp.screenPos.xy * scale;
	float3 dir = float3(plane.x * 2, plane.y * 2, -1 + dot(plane, plane)) / (1 + dot(plane, plane));
	return texCube.Sample(samplerState, dir.xzy);	// y is upper
}
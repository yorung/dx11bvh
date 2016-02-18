cbuffer perObject : register(b0)
{
	row_major matrix invVP;
}

TextureCube texCube : register(t0);
SamplerState samplerState : register(s0);

struct VsToPs
{
	float4 pos : SV_POSITION;
	float4 pos2 : POS2;
};

VsToPs mainVS(uint id : SV_VertexID)
{
	VsToPs ret;
	ret.pos = float4(id & 2 ? 1 : -1, id & 1 ? -1 : 1, 1, 1);
	ret.pos2 = ret.pos;
	return ret;
}

float4 mainPS(VsToPs inp) : SV_Target
{
	float3 dir = normalize(mul(inp.pos2, invVP).xyz);
	return texCube.Sample(samplerState, dir);
}

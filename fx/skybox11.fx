cbuffer perObject : register(b0)
{
	row_major matrix invVP;
}

TextureCube texCube : register(t0);
SamplerState samplerState : register(s0);

struct VsToPs
{
	float4 pos : SV_POSITION;
	float3 dir : DIR;
};

VsToPs mainVS(uint id : SV_VertexID)
{
	VsToPs ret;
	ret.pos = float4(id & 1 ? 1 : -1, id & 2 ? -1 : 1, 1, 1);
	ret.dir = normalize(mul(ret.pos, invVP));
	return ret;
}

float4 mainPS(VsToPs inp) : SV_Target
{
	return texCube.Sample(samplerState, inp.dir);
}

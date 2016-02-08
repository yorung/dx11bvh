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
	float3 dir;
	float lonR = inp.screenPos.x * 3.14159265f;
	float latR = inp.screenPos.y * 3.14159265f / 2;
	dir.x = cos(latR) * sin(lonR);
	dir.z = cos(latR) * cos(lonR);
	dir.y = sin(latR);
	return texCube.Sample(samplerState, dir);
}
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
	ret.pos = float4(id & 1 ? 1 : -1, id & 2 ? -1 : 1, 1, 1);
	ret.screenPos = ret.pos;
	return ret;
}

float4 mainPS(VsToPs inp) : SV_Target
{
	float dist = sqrt(dot(inp.screenPos.xy, inp.screenPos.xy));
//	float axisX = sin((dist - 1) * 3.14159265f);
	float axisX = -cos(dist * 3.14159265f);
//	float axisX = sin(dist * 3.14159265f);
	float axisY = atan2(inp.screenPos.x, inp.screenPos.y);
	float3 dir;
	dir.x = sin(axisY) * cos(axisX);
	dir.z = cos(axisY) * cos(axisX);
	dir.y = sin(axisX);
	return texCube.Sample(samplerState, dir);
//	return dist;
}
cbuffer perFrame : register(b0)
{
	float4x4 matVP;
};

struct VsInput {
	float3 pos : POSITION;
	float3 col : COLOR;
};

struct VsOutput {
	float4 pos : SV_POSITION;
	float3 col : COLOR;
};

VsOutput mainVS(VsInput inp)
{
	VsOutput outp;
	outp.pos = mul(matVP, float4(inp.pos, 1));
	outp.col = inp.col;
	return outp;
}

float4 mainPS(VsOutput inp) : SV_TARGET
{
	return float4(inp.col, 1);
}

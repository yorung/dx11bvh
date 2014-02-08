cbuffer perFrame : register(b0)
{
	row_major float4x4 g_matW : packoffset(c0);
	row_major float4x4 g_matVP : packoffset(c4);
};

struct VsInput {
	float3 pos : POSITION;
	float4 col : COLOR;
};

struct VsOutput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

VsOutput mainVS(VsInput inp)
{
	VsOutput outp;
	outp.pos = mul(float4(inp.pos, 1), mul(g_matW,g_matVP));
	outp.col = inp.col;
	return outp;
}

float4 mainPS(VsOutput inp) : SV_TARGET
{
	return inp.col;
}

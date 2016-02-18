struct Vertex
{
	float3 pos;
	float3 normal;
};

struct Skin
{
	float3 weights;
	uint indices;
};

cbuffer perObject : register(b0)
{
	row_major float4x4 bones[70];
};

StructuredBuffer<Vertex> vertexIn : register(t0);
StructuredBuffer<Skin> skinIn : register(t1);
RWStructuredBuffer<Vertex> vertexOut : register(u0);

[numthreads(1, 1, 1)]
void mainCS(uint3 id : SV_DispatchThreadID)
{
	Vertex vIn = vertexIn[id.x];
	Skin sIn = skinIn[id.x];
	float4x4 comb =
		bones[sIn.indices & 0xff] * sIn.weights[0] +
		bones[(sIn.indices & 0xff00) >> 8] * sIn.weights[1] +
		bones[(sIn.indices & 0xff0000) >> 16] * sIn.weights[2] +
		bones[(sIn.indices & 0xff000000) >> 24] * (1 - sIn.weights[0] - sIn.weights[1] - sIn.weights[2]);
	vertexOut[id.x].pos = mul(float4(vIn.pos, 1), comb).xyz;
	vertexOut[id.x].normal = mul(float4(vIn.normal, 0), comb).xyz;
}

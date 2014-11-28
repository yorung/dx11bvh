struct Vertex
{
	float3 pos;
	float3 normal;
};

struct Skin
{
	float3 weights;
	int4 indices;
};

cbuffer perObject : register(b0)
{
	row_major float4x4 bones[70];
};

StructedBuffer<Vertex> vertexIn : register(t0);
StructedBuffer<Skin> skinIn : register(t1);
RWStructedBuffer<Vertex> vertexOut : register(u0);

[numthreads(64, 1, 1)]
void mainCS(uint3 id : SV_DispatchThreadID)
{
	Vertex vIn = vertexIn[id.x];
	Skin sIn = skinIn[id.x];
	float4x4 comb =
		bones[sIn.indices[0]] * sIn.weights[0] +
		bones[sIn.indices[1]] * sIn.weights[1] +
		bones[sIn.indices[2]] * sIn.weights[2] +
		bones[sIn.indices[3]] * (1 - sIn.weights[0] - sIn.weights[1] - sIn.weights[2]);
	vertexOut[id.x].pos = mul(float4(vIn.pos, 1), comb).xyz;
	vertexOut[id.x].normal = mul(float4(vIn.normal, 1), comb).xyz;
}

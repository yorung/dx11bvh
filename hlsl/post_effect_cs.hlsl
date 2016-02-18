cbuffer perObject : register(b0)
{
	row_major matrix invVP;
}

Texture2D gTexIn : register(t0);
RWTexture2D<float4> gTexOut : register(u0);

[numthreads(32, 8, 1)]
void mainCS(uint3 id : SV_DispatchThreadID)
{
	float3 col = gTexIn[id.xy].xyz;
//	float3 col = gTexOut[id.xy].xyz;
//	float brightness = col.r * 0.29 + col.g * 0.58 + col.b * 0.11;
//	brightness = 0.5;
//	gTexOut[id.xy] = float4(brightness, brightness, brightness, 1);
//	gTexOut[id.xy] = float4(0.0, 0.5, brightness, 1);
	col *= 2;
	gTexOut[id.xy] = float4(col.x, col.y, col.z, 1);
}

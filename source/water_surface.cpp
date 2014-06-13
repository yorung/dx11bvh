#include "stdafx.h"

WaterSurface waterSurface;

BufferMan::BMID WaterSurface::constantBufferId = -1;

struct WaterConstantBuffer
{
	Mat matW;
	Mat matV;
	Mat matP;
	Vec3 camPos;
	float padding;
};

const int tileMax = 50;
const int vertMax = tileMax + 1;
const float pitch = 1.0f / tileMax;
const float repeat = 4;
const float halflife = 0.6f;
const float heightUnit = 0.00375f;

static Vec3 MakePos(int x, int z, float hmap[vertMax][vertMax])
{
	float height = hmap[std::max(0,std::min(tileMax, x))][std::max(0,std::min(tileMax, z))];
	return Vec3(((float)x - tileMax / 2) * pitch, height, ((float)z - tileMax / 2) * pitch);
}

void WaterSurface::CreateRipple()
{
	WaterRipple r;
	r.generatedTime = GetTime();
	r.u = Random() * 2 - 1;
	r.v = Random() * 2 - 1;
	ripples.push_back(r);

	if (ripples.size() > 10) {
		ripples.pop_front();
	}
}

void WaterSurface::UpdateVert(std::vector<WaterVert>& vert)
{
	double tm = GetTime();

	float hmap[vertMax][vertMax];
	memset(hmap, 0, sizeof(hmap));
	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
			std::for_each(ripples.begin(), ripples.end(),
				[&](const WaterRipple& r) {
					float u = (float)x / tileMax * 2 - 1;
					float v = (float)z / tileMax * 2 - 1;
					float uDist = u - r.u;
					float vDist = v - r.v;
					float l = sqrt(uDist * uDist + vDist * vDist);
					float lifeTime = (float)(tm - r.generatedTime);
					float timeAfterArrived = lifeTime - l;
					float h = timeAfterArrived > 0 ? (float)sin(timeAfterArrived * XM_2PI * repeat) * heightUnit : 0;
					float life = (float)(tm - r.generatedTime);
					h *= std::min(1.0f, std::powf(0.5f, life / halflife));
					hmap[x][z] += h;
				}
			);
		}
	}

	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
			float h = hmap[x][z];
			WaterVert v;
			v.color = 0xff00aa00;
			v.pos = MakePos(x, z, hmap);
			Vec3 v1 = MakePos(x, z - 1, hmap);
			Vec3 v2 = MakePos(x - 1, z, hmap);
			Vec3 v3 = MakePos(x + 1, z + 1, hmap);
			v.normal = cross(v2 - v1, v3 - v2);
			vert.push_back(v);
		}
	}
}

WaterSurface::WaterSurface()
{
	pVertexBuffer = nullptr;
	pIndexBuffer = nullptr;
	pSamplerState = nullptr;
	pDSState = nullptr;
}

WaterSurface::~WaterSurface()
{
	Destroy();
}

void WaterSurface::Destroy()
{
	SAFE_RELEASE(pIndexBuffer);
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pSamplerState);
	SAFE_RELEASE(pDSState);
}

void WaterSurface::Init()
{
	Destroy();

	std::vector<DWORD> indi;
	std::vector<WaterVert> vert;
	UpdateVert(vert);

	for (int z = 0; z < tileMax; z++) {
		if (z != 0) {
			indi.push_back(z * vertMax);
		}
		indi.push_back(z * vertMax);
		for (int x = 0; x < tileMax; x++) {
			indi.push_back((z + 1) * vertMax + x);
			indi.push_back(z * vertMax + x + 1);
		}
		indi.push_back((z + 1) * vertMax + vertMax - 1);
		if (z != tileMax - 1) {
			indi.push_back((z + 1) * vertMax + vertMax - 1);
		}
	}

	int sizeVertices = vert.size() * sizeof(WaterVert);
	int sizeIndices = indi.size() * sizeof(DWORD);
	void* vertices = &vert[0];
	void* indices = &indi[0];
	lines = indi.size() / 2;

	static D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	texId = texMan.Create("resource\\Tiny_skin.dds", true);
	shaderId = shaderMan.Create("fx\\water_surface.fx", layout, dimof(layout));

	D3D11_SUBRESOURCE_DATA subresData = { vertices, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(sizeVertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), &subresData, &pVertexBuffer);
	subresData.pSysMem = indices;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(sizeIndices, D3D11_BIND_INDEX_BUFFER), &subresData, &pIndexBuffer);
	if (constantBufferId < 0) {
		constantBufferId = bufferMan.Create(sizeof(WaterConstantBuffer));
	}

	CD3D11_SAMPLER_DESC descSamp(D3D11_DEFAULT);
	descSamp.AddressU = descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	deviceMan11.GetDevice()->CreateSamplerState(&descSamp, &pSamplerState);
	deviceMan11.GetDevice()->CreateDepthStencilState(&CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT), &pDSState);
}

void WaterSurface::Update()
{
	std::vector<WaterVert> vert;
	UpdateVert(vert);
	D3D11_MAPPED_SUBRESOURCE r;
	deviceMan11.GetContext()->Map(pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &r);
	memcpy(r.pData, &vert[0], vert.size() * sizeof(WaterVert));
	deviceMan11.GetContext()->Unmap(pVertexBuffer, 0);
}

void WaterSurface::Draw()
{
	Update();

	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	shaderMan.Apply(shaderId);

	deviceMan11.GetContext()->OMSetDepthStencilState(pDSState, 1);
	deviceMan11.GetContext()->PSSetSamplers(0, 1, &pSamplerState);
	ID3D11ShaderResourceView* tx = texMan.Get(texId);
	deviceMan11.GetContext()->PSSetShaderResources(0, 1, &tx);

	UINT strides[] = { sizeof(WaterVert) };
	UINT offsets[] = { 0 };
	deviceMan11.GetContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceMan11.GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, strides, offsets);

	WaterConstantBuffer cBuf;
//	matrixMan.Get(MatrixMan::WORLD, cBuf.matW);
	cBuf.matW = Matrix::CreateScale(4000);
	matrixMan.Get(MatrixMan::VIEW, cBuf.matV);
	matrixMan.Get(MatrixMan::PROJ, cBuf.matP);
	cBuf.camPos = fastInv(cBuf.matV).GetRow(3);
	bufferMan.Write(constantBufferId, &cBuf);
	const auto buf = bufferMan.Get(constantBufferId);
	deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &buf);

	deviceMan11.GetContext()->DrawIndexed(lines * 2, 0, 0);
}


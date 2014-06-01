#include "stdafx.h"

WaterSurface waterSurface;

BufferMan::BMID WaterSurface::constantBufferId = -1;

struct WaterVert {
	Vec3 pos;
	DWORD color;
};

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

	std::vector<WaterVert> vert;
	std::vector<DWORD> indi;

	for (float x = -10; x <= 10; x++) {
		WaterVert v;
		v.color = 0xff00aa00;
		v.pos = Vec3(x * 100, 0, -1000);
		vert.push_back(v);
		indi.push_back(indi.size());
		v.pos = Vec3(x * 100, 0, 1000);
		vert.push_back(v);
		indi.push_back(indi.size());
	}
	for (float z = -10; z <= 10; z++) {
		WaterVert v;
		v.color = 0xff00aa00;
		v.pos = Vec3(-1000, 0, z * 100);
		vert.push_back(v);
		indi.push_back(indi.size());
		v.pos = Vec3(1000, 0, z * 100);
		vert.push_back(v);
		indi.push_back(indi.size());
	}

	int sizeVertices = vert.size() * sizeof(WaterVert);
	int sizeIndices = indi.size() * sizeof(DWORD);
	void* vertices = &vert[0];
	void* indices = &indi[0];
	lines = indi.size() / 2;

	static D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shaderId = shaderMan.Create("fx\\solid.fx", layout, dimof(layout));

	D3D11_SUBRESOURCE_DATA subresData = { vertices, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(sizeVertices, D3D11_BIND_VERTEX_BUFFER), &subresData, &pVertexBuffer);
	subresData.pSysMem = indices;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(sizeIndices, D3D11_BIND_INDEX_BUFFER), &subresData, &pIndexBuffer);
	if (constantBufferId < 0) {
		constantBufferId = bufferMan.Create(sizeof(SolidConstantBuffer));
	}

	CD3D11_SAMPLER_DESC descSamp(D3D11_DEFAULT);
	descSamp.AddressU = descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	deviceMan11.GetDevice()->CreateSamplerState(&descSamp, &pSamplerState);
	deviceMan11.GetDevice()->CreateDepthStencilState(&CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT), &pDSState);
}

void WaterSurface::Draw()
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	shaderMan.Apply(shaderId);

	Mat matWorld, matView, matProj;
	matrixMan.Get(MatrixMan::WORLD, matWorld);
	matrixMan.Get(MatrixMan::VIEW, matView);
	matrixMan.Get(MatrixMan::PROJ, matProj);
	Mat matW = matWorld;
	Mat matVP = matView * matProj;

	deviceMan11.GetContext()->OMSetDepthStencilState(pDSState, 1);
	deviceMan11.GetContext()->PSSetSamplers(0, 1, &pSamplerState);

	UINT strides[] = { sizeof(WaterVert) };
	UINT offsets[] = { 0 };
	deviceMan11.GetContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceMan11.GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, strides, offsets);

	SolidConstantBuffer cBuf;
	cBuf.matW = matW;
	cBuf.matVP = matVP;
	bufferMan.Write(constantBufferId, &cBuf);
	const auto buf = bufferMan.Get(constantBufferId);
	deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &buf);

	deviceMan11.GetContext()->DrawIndexed(lines * 2, 0, 0);
}


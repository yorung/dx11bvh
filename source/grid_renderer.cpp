#include "stdafx.h"

GridRenderer gridRenderer;

BufferMan::BMID GridRenderer::constantBufferId = -1;

struct GridVert {
	Vec3 pos;
	uint32_t color;
};

GridRenderer::GridRenderer()
{
	vbo = 0;
	ibo = 0;
	vao = 0;
	pSamplerState = nullptr;
}

GridRenderer::~GridRenderer()
{
	Destroy();
}

void GridRenderer::Destroy()
{
	afSafeDeleteBuffer(ibo);
	afSafeDeleteBuffer(vbo);
	afSafeDeleteVAO(vao);
	SAFE_RELEASE(pSamplerState);
}

void GridRenderer::Init()
{
	Destroy();

	std::vector<GridVert> vert;
	std::vector<AFIndex> indi;

	for (float x = -1000; x <= 1000; x += 100) {
		GridVert v;
		v.color = 0xff777777;
		v.pos = Vec3(x, 0, -1000);
		vert.push_back(v);
		indi.push_back(indi.size());
		v.pos = Vec3(x, 0, 1000);
		vert.push_back(v);
		indi.push_back(indi.size());
	}
	for (float z = -1000; z <= 1000; z += 100) {
		GridVert v;
		v.color = 0xff777777;
		v.pos = Vec3(-1000, 0, z);
		vert.push_back(v);
		indi.push_back(indi.size());
		v.pos = Vec3(1000, 0, z);
		vert.push_back(v);
		indi.push_back(indi.size());
	}

	int sizeVertices = vert.size() * sizeof(GridVert);
	lines = indi.size() / 2;

	static D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shaderId = shaderMan.Create("solid", layout, dimof(layout));

	vbo = afCreateVertexBuffer(sizeVertices, &vert[0]);
	ibo = afCreateIndexBuffer(&indi[0], indi.size());
	if (constantBufferId < 0) {
		constantBufferId = bufferMan.Create(sizeof(SolidConstantBuffer));
	}

	CD3D11_SAMPLER_DESC descSamp(D3D11_DEFAULT);
	descSamp.AddressU = descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	deviceMan11.GetDevice()->CreateSamplerState(&descSamp, &pSamplerState);

	int strides[] = { sizeof(GridVert) };
	vao = afCreateVAO(shaderId, layout, dimof(layout), 1, &vbo, strides, ibo);
}

void GridRenderer::Draw()
{
	shaderMan.Apply(shaderId);

	Mat matWorld, matView, matProj;
	matrixMan.Get(MatrixMan::WORLD, matWorld);
	matrixMan.Get(MatrixMan::VIEW, matView);
	matrixMan.Get(MatrixMan::PROJ, matProj);
	Mat matW = matWorld;
	Mat matVP = matView * matProj;

	afDepthStencilMode(false);
	deviceMan11.GetContext()->PSSetSamplers(0, 1, &pSamplerState);

	SolidConstantBuffer cBuf;
	cBuf.matW = matW;
	cBuf.matVP = matVP;
	bufferMan.Write(constantBufferId, &cBuf);
	const auto buf = bufferMan.Get(constantBufferId);
	deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &buf);

	afBindVAO(vao);
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceMan11.GetContext()->DrawIndexed(lines * 2, 0, 0);
}


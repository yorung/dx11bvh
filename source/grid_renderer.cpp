#include "stdafx.h"

GridRenderer gridRenderer;

struct GridVert {
	Vec3 pos;
	uint32_t color;
};

GridRenderer::GridRenderer()
{
	vao = 0;
}

GridRenderer::~GridRenderer()
{
	assert(!ubo);
	assert(!vbo);
	assert(!ibo);
}

void GridRenderer::Destroy()
{
	afSafeDeleteBuffer(ubo);
	afSafeDeleteBuffer(ibo);
	afSafeDeleteBuffer(vbo);
	afSafeDeleteVAO(vao);
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
		indi.push_back((AFIndex)indi.size());
		v.pos = Vec3(x, 0, 1000);
		vert.push_back(v);
		indi.push_back((AFIndex)indi.size());
	}
	for (float z = -1000; z <= 1000; z += 100) {
		GridVert v;
		v.color = 0xff777777;
		v.pos = Vec3(-1000, 0, z);
		vert.push_back(v);
		indi.push_back((AFIndex)indi.size());
		v.pos = Vec3(1000, 0, z);
		vert.push_back(v);
		indi.push_back((AFIndex)indi.size());
	}

	int sizeVertices = vert.size() * sizeof(GridVert);
	lines = indi.size() / 2;

	static D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shaderId = shaderMan.Create("solid", layout, dimof(layout), BM_NONE, false);

	vbo = afCreateVertexBuffer(sizeVertices, &vert[0]);
	ibo = afCreateIndexBuffer(&indi[0], indi.size());
	ubo = afCreateUBO(sizeof(SolidConstantBuffer));

	int strides[] = {sizeof(GridVert)};
	VBOID vbos[] = {vbo};
	vao = afCreateVAO(shaderId, layout, dimof(layout), 1, vbos, strides, ibo);
}

void GridRenderer::Draw()
{
	shaderMan.Apply(shaderId);

	Mat matView, matProj;
	matrixMan.Get(MatrixMan::VIEW, matView);
	matrixMan.Get(MatrixMan::PROJ, matProj);
	afBlendMode(BM_NONE);
	afDepthStencilMode(true);

	SolidConstantBuffer cBuf;
	cBuf.matVP = matView * matProj;
	afWriteBuffer(ubo, &cBuf, sizeof(cBuf));
	afBindBufferToBindingPoint(ubo, 0);
	afBindVAO(vao);
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceMan11.GetContext()->DrawIndexed(lines * 2, 0, 0);
}

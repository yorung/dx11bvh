#include "stdafx.h"

GridRenderer gridRenderer;

struct GridVert {
	Vec3 pos;
	Vec3 color;
};

GridRenderer::~GridRenderer()
{
	assert(!ubo);
	assert(!vbo);
	assert(!ibo);
	assert(!vao);
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
		v.color = Vec3(0.5, 0.5, 0.5);
		v.pos = Vec3(x, 0, -1000);
		vert.push_back(v);
		indi.push_back((AFIndex)indi.size());
		v.pos = Vec3(x, 0, 1000);
		vert.push_back(v);
		indi.push_back((AFIndex)indi.size());
	}
	for (float z = -1000; z <= 1000; z += 100) {
		GridVert v;
		v.color = Vec3(0.5, 0.5, 0.5);
		v.pos = Vec3(-1000, 0, z);
		vert.push_back(v);
		indi.push_back((AFIndex)indi.size());
		v.pos = Vec3(1000, 0, z);
		vert.push_back(v);
		indi.push_back((AFIndex)indi.size());
	}

	int sizeVertices = vert.size() * sizeof(GridVert);
	lines = indi.size() / 2;

	static InputElement layout[] = {
		CInputElement("POSITION", SF_R32G32B32_FLOAT, 0),
		CInputElement("COLOR", SF_R32G32B32_FLOAT, 12),
	};
	shaderId = shaderMan.Create("solid", layout, dimof(layout), BM_NONE, DSM_DEPTH_ENABLE, CM_DISABLE);

	vbo = afCreateVertexBuffer(sizeVertices, &vert[0]);
	ibo = afCreateIndexBuffer(&indi[0], indi.size());
	ubo = afCreateUBO(sizeof(Mat));

	int strides[] = {sizeof(GridVert)};
	VBOID vbos[] = {vbo};
	vao = afCreateVAO(layout, dimof(layout), 1, vbos, strides, ibo);
}

void GridRenderer::Draw()
{
	shaderMan.Apply(shaderId);
	Mat matView, matProj;
	matrixMan.Get(MatrixMan::VIEW, matView);
	matrixMan.Get(MatrixMan::PROJ, matProj);
	Mat matVP = matView * matProj;
	afWriteBuffer(ubo, &matVP, sizeof(Mat));
	afBindBufferToBindingPoint(ubo, 0);
	afBindVAO(vao);
	afDrawLineList(lines * 2);
}

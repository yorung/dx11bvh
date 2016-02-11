#include "stdafx.h"

WaterSurface waterSurface;

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
	r.centerPos = Vec2(Random(), Random()) * 2 - Vec2(1, 1);
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
					Vec2 pos = Vec2((float)x, (float)z) / (float)tileMax * 2.0f - Vec2(1, 1);
					float lifeTime = (float)(tm - r.generatedTime);
					float timeAfterArrived = lifeTime - length(r.centerPos - pos);
					float h = timeAfterArrived > 0 ? (float)sin(timeAfterArrived * (M_PI * 2) * repeat) * heightUnit : 0;
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
			v.pos = MakePos(x, z, hmap);
			Vec3 v1 = MakePos(x, z - 1, hmap);
			Vec3 v2 = MakePos(x - 1, z, hmap);
			Vec3 v3 = MakePos(x + 1, z + 1, hmap);
			v.normal = cross(v2 - v1, v3 - v2);
			vert.push_back(v);
		}
	}
}

WaterSurface::~WaterSurface()
{
	assert(!ubo);
	assert(!ibo);
	assert(!vbo);
	assert(!vao);
	assert(!srv);
	assert(!sampler);
}

void WaterSurface::Destroy()
{
	afSafeDeleteBuffer(ubo);
	afSafeDeleteBuffer(ibo);
	afSafeDeleteBuffer(vbo);
	afSafeDeleteVAO(vao);
	afSafeDeleteTexture(srv);
	afSafeDeleteSampler(sampler);
}

void WaterSurface::Init()
{
	Destroy();

	std::vector<AFIndex> indi;
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
	int sizeIndices = indi.size() * sizeof(AFIndex);
	void* vertices = &vert[0];
	AFIndex* indices = &indi[0];
	lines = indi.size() / 2;

	static InputElement layout[] = {
		CInputElement(0, "POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0),
		CInputElement(0, "NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 12),
	};
	srv = afLoadTexture("resource\\sphere_map.dds", TexDesc());
	shaderId = shaderMan.Create("water_surface", layout, dimof(layout), BM_NONE, DSM_DEPTH_ENABLE, CM_CW);

	vbo = afCreateDynamicVertexBuffer(sizeVertices);
	ibo = afCreateIndexBuffer(indices, indi.size());

	int strides[] = { sizeof(WaterVert) };
	VBOID vbos[] = { vbo };
	vao = afCreateVAO(layout, dimof(layout), 1, vbos, strides, ibo);
	ubo = afCreateUBO(sizeof(WaterConstantBuffer));
	sampler = afCreateSampler(SF_MIPMAP, SW_REPEAT);
}

void WaterSurface::Update()
{
	std::vector<WaterVert> vert;
	UpdateVert(vert);
	afWriteBuffer(vbo, &vert[0], vert.size() * sizeof(WaterVert));
}

void WaterSurface::Draw()
{
	Update();

	shaderMan.Apply(shaderId);
	afBindSamplerToBindingPoint(sampler, 0);
	afBindTextureToBindingPoint(srv, 0);

	WaterConstantBuffer cBuf;
	cBuf.matW = scale(4000);
	matrixMan.Get(MatrixMan::VIEW, cBuf.matV);
	matrixMan.Get(MatrixMan::PROJ, cBuf.matP);
	cBuf.camPos = fastInv(cBuf.matV).GetRow(3);
	afWriteBuffer(ubo, &cBuf, sizeof(cBuf));
	afBindBufferToBindingPoint(ubo, 0);
	afBindVAO(vao);
	afDrawIndexedTriangleStrip(lines * 2);
}

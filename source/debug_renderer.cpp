#include "stdafx.h"

DebugRenderer debugRenderer;

static void InitSkin(MeshSkin& s, BONE_ID boneId)
{
	s.blendIndices.x = s.blendIndices.y = s.blendIndices.z = s.blendIndices.w = boneId;
	s.blendWeights.x = s.blendWeights.y = s.blendWeights.z = 0;
}

static void InitVertex(MeshVertex& v)
{
	v.normal.x = 1;
	v.normal.y = 0;
	v.normal.z = 0;
	v.xyz.x = v.xyz.y = v.xyz.z = 0;
}

static void InitVertexColor(MeshColor& c, DWORD color)
{
	c.color = color;
	c.uv.x = c.uv.y = 0;
}

void CreateCone(Block& b, const Vec3& v1, const Vec3& v2, BONE_ID boneId, DWORD color)
{
	float radius = 0.15f;
	Vec3 boneDir = v2 - v1;
	Vec3 vRot0 = cross(boneDir, Vec3(0, 0, radius));
	if (length(vRot0) < 0.001f) {
		vRot0 = cross(boneDir, Vec3(0, radius, 0));
	}
	Vec3 vRot90 = cross(vRot0, normalize(boneDir));
	Vec3 vRotLast = v1 + vRot0;
	static const int div = 10;
	for (int j = 0; j < div; j++) {
		MeshVertex vert[3];
		MeshColor col;
		MeshSkin skin;
		InitVertexColor(col, color);
		for (auto& it : vert) {
			InitVertex(it);
		}
		InitSkin(skin, boneId);
		float rad = XM_2PI / div * (j + 1);
		Vec3 vRot = v1 + vRot0 * cosf(rad) + vRot90 * sinf(rad);
		vert[0].xyz = vRotLast;
		vert[1].xyz = v2;
		vert[2].xyz = vRot;
		Vec3 normal = cross(vRotLast - v2, v2 - vRot);
		for (int i = 0; i < 3; i++) {
			vert[i].normal = normal;
			b.vertices.push_back(vert[i]);
			b.skin.push_back(skin);
			b.color.push_back(col);
			b.indices.push_back(b.indices.size());
		}
		vRotLast = vRot;
	}
}

void DebugRenderer::CreatePivotMesh()
{
	for (BONE_ID i = 0; (unsigned)i < BONE_MAX; i++)	{
		float len = 12.0f;
		CreateCone(pivots, Vec3(), Vec3(len, 0, 0), i, 0xff0000ff);
		CreateCone(pivots, Vec3(), Vec3(0, len, 0), i, 0xff00ff00);
		CreateCone(pivots, Vec3(), Vec3(0, 0, len), i, 0xffff0000);
	}

	pivotsRenderer.Init(pivots);

	Material mat;
	mat.faceColor.x = 0.6f;
	mat.faceColor.y = 0.6f;
	mat.faceColor.z = 0.6f;
	mat.faceColor.w = 1.0f;
	mat.power = 1.0f;
	mat.specular.x = 1.0f;
	mat.specular.y = 1.0f;
	mat.specular.z = 1.0f;
	mat.specular.w = 1.0f;
	mat.emissive.x = 0.4f;
	mat.emissive.y = 0.4f;
	mat.emissive.z = 0.4f;
	mat.emissive.w = 1.0f;
	mat.tmid = texMan.Create("resource\\white.bmp");

	MaterialMap map;
	map.materialId = matMan.Create(mat);
	map.faceStartIndex = 0;
	map.faces = pivots.indices.size() / 3;
	pivots.materialMaps.push_back(map);
}

DebugRenderer::DebugRenderer()
{
}

DebugRenderer::~DebugRenderer()
{
	Destroy();
}

void DebugRenderer::Init()
{
	CreatePivotMesh();
}

void DebugRenderer::DrawPivots(const Mat mat[BONE_MAX], int num)
{
	Mat mat2[BONE_MAX];
	for (int i = 0; i < BONE_MAX; i++) {
		mat2[i] = i < num ? mat[i] : Mat();
	}

	pivotsRenderer.Draw(mat2, BONE_MAX, pivots);

	for (int i = 0; i < BONE_MAX; i++) {
		mat2[i] = orthogonalize(mat2[i]);
	}
	pivotsRenderer.Draw(mat2, BONE_MAX, pivots);
}

void DebugRenderer::Destroy()
{
	pivotsRenderer.Destroy();
}
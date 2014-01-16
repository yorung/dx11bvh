#include "stdafx.h"

DebugRenderer debugRenderer;

static void InitVertex(MeshVertex& v, BONE_ID boneId, DWORD color)
{
	v.blendIndices.x = v.blendIndices.y = v.blendIndices.z = v.blendIndices.w = boneId;
	v.color = color;
	v.blendWeights.x = v.blendWeights.y = v.blendWeights.z = 0;
	v.normal.x = 1;
	v.normal.y = 0;
	v.normal.z = 0;
	v.uv.x = v.uv.y = 0;
	v.xyz.x = v.xyz.y = v.xyz.z = 0;
}

static inline float len(XMVECTOR v) {
	return XMVectorGetX(XMVector3Length(v));
}

void CreateCone(Block& b, XMVECTOR v1, XMVECTOR v2, BONE_ID boneId, DWORD color)
{
	float radius = 0.15f;
	XMVECTOR boneDir = XMVectorSubtract(v2, v1);
	XMVECTOR vRot0 = XMVector3Cross(boneDir, XMVectorSet(0, 0, radius, 0));
	if (len(vRot0) < 0.001f) {
		vRot0 = XMVector3Cross(boneDir, XMVectorSet(0, radius, 0, 0));
	}
	XMVECTOR vRot90 = XMVector3Cross(vRot0, XMVector3Normalize(boneDir));
	XMVECTOR vRotLast = XMVectorAdd(v1, vRot0);
	static const int div = 10;
	for (int j = 0; j < div; j++) {
		MeshVertex vert[3];
		for (auto& it : vert) {
			InitVertex(it, boneId, color);
		}
		float rad = XM_2PI / div * (j + 1);
		XMVECTOR vRot = XMVectorAdd(v1, XMVectorScale(vRot0, cosf(rad)) + XMVectorScale(vRot90, sinf(rad)));
		XMStoreFloat3(&vert[0].xyz, vRotLast);
		XMStoreFloat3(&vert[1].xyz, v2);
		XMStoreFloat3(&vert[2].xyz, vRot);
		XMVECTOR normal = XMVector3Cross(XMVectorSubtract(vRotLast, v2), XMVectorSubtract(v2, vRot));
		for (auto& it : vert) {
			XMStoreFloat3(&it.normal, normal);
			b.vertices.push_back(it);
			b.indices.push_back(b.indices.size());
		}
		vRotLast = vRot;
	}
}

void DebugRenderer::CreatePivotMesh()
{
	for (BONE_ID i = 0; (unsigned)i < BONE_MAX; i++)	{
		XMVECTOR v = XMVectorZero();
		float len = 50.0f;
		CreateCone(pivots, v, v + XMVectorSet(len, 0, 0, 0), i, 0xff0000ff);
		CreateCone(pivots, v, v + XMVectorSet(0, len, 0, 0), i, 0xff00ff00);
		CreateCone(pivots, v, v + XMVectorSet(0, 0, len, 0), i, 0xffff0000);
	}

	int sizeVertices = pivots.vertices.size() * sizeof(pivots.vertices[0]);
	int sizeIndices = pivots.indices.size() * sizeof(pivots.indices[0]);
	if (sizeVertices && sizeIndices) {
		pivotsRenderer.Init(sizeVertices, sizeIndices, &pivots.vertices[0], &pivots.indices[0]);
	}

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
	mat.tmid = texMan.Create("white.bmp", true);

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

void DebugRenderer::DrawPivots(const XMMATRIX mat[BONE_MAX], int num)
{
	int facesBackup = pivots.materialMaps.begin()->faces;
	pivots.materialMaps.begin()->faces = facesBackup * num / BONE_MAX;

	pivotsRenderer.Draw(mat, num, pivots);

	pivots.materialMaps.begin()->faces = facesBackup;
}

void DebugRenderer::Destroy()
{
	pivotsRenderer.Destroy();
}
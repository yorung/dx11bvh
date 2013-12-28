#include "stdafx.h"

static float CalcRadius(const Mesh* m)
{
	const Block& b = m->GetRawDatas();
	float maxSq = 0;
	for (auto& it : b.vertices) {
		float sq = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&it.xyz)));
		maxSq = std::max(maxSq, sq);
	}
	return sqrt(maxSq);
}

App::App() : scale(1), lastX(-1), lastY(-1), sprite(nullptr), font(nullptr)
{
	quat = XMQuaternionIdentity();
	ZeroMemory(mesh, sizeof(mesh));
}

App::~App()
{
}

void App::Init(const char* fileName)
{
	Destroy();

	sprite = new SpriteBatch(deviceMan11.GetContext());
	font = new SpriteFont(deviceMan11.GetDevice(), L"resource\\font.spritefont");

	if (fileName) {
		const char* ext = strrchr(fileName, '.');
		if (ext && !_stricmp(ext, ".bvh")) {
			mesh[0] = new Bvh(fileName);
		} else {
			mesh[0] = new MeshX(fileName);
		}
	} else {
		mesh[0] = new Bvh("D:\\github\\aachan.bvh");
		mesh[1] = new Bvh("D:\\github\\kashiyuka.bvh");
		mesh[2] = new Bvh("D:\\github\\nocchi.bvh");
	}

	float radius = CalcRadius(mesh[0]);
	scale = 1 / std::max(0.00001f, radius);

	matrixMan.Set(MatrixMan::PROJ, XMMatrixPerspectiveFovLH(45 * XM_PI / 180, (float)SCR_W / SCR_H, 0.1f, 1000.0f));
}

void App::MouseWheel(float delta)
{
	if (delta > 0) {
		scale *= 1.1f;
	}
	if (delta < 0) {
		scale /= 1.1f;
	}
}

void App::LButtonDown(float x, float y)
{
	lastX = x;
	lastY = y;
}

void App::LButtonUp(float x, float y)
{
	MouseMove(x, y);
	lastX = lastY = -1;
}

void App::MouseMove(float x, float y)
{
	if (lastX < 0 || lastY < 0) {
		return;
	}
	float dx = x - lastX;
	float dy = - (y - lastY);

	lastX = x;
	lastY = y;

	XMVECTOR axis = XMVectorSet(-dy, dx, 0, 0);
	float len = XMVectorGetX(XMVector2Length(axis));
	if (!len) {
		return;
	}

	XMVECTOR q = XMQuaternionRotationAxis(axis, len * -2 * XM_PI);
	quat = XMQuaternionMultiply(quat, q);
}

void App::Draw()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	float time = (float)((double)t.QuadPart / f.QuadPart);

//	XMMATRIX mRot = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), time / 2 * XM_PI));
	XMMATRIX mRot = XMMatrixRotationQuaternion(quat);
	XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);

	matrixMan.Set(MatrixMan::WORLD, mScale * mRot);

	float dist = 3;
//	float rot = time / 5 * XM_PI;
	float rot = XM_PI;
	matrixMan.Set(MatrixMan::VIEW, XMMatrixLookAtLH(XMVectorSet(sin(rot) * dist, 0, cos(rot) * dist, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0)));

	for (auto& it : mesh) {
		if (it) {
			it->Draw(0, time);
		}
	}

    sprite->Begin();
    XMFLOAT2 pos;
    pos.x = SCR_W / 2;
    pos.y = SCR_H / 2;
    font->DrawString(sprite, L"Font Test", pos);
    sprite->End();
}

void App::Destroy()
{
	SAFE_DELETE(mesh[0]);
	SAFE_DELETE(mesh[1]);
	SAFE_DELETE(mesh[2]);
	SAFE_DELETE(font);
	SAFE_DELETE(sprite);
}

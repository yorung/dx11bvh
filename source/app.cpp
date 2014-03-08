#include "stdafx.h"

static float INVALID_POS = -99999.f;

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

App::App() : scale(1), lastX(INVALID_POS), lastY(INVALID_POS), sprite(nullptr), font(nullptr)
{
	ZeroMemory(mesh, sizeof(mesh));
}

App::~App()
{
}

void App::Init(const char* fileName)
{
	Destroy();

	gridRenderer.Init();
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
	scale = std::max(0.00001f, radius);

	height = radius / 2;
}

void App::MouseWheel(float delta)
{
	if (delta > 0) {
		scale /= 1.1f;
	}
	if (delta < 0) {
		scale *= 1.1f;
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
	lastX = lastY = INVALID_POS;
}

void App::MouseMove(float x, float y)
{
	if (lastX <= INVALID_POS || lastY <= INVALID_POS) {
		return;
	}
	rotX += (x - lastX) * XM_PI * 2.0f;
	rotY += (y - lastY) * XM_PI * 2.0f;

	lastX = x;
	lastY = y;
}

inline XMFLOAT2 GetScreenPos(const XMMATRIX& mLocal)
{
	XMFLOAT4X4 mViewport;
	XMMATRIX mW, mV, mP;
	matrixMan.Get(MatrixMan::WORLD, mW);
	matrixMan.Get(MatrixMan::VIEW, mV);
	matrixMan.Get(MatrixMan::PROJ, mP);
	XMStoreFloat4x4(&mViewport, XMMatrixIdentity());
	mViewport._11 = SCR_W / 2;
	mViewport._22 = -SCR_H / 2;
	mViewport._41 = SCR_W / 2;
	mViewport._42 = SCR_H / 2;

	XMMATRIX m = mLocal * mW * mV * mP * XMLoadFloat4x4(&mViewport);

	XMFLOAT2 p;
	p.x = XMVectorGetX(m.r[3]) / XMVectorGetW(m.r[3]);
	p.y = XMVectorGetY(m.r[3]) / XMVectorGetW(m.r[3]);
	return p;
}

void App::DrawBoneNames(Bvh* bvh)
{
	const std::vector<BvhFrame>& frames = bvh->GetFrames();
	for (auto& it : frames) {
		if (it.childId < 0) {
			continue;
		}
		XMFLOAT2 pos = GetScreenPos(XMLoadFloat4x4(&it.result));

		WCHAR wname[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, it.name, -1, wname, dimof(wname));

		XMVECTOR size = font->MeasureString(wname);
		pos.x -= XMVectorGetX(size) / 2;
		pos.y -= XMVectorGetY(size) / 2;
		XMFLOAT2 origin = {0, 0};
		font->DrawString(sprite, wname, pos, Colors::Black, 0, origin, 0.7f);
		pos.x += 1.0f;
		pos.y += 1.0f;
		font->DrawString(sprite, wname, pos, Colors::White, 0, origin, 0.7f);
	}
}

void App::Draw()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	double time = ((double)t.QuadPart / f.QuadPart);

	matrixMan.Set(MatrixMan::WORLD, Matrix());

	float dist = 3 * scale;
	Matrix cam = translate(0, height, -dist) * q2m(Quat(Vec3(1,0,0), rotY)) * q2m(Quat(Vec3(0,1,0), rotX));
	matrixMan.Set(MatrixMan::VIEW, (Matrix)inv(cam));
	matrixMan.Set(MatrixMan::PROJ, XMMatrixPerspectiveFovLH(45 * XM_PI / 180, (float)SCR_W / SCR_H, dist / 1000, dist * 1000));

	sprite->Begin();

	for (auto& it : mesh) {
		if (it) {
			it->Draw(0, time);

			Bvh* bvh = dynamic_cast<Bvh*>(it);
			if (bvh) {
				DrawBoneNames(bvh);
			}
		}
	}

	sprite->End();

	gridRenderer.Draw();
}

void App::Destroy()
{
	SAFE_DELETE(mesh[0]);
	SAFE_DELETE(mesh[1]);
	SAFE_DELETE(mesh[2]);
	SAFE_DELETE(font);
	SAFE_DELETE(sprite);
	gridRenderer.Destroy();
}

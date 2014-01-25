#include "stdafx.h"

#pragma comment(lib, "winmm.lib")

static double GetTime()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / f.QuadPart;
}

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

App::App() : scale(1), lastX(-1), lastY(-1), sprite(nullptr), font(nullptr), meshTiny(nullptr), animationNumber(0)
{
	quat = XMQuaternionIdentity();
	ZeroMemory(mesh, sizeof(mesh));
	startTime = GetTime();
}

App::~App()
{
}

void App::Init(const char* fileName)
{
	Destroy();

	debugRenderer.Init();

	g_type = "mesh";

	sprite = new SpriteBatch(deviceMan11.GetContext());
	font = new SpriteFont(deviceMan11.GetDevice(), L"resource\\font.spritefont");

	meshTiny = new MeshX("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\Media\\Tiny\\tiny.x");

	if (fileName) {
		const char* ext = strrchr(fileName, '.');
		if (ext && !_stricmp(ext, ".bvh")) {
			mesh[0] = new Bvh(fileName);
		} else {
			mesh[0] = new MeshX(fileName);
		}
	} else {
		mesh[0] = new Bvh("D:\\github\\aachan.bvh");
	//	mesh[1] = new Bvh("D:\\github\\kashiyuka.bvh");
	//	mesh[2] = new Bvh("D:\\github\\nocchi.bvh");
	}

	float radius = CalcRadius(mesh[0]);
	scale = 1 / std::max(0.00001f, radius);

	matrixMan.Set(MatrixMan::PROJ, XMMatrixPerspectiveFovLH(45 * XM_PI / 180, (float)SCR_W / SCR_H, 0.1f, 1000.0f));

	dynamic_cast<Bvh*>(mesh[0])->FixBones("Chest");
	dynamic_cast<Bvh*>(mesh[0])->LinkTo("RightHip", "Chest");
	dynamic_cast<Bvh*>(mesh[0])->LinkTo("LeftHip", "Chest");
	dynamic_cast<Bvh*>(mesh[0])->FixBones("Neck");
	dynamic_cast<Bvh*>(mesh[0])->LinkTo("RightCollar", "Neck");
	dynamic_cast<Bvh*>(mesh[0])->LinkTo("LeftCollar", "Neck");

	dynamic_cast<Bvh*>(mesh[0])->ResetAnim();
	meshTiny->SyncLocalAxisWithBvh(dynamic_cast<Bvh*>(mesh[0]));

	startTime = GetTime();
//	PlaySoundA("D:\\github\\Perfume_globalsite_sound.wav", NULL, SND_FILENAME | SND_ASYNC);
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

void App::DrawBoneNames(MeshX* meshX)
{
	const std::vector<Frame>& frames = meshX->GetFrames();
	for (auto& it : frames) {
		XMFLOAT2 pos = GetScreenPos(XMLoadFloat4x4(&it.result));

		WCHAR wname[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, it.name, -1, wname, dimof(wname));
		if (wname[0] == '\0') {
			wcscpy(wname, L"[NO NAME]");
		}

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

void App::Update()
{
	if (GetKeyState('P') & 0x80) {
		g_type = "pivot";
	}
	if (GetKeyState('M') & 0x80) {
		g_type = "mesh";
	}
	if (GetKeyState('B') & 0x80) {
		g_type = "bone";
	}
	for (auto& i : {'0', '1', '2', '3', '4'}) {
		if (GetKeyState(i) & 0x80) {
			animationNumber = i - '0';
		}
	}
}

void App::Draw()
{
	double time = GetTime() - startTime;
//	double time = 0;

//	XMMATRIX mRot = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), time / 2 * XM_PI));
	XMMATRIX mRot = XMMatrixRotationQuaternion(quat);
	XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);

	matrixMan.Set(MatrixMan::WORLD, mScale * mRot);

	float dist = 3;
//	float rot = time / 5 * XM_PI;
	float rot = XM_PI;
	matrixMan.Set(MatrixMan::VIEW, XMMatrixLookAtLH(XMVectorSet(sin(rot) * dist, 0, cos(rot) * dist, 1), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0)));

    sprite->Begin();

	for (auto& it : mesh) {
		if (it && meshTiny) {
			it->Draw(0, time);

			Bvh* bvh = dynamic_cast<Bvh*>(it);

			if (bvh) {
				meshTiny->DrawBvh(bvh, time);
			//	meshTiny->Draw(animationNumber, time);
				if (GetKeyState('T') & 0x01) {
					DrawBoneNames(bvh);
				}
			}
		}
	}

	MeshX* x = dynamic_cast<MeshX*>(meshTiny);
	if (x && (GetKeyState('T') & 0x01)) {
		DrawBoneNames(x);
	}

	sprite->End();
}

void App::Destroy()
{
	SAFE_DELETE(mesh[0]);
	SAFE_DELETE(mesh[1]);
	SAFE_DELETE(mesh[2]);
	SAFE_DELETE(meshTiny);
	SAFE_DELETE(font);
	SAFE_DELETE(sprite);

	debugRenderer.Destroy();
}


std::string g_type;

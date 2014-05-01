#include "stdafx.h"

static double GetTime()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / f.QuadPart;
}
static float INVALID_POS = -99999.f;

static float CalcRadius(const Mesh* m)
{
	const Block& b = m->GetRawDatas();
	float maxSq = 0;
	for (auto& it : b.vertices) {
		float sq = lengthSq(it.xyz);
		maxSq = std::max(maxSq, sq);
	}
	return sqrt(maxSq);
}

App::App() : scale(1), radius(1), lastX(INVALID_POS), lastY(INVALID_POS), sprite(nullptr), font(nullptr), animationNumber(0), trackTime(0), meshTiny(nullptr)
{
	ZeroMemory(mesh, sizeof(mesh));
	lastTime = GetTime();
}

App::~App()
{
}

void App::Init(const char* fileName)
{
	Destroy();

	debugRenderer.Init();
	gridRenderer.Init();

	g_type = "mesh";

	sprite = new SpriteBatch(deviceMan11.GetContext());
	font = new SpriteFont(deviceMan11.GetDevice(), L"resource\\font.spritefont");
	meshTiny = new MeshX("resource\\tiny.x");

	if (fileName) {
		const char* ext = strrchr(fileName, '.');
		if (ext && !_stricmp(ext, ".bvh")) {
			Bvh* bvh = new Bvh(fileName);
			mesh[0] = bvh;
			bvh->ResetAnim();
			meshTiny->SyncLocalAxisWithBvh(bvh, bind[0]);
		} else {
			mesh[0] = new MeshX(fileName);
		}
	} else {
		const char* bvhNames[] = {
			"D:\\github\\aachan.bvh",
			"D:\\github\\kashiyuka.bvh",
			"D:\\github\\nocchi.bvh",
		};
		for (int i = 0; i < dimof(bvhNames); i++) {
			Bvh* bvh = new Bvh(bvhNames[i]);
			mesh[i] = bvh;
			bvh->ResetAnim();
			meshTiny->SyncLocalAxisWithBvh(bvh, bind[i]);
		}
	}

	radius = CalcRadius(mesh[0]);
	scale = std::max(0.00001f, radius);

	height = radius / 2;

	lastTime = GetTime();
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

inline Vec2 GetScreenPos(const Mat& mLocal)
{
	Mat mW, mV, mP, mViewport;
	matrixMan.Get(MatrixMan::WORLD, mW);
	matrixMan.Get(MatrixMan::VIEW, mV);
	matrixMan.Get(MatrixMan::PROJ, mP);
	mViewport._11 = SCR_W / 2;
	mViewport._22 = -SCR_H / 2;
	mViewport._41 = SCR_W / 2;
	mViewport._42 = SCR_H / 2;

	Mat m = mLocal * mW * mV * mP * mViewport;

	Vec2 p;
	p.x = m._41 / m._44;
	p.y = m._42 / m._44;
	return p;
}

void App::DrawBoneNames(Bvh* bvh)
{
	const std::vector<BvhFrame>& frames = bvh->GetFrames();
	for (auto& it : frames) {
		if (it.childId < 0) {
			continue;
		}
		Vec2 pos = GetScreenPos(it.result);

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

void App::DrawBoneNames(const MeshX* meshX, const MeshXAnimResult& result)
{
	const std::vector<Frame>& frames = meshX->GetFrames();
	for (BONE_ID id = 0; id < (BONE_ID)frames.size(); id++) {
		const Frame& f = frames[id];
		Vec2 pos = GetScreenPos(result.boneMat[id]);

		WCHAR wname[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, f.name, -1, wname, dimof(wname));
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
	for (auto& i : {'0', '1', '2', '3', '4', '9'}) {
		if (GetKeyState(i) & 0x80) {
			animationNumber = i - '0';
		}
	}
}

void App::Draw()
{
	double currentTime = GetTime();
	double deltaTime = currentTime - lastTime;
	lastTime = currentTime;


	if (GetKeyState(VK_RETURN) & 0x01) {
		trackTime += deltaTime;
	}

	matrixMan.Set(MatrixMan::WORLD, Mat());

	float dist = 3 * scale;
	Mat cam = translate(0, height, -dist) * q2m(Quat(Vec3(1,0,0), rotY)) * q2m(Quat(Vec3(0,1,0), rotX));
	matrixMan.Set(MatrixMan::VIEW, inv(cam));
	matrixMan.Set(MatrixMan::PROJ, XMMatrixPerspectiveFovLH(45 * XM_PI / 180, (float)SCR_W / SCR_H, dist / 1000, dist * 1000));

	sprite->Begin();

	for (int i = 0; i < dimof(mesh); i++) {
		Mesh* it = mesh[i];
		MeshX* meshX = meshTiny;
		MeshXAnimResult meshXAnimResult;
		if (it && meshX) {

			Bvh* bvh = dynamic_cast<Bvh*>(it);

			if (bvh) {
				bvh->Draw(animationNumber == 9 ? 0 : animationNumber, trackTime);
				if (animationNumber == 9) {
					meshX->CalcAnimationFromBvh(bvh, bind[i], trackTime, meshXAnimResult, 270 / radius);
				} else {
					meshX->CalcAnimation(animationNumber, trackTime, meshXAnimResult);
				}
				meshX->Draw(meshXAnimResult);
				if (GetKeyState('T') & 0x01) {
					DrawBoneNames(bvh);
				}
			}
		}
		if (meshX && (GetKeyState('T') & 0x01)) {
			DrawBoneNames(meshX, meshXAnimResult);
		}
	}

	sprite->End();

	gridRenderer.Draw();
}

void App::Destroy()
{
	for (auto& it : mesh) {
		SAFE_DELETE(it);
	}
	SAFE_DELETE(meshTiny);
	SAFE_DELETE(font);
	SAFE_DELETE(sprite);

	debugRenderer.Destroy();
	gridRenderer.Destroy();
}


std::string g_type;

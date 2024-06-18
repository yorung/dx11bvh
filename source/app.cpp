#include "stdafx.h"

#ifndef _MSC_VER
static int GetKeyState(char) {
	return 0x01;
}
#define VK_RETURN 0x0D
#endif

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

App::App() : radius(1), animationNumber(0), trackTime(0), meshTiny(nullptr)
{
	memset(mesh, 0, sizeof(mesh));
	lastTime = GetTime();
}

App::~App()
{
}

void App::Init(const char* fileName)
{
	Destroy();

	fontMan.Init();
	debugRenderer.Init();
	gridRenderer.Create(20, 100.f);
	computeShaderSkinning.Create("hlsl/skin_cs.hlsl");

	g_type = "mesh";

	const char* meshFileName = "resource/external/tiny.x";
	const char* ext = fileName ? strrchr(fileName, '.') : nullptr;
	if (ext && !_stricmp(ext, ".x")) {
		meshFileName = fileName;
	}
#if 1
	meshTiny = new MeshX(meshFileName);

	if (ext && !_stricmp(ext, ".bvh")) {
		Bvh* bvh = new Bvh(fileName);
		mesh[0] = bvh;
		bvh->ResetAnim();
		meshTiny->SyncLocalAxisWithBvh(bvh, bind[0]);
	} else {
		const char* bvhNames[] = {
			"resource/external/aachan.bvh",
			"resource/external/kashiyuka.bvh",
			"resource/external/nocchi.bvh",
		};
		for (int i = 0; i < (int)dimof(bvhNames); i++) {
			Bvh* bvh = new Bvh(bvhNames[i]);
			mesh[i] = bvh;
			bvh->ResetAnim();
			meshTiny->SyncLocalAxisWithBvh(bvh, bind[i]);
		}
	}

	if (mesh[0]) {
		radius = CalcRadius(mesh[0]);
	}
#endif
	float scale = std::max(0.00001f, radius);
	devCamera.SetDistance(1072.f);
//	devCamera.SetDistance(scale * 3);
	devCamera.SetHeight(radius / 2);

	PlaySoundA("resource/external/Perfume_globalsite_sound.wav", NULL, SND_FILENAME | SND_ASYNC);
	trackTime = 0;

	lastTime = GetTime();
}

inline Vec2 GetScreenPos(const Mat& mLocal)
{
	Mat mV, mP, mViewport;
	matrixMan.Get(MatrixMan::VIEW, mV);
	matrixMan.Get(MatrixMan::PROJ, mP);
	mViewport = makeViewportMatrix(systemMisc.GetScreenSize());
	Mat m = mLocal * mV * mP * mViewport;

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
		Vec2 size = fontMan.MeasureString(12, it.name);
		Vec2 pos = GetScreenPos(it.result);
		pos -= size / 2;
		fontMan.DrawString(floor(pos), 12, it.name);
	}
}

void App::DrawBoneNames(const MeshX* meshX, const MeshXAnimResult& result)
{
	const std::vector<Frame>& frames = meshX->GetFrames();
	for (BONE_ID id = 0; id < (BONE_ID)frames.size(); id++) {
		const Frame& f = frames[id];
		Vec2 pos = floor(GetScreenPos(result.boneMat[id]));
		fontMan.DrawString(pos, 12, f.name[0] == '\0' ? "[NO NAME]" : f.name);
	}
}

void App::DrawCameraParams()
{
	Mat v;
	matrixMan.Get(MatrixMan::VIEW, v);
	Mat mInv = inv(v);

	char buf[128];
	Vec2 pos = { 5, 105 };
	auto draw = [&]() {
		fontMan.DrawString(pos, 20, buf);
		pos.y += 22;
	};

//	snprintf(buf, sizeof(buf), "cam pos(via inv view):%f, %f, %f dir:%f, %f, %f", mInv._41, mInv._42, mInv._43, mInv._31, mInv._32, mInv._33);
//	draw();

//	mInv = fastInv(v);
//	snprintf(buf, sizeof(buf), "cam pos(by fastInv):%f, %f, %f dir:%f, %f, %f", mInv._41, mInv._42, mInv._43, mInv._31, mInv._32, mInv._33);
//	draw();

//	snprintf(buf, sizeof(buf), "cam dir(view mtx direct): %f, %f, %f", v._13, v._23, v._33);
//	draw();

	snprintf(buf, sizeof(buf), "cam dir: %f, %f, %f", v._13, v._23, v._33);
	draw();

	float longitude = atan2(v._13, v._33) * (180.0f / 3.14159265f);
	float latitude = asin(v._23) * (180.0f / 3.14159265f);
	snprintf(buf, sizeof(buf), "longitude: %f, latitude: %f", longitude, latitude);
	draw();
}

#if 0
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
	static char v[] = "012349";
	for (auto& i : v) {
		if (i && GetKeyState(i) & 0x80) {
			animationNumber = i - '0';
		}
	}
}
#endif
void App::Update()
{
	g_type = "mesh";
	animationNumber = 9;
}

void App::Draw()
{
	AFRenderTarget rt;
	rt.InitForDefaultRenderTarget();
	rt.BeginRenderToThis();
//	fontMan.DrawString(Vec2(0, 110), 16, "TEXT SPRITE TEST!!!!!!!!!!!!!text sprite 1234567890");
//	fontMan.DrawString(Vec2(10, 130), 32, "@#$%^&*()");
//	fontMan.DrawString(Vec2(10, 170), 40, L"あいうえお한글漢字");

	ID3D11DeviceContext* context = deviceMan11.GetContext();

	double currentTime = GetTime();
	double deltaTime = currentTime - lastTime;
	lastTime = currentTime;


//	if (GetKeyState(VK_RETURN) & 0x01) {
		trackTime += deltaTime;
//	}

	matrixMan.Set(MatrixMan::VIEW, devCamera.GetViewMatrix());
	matrixMan.Set(MatrixMan::PROJ, devCamera.GetProjMatrix());

	gridRenderer.Draw();

	for (int i = 0; i < (int)dimof(mesh); i++) {
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

	if (GetKeyState('T') & 0x01) {
		DrawCameraParams();
		char buf[20];
		sprintf(buf, "FPS: %f", fps.Get());
		Vec2 pos = { 5, 15 };
		fontMan.DrawString(pos, 16, buf);
	}

	afBindTextureToBindingPoint(0, 0);

	fontMan.Render();
	deviceMan11.Present();
	fps.Update();
}

void App::Destroy()
{
	for (auto& it : mesh) {
		SAFE_DELETE(it);
	}
	SAFE_DELETE(meshTiny);

	fontMan.Destroy();
	debugRenderer.Destroy();
	gridRenderer.Destroy();
	computeShaderSkinning.Destroy();
	matMan.Destroy();
}


std::string g_type;

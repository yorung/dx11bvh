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

void App::ApplySky()
{
	switch(skyNum) {
	case 0:
		//skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\C++\\Direct3D\\StateManager\\Media\\skybox02.dds", "sky_cubemap_high");
		skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\Media\\Lobby\\LobbyCube.dds", "sky_cubemap");
		break;
	case 1:
		skyMan.Create("resource\\OutputCube2.dds", "projection_equirectangular");
		//skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\C++\\Direct3D\\StateManager\\Media\\skybox02.dds", "projection_equirectangular");
		break;
	case 2:
		skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\C++\\Direct3D\\StateManager\\Media\\skybox02.dds", "projection_little_planet");
		break;
	case 3:
		skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\Media\\trees\\tree35S.dds", "sky_spheremap");
		//skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\Media\\SubD10\\msd_MicrosoftCharacterMaya100_heroChr_mechanicalCM_tga_001.dds", "sky_spheremap");
		//skyMan.Create("resource\\sphere_map.dds", "sky_spheremap");
		break;
	case 4:
		skyMan.Create("resource\\PANO_20141115_141959.dds", "sky_photosphere");
		break;
	case 5:
		skyMan.Create("resource\\Equirectangular-projection.jpg", "sky_photosphere");
		break;
	case 6:
		skyMan.Create("resource\\warcraft.dds", "sky_photosphere");
		break;
	case 7:
		skyMan.Create("resource\\PANO_20141115_141959.dds", "projection_equirectangular_to_stereographic");
		break;
	}
}

void App::Init(const char* fileName)
{
	Destroy();

	for (int i = 0; i < (int)dimof(rt); i++) {
//		rt[i].Init(ivec2(SCR_W, SCR_H), i == 2 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R8G8B8A8_UNORM);
		rt[i].Init(ivec2(SCR_W, SCR_H), DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	fontMan.Init();
	debugRenderer.Init();
	gridRenderer.Init();
	waterSurface.Init();
	postEffectCopy.Create("post_effect_copy");
	postEffectMono.Create("post_effect_mono");
	postEffectUAVTest.Create("post_effect_uav_test");
	computeShaderMan.Create("fx\\post_effect_cs.fx");
	computeShaderSkinning.Create("fx\\skin_cs.fx");

	g_type = "mesh";

	const char* meshFileName = "resource\\tiny.x";
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
			"D:\\github\\aachan.bvh",
			"D:\\github\\kashiyuka.bvh",
			"D:\\github\\nocchi.bvh",
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
	devCamera.SetDistance(scale * 3);
	devCamera.SetHeight(radius / 2);

	lastTime = GetTime();
	ApplySky();
}

inline Vec2 GetScreenPos(const Mat& mLocal)
{
	Mat mV, mP, mViewport;
	matrixMan.Get(MatrixMan::VIEW, mV);
	matrixMan.Get(MatrixMan::PROJ, mP);
	mViewport._11 = SCR_W / 2;
	mViewport._22 = -SCR_H / 2;
	mViewport._41 = SCR_W / 2;
	mViewport._42 = SCR_H / 2;

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

void App::Update()
{
	if (GetKeyState(VK_F1) & 0x80) {
		skyNum = 0;
		ApplySky();
	}
	if (GetKeyState(VK_F2) & 0x80) {
		skyNum = 1;
		ApplySky();
	}
	if (GetKeyState(VK_F3) & 0x80) {
		skyNum = 2;
		ApplySky();
	}
	if (GetKeyState(VK_F4) & 0x80) {
		skyNum = 3;
		ApplySky();
	}
	if (GetKeyState(VK_F5) & 0x80) {
		skyNum = 4;
		ApplySky();
	}
	if (GetKeyState(VK_F6) & 0x80) {
		skyNum = 5;
		ApplySky();
	}
	if (GetKeyState(VK_F7) & 0x80) {
		skyNum = 6;
		ApplySky();
	}
	if (GetKeyState(VK_F8) & 0x80) {
		skyNum = 7;
		ApplySky();
	}

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

void App::Draw()
{
//	fontMan.DrawString(Vec2(0, 110), 16, "TEXT SPRITE TEST!!!!!!!!!!!!!text sprite 1234567890");
//	fontMan.DrawString(Vec2(10, 130), 32, "@#$%^&*()");
//	fontMan.DrawString(Vec2(10, 170), 40, L"あいうえお한글漢字");

	// render to rt[0]
	rt[0].BeginRenderToThis();

	ID3D11DeviceContext* context = deviceMan11.GetContext();

	double currentTime = GetTime();
	double deltaTime = currentTime - lastTime;
	lastTime = currentTime;


	if (GetKeyState(VK_RETURN) & 0x01) {
		trackTime += deltaTime;
	}

	matrixMan.Set(MatrixMan::VIEW, devCamera.GetViewMatrix());
	matrixMan.Set(MatrixMan::PROJ, devCamera.GetProjMatrix());

	gridRenderer.Draw();
	waterSurface.Draw();

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

	skyMan.Draw();

	fontMan.Render();

	// 0 => 1
	deviceMan11.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);	// unbind RT to prevent warnings in debug layer
	computeShaderMan.Draw(rt[0].GetTexture(), rt[1].GetUnorderedAccessView());

	AFRenderTarget defaultTarget;
	defaultTarget.InitForDefaultRenderTarget();

	if (GetKeyState('U') & 0x01) {
		// 1 => 2
		ID3D11UnorderedAccessView* view = rt[2].GetUnorderedAccessView();
		deviceMan11.GetContext()->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 1, 1, &view, nullptr);
		postEffectUAVTest.Draw(rt[1].GetTexture());

		// 2 => front
		defaultTarget.BeginRenderToThis();
		postEffectMono.Draw(rt[2].GetTexture());
	} else {
		// 1 => 0
		rt[0].BeginRenderToThis();
		postEffectCopy.Draw(rt[1].GetTexture());

		// 0 => front
		defaultTarget.BeginRenderToThis();
		postEffectMono.Draw(rt[0].GetTexture());
	}


	ID3D11ShaderResourceView* dummy = nullptr;
	deviceMan11.GetContext()->PSSetShaderResources(0, 1, &dummy);

	deviceMan11.Present();
	fps.Update();
}

void App::Destroy()
{
	for (auto& it : mesh) {
		SAFE_DELETE(it);
	}
	SAFE_DELETE(meshTiny);
	for (auto& it : rt) {
		it.Destroy();
	}

	fontMan.Destroy();
	debugRenderer.Destroy();
	gridRenderer.Destroy();
	waterSurface.Destroy();
	postEffectMono.Destroy();
	postEffectCopy.Destroy();
	postEffectUAVTest.Destroy();
	computeShaderMan.Destroy();
	computeShaderSkinning.Destroy();
	matMan.Destroy();
}


std::string g_type;

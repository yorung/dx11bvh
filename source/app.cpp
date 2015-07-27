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

App::App() : radius(1), animationNumber(0), trackTime(0), meshTiny(nullptr),
	renderTargetView(nullptr), shaderResourceView(nullptr), unorderedAccessView(nullptr),
	renderTargetView2(nullptr), shaderResourceView2(nullptr), unorderedAccessView2(nullptr)
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

	CD3D11_TEXTURE2D_DESC tDesc(DXGI_FORMAT_R8G8B8A8_TYPELESS, SCR_W, SCR_H, 1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
	ID3D11Texture2D* tex;
	ID3D11Texture2D* tex2;
	HRESULT hr = deviceMan11.GetDevice()->CreateTexture2D(&tDesc, NULL, &tex);
	hr = deviceMan11.GetDevice()->CreateTexture2D(&tDesc, NULL, &tex2);
	CD3D11_RENDER_TARGET_VIEW_DESC rDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
	hr = deviceMan11.GetDevice()->CreateRenderTargetView(tex, &rDesc, &renderTargetView);
	hr = deviceMan11.GetDevice()->CreateRenderTargetView(tex2, &rDesc, &renderTargetView2);
	CD3D11_SHADER_RESOURCE_VIEW_DESC sDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
	hr = deviceMan11.GetDevice()->CreateShaderResourceView(tex, &sDesc, &shaderResourceView);
	hr = deviceMan11.GetDevice()->CreateShaderResourceView(tex2, &sDesc, &shaderResourceView2);
	CD3D11_UNORDERED_ACCESS_VIEW_DESC uDesc(D3D11_UAV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
	hr = deviceMan11.GetDevice()->CreateUnorderedAccessView(tex, &uDesc, &unorderedAccessView);
	hr = deviceMan11.GetDevice()->CreateUnorderedAccessView(tex2, &uDesc, &unorderedAccessView2);
	SAFE_RELEASE(tex);
	SAFE_RELEASE(tex2);

	fontMan.Init();
	debugRenderer.Init();
	gridRenderer.Init();
	waterSurface.Init();
	postEffectMan.Create("post_effect_mono");
	computeShaderMan.Create("fx\\post_effect_cs.fx");
	computeShaderSkinning.Create("fx\\skin_cs.fx");

	g_type = "mesh";

	const char* meshFileName = "resource\\tiny.x";
	const char* ext = fileName ? strrchr(fileName, '.') : nullptr;
	if (ext && !_stricmp(ext, ".x")) {
		meshFileName = fileName;
	}

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

	radius = CalcRadius(mesh[0]);
	float scale = std::max(0.00001f, radius);
	devCamera.SetDistance(scale * 3);
	devCamera.SetHeight(radius / 2);

	//	skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\C++\\Direct3D\\StateManager\\Media\\skybox02.dds", "sky_cubemap");
//	skyMan.Create("resource\\Tiny_skin.dds", "sky_spheremap");
	skyMan.Create("resource\\PANO_20141115_141959.dds", "sky_photosphere");

	lastTime = GetTime();
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
	Vec2 pos = { 5, 55 };
	snprintf(buf, sizeof(buf), "cam pos(via inv view):%f, %f, %f dir:%f, %f, %f", mInv._41, mInv._42, mInv._43, mInv._31, mInv._32, mInv._33);
	fontMan.DrawString(pos, 16, buf);

	mInv = fastInv(v);
	snprintf(buf, sizeof(buf), "cam pos(by fastInv):%f, %f, %f dir:%f, %f, %f", mInv._41, mInv._42, mInv._43, mInv._31, mInv._32, mInv._33);
	pos.y = 75;
	fontMan.DrawString(pos, 16, buf);

	snprintf(buf, sizeof(buf), "cam dir(view mtx direct): %f, %f, %f", v._13, v._23, v._33);
	pos.y = 95;
	fontMan.DrawString(pos, 16, buf);
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
	static char v[] = "012349";
	for (auto& i : v) {
		if (i && GetKeyState(i) & 0x80) {
			animationNumber = i - '0';
		}
	}
}

void App::Draw()
{
	fontMan.DrawString(Vec2(0, 110), 16, "TEXT SPRITE TEST!!!!!!!!!!!!!text sprite 1234567890");
	fontMan.DrawString(Vec2(10, 130), 32, "@#$%^&*()");
	fontMan.DrawString(Vec2(10, 170), 40, L"あいうえお한글漢字");

	ID3D11DeviceContext* context = deviceMan11.GetContext();
	ID3D11RenderTargetView* defaultRenderTarget;
	ID3D11DepthStencilView* defaultDepthStencil;
	context->OMGetRenderTargets(1, &defaultRenderTarget, &defaultDepthStencil);
	context->OMSetRenderTargets(1, &renderTargetView, defaultDepthStencil);
	context->ClearDepthStencilView(defaultDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float clearColor2[4] = { 0.0f, 0.2f, 0.0f, 0.0f };
	context->ClearRenderTargetView(renderTargetView, clearColor2);

	double currentTime = GetTime();
	double deltaTime = currentTime - lastTime;
	lastTime = currentTime;


	if (GetKeyState(VK_RETURN) & 0x01) {
		trackTime += deltaTime;
	}

	matrixMan.Set(MatrixMan::WORLD, Mat());
	matrixMan.Set(MatrixMan::VIEW, devCamera.CalcViewMatrix());

	//if (GetKeyState(VK_RETURN) & 0x01) {
	//	Mat dx = XMMatrixPerspectiveFovLH(45 * XM_PI / 180, (float)SCR_W / SCR_H, dist / 1000, dist * 1000);
	//}

	float dist = devCamera.GetDistance();

	float f = dist * 1000;
	float n = dist / 1000;

	matrixMan.Set(MatrixMan::PROJ, perspective(45, (float)SCR_W / SCR_H, n, f));

	skyMan.Draw();
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

	DrawCameraParams();

	char buf[20];
	sprintf(buf, "FPS: %f", fps.Get());
	Vec2 pos = { 5, 15 };
	fontMan.DrawString(pos, 16, buf);

	fontMan.Render();

	AFRenderTarget defaultTarget;
	defaultTarget.InitForDefaultRenderTarget();
	defaultTarget.BeginRenderToThis();

	computeShaderMan.Draw(shaderResourceView, unorderedAccessView2);
	postEffectMan.Draw(shaderResourceView2);

	deviceMan11.Present();

	fps.Update();

	SAFE_RELEASE(defaultRenderTarget);
	SAFE_RELEASE(defaultDepthStencil);
}

void App::Destroy()
{
	for (auto& it : mesh) {
		SAFE_DELETE(it);
	}
	SAFE_DELETE(meshTiny);
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(shaderResourceView);
	SAFE_RELEASE(unorderedAccessView);
	SAFE_RELEASE(renderTargetView2);
	SAFE_RELEASE(shaderResourceView2);
	SAFE_RELEASE(unorderedAccessView2);

	fontMan.Destroy();
	debugRenderer.Destroy();
	gridRenderer.Destroy();
	waterSurface.Destroy();
	postEffectMan.Destroy();
	computeShaderMan.Destroy();
	computeShaderSkinning.Destroy();
}


std::string g_type;

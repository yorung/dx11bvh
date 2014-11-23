#include "stdafx.h"

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

App::App() : scale(1), radius(1), lastX(INVALID_POS), lastY(INVALID_POS), sprite(nullptr), font(nullptr), animationNumber(0), trackTime(0), meshTiny(nullptr),
	renderTargetView(nullptr), shaderResourceView(nullptr), unorderedAccessView(nullptr),
	renderTargetView2(nullptr), shaderResourceView2(nullptr), unorderedAccessView2(nullptr)
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

	debugRenderer.Init();
	gridRenderer.Init();
	waterSurface.Init();
	postEffectMan.Create("fx\\post_effect_mono.fx");
	computeShaderMan.Create("fx\\post_effect_cs.fx");

	g_type = "mesh";

	const char* meshFileName = "resource\\tiny.x";
	const char* ext = fileName ? strrchr(fileName, '.') : nullptr;
	if (ext && !_stricmp(ext, ".x")) {
		meshFileName = fileName;
	}

	sprite = new SpriteBatch(deviceMan11.GetContext());
	font = new SpriteFont(deviceMan11.GetDevice(), L"resource\\font.spritefont");
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

	//	skyMan.Create("C:\\Program Files (x86)\\Microsoft DirectX SDK (August 2009)\\Samples\\C++\\Direct3D\\StateManager\\Media\\skybox02.dds", "fx\\sky_cubemap.fx");
//	skyMan.Create("resource\\Tiny_skin.dds", "fx\\sky_spheremap.fx");
	skyMan.Create("resource\\PANO_20141115_141959.dds", "fx\\sky_photosphere.fx");

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

	waterSurface.CreateRipple();
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

void App::DrawCameraParams()
{
	Mat v;
	matrixMan.Get(MatrixMan::VIEW, v);
	Mat mInv = inv(v);

	char buf[128];
	WCHAR wBuf[128];
	XMFLOAT2 origin = {0, 0};

	sprintf(buf, "cam pos(via inv view):%f, %f, %f dir:%f, %f, %f", mInv._41, mInv._42, mInv._43, mInv._31, mInv._32, mInv._33);
	MultiByteToWideChar(CP_ACP, 0, buf, -1, wBuf, dimof(wBuf));
	Vec2 pos = {5, 5};
	font->DrawString(sprite, wBuf, pos, Colors::White, 0, origin, 1.0f);

	mInv = fastInv(v);
	sprintf(buf, "cam pos(by fastInv):%f, %f, %f dir:%f, %f, %f", mInv._41, mInv._42, mInv._43, mInv._31, mInv._32, mInv._33);
	MultiByteToWideChar(CP_ACP, 0, buf, -1, wBuf, dimof(wBuf));
	pos.y = 25;
	font->DrawString(sprite, wBuf, pos, Colors::White, 0, origin, 1.0f);

	sprintf(buf, "cam dir(view mtx direct): %f, %f, %f", v._13, v._23, v._33);
	MultiByteToWideChar(CP_ACP, 0, buf, -1, wBuf, dimof(wBuf));
	pos.y = 45;
	font->DrawString(sprite, wBuf, pos, Colors::White, 0, origin, 1.0f);
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
	ID3D11DeviceContext* context = deviceMan11.GetContext();
	ID3D11RenderTargetView* defaultRenderTarget;
	ID3D11DepthStencilView* defaultDepthStencil;
	deviceMan11.BeginScene();
	context->OMGetRenderTargets(1, &defaultRenderTarget, &defaultDepthStencil);
	context->OMSetRenderTargets(1, &renderTargetView, defaultDepthStencil);

	float clearColor[4] = { 0.2f, 0.0f, 0.2f, 0.0f };
	context->ClearRenderTargetView(defaultRenderTarget, clearColor);
	float clearColor2[4] = { 0.0f, 0.2f, 0.0f, 0.0f };
	context->ClearRenderTargetView(renderTargetView, clearColor2);

	double currentTime = GetTime();
	double deltaTime = currentTime - lastTime;
	lastTime = currentTime;


	if (GetKeyState(VK_RETURN) & 0x01) {
		trackTime += deltaTime;
	}

	matrixMan.Set(MatrixMan::WORLD, Mat());

	float dist = 3 * scale;
	Mat cam = translate(0, height, -dist) * q2m(Quat(Vec3(1,0,0), rotY)) * q2m(Quat(Vec3(0,1,0), rotX));
	matrixMan.Set(MatrixMan::VIEW, fastInv(cam));
	matrixMan.Set(MatrixMan::PROJ, XMMatrixPerspectiveFovLH(45 * XM_PI / 180, (float)SCR_W / SCR_H, dist / 1000, dist * 1000));

	skyMan.Draw();
	gridRenderer.Draw();
	waterSurface.Draw();

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


	DrawCameraParams();

	sprite->End();

//	auto shaderResourceView2 = texMan.Get(texMan.Create("resource\\PANO_20141115_141959.dds", true));
//	auto shaderResourceView2 = texMan.Get(texMan.Create("resource\\Tiny_skin.dds", true));
	context->OMSetRenderTargets(1, &defaultRenderTarget, NULL);
	computeShaderMan.Draw(shaderResourceView, unorderedAccessView2);

	postEffectMan.Draw(shaderResourceView2);

	deviceMan11.EndScene();

	SAFE_RELEASE(defaultRenderTarget);
	SAFE_RELEASE(defaultDepthStencil);
}

void App::Destroy()
{
	for (auto& it : mesh) {
		SAFE_DELETE(it);
	}
	SAFE_DELETE(meshTiny);
	SAFE_DELETE(font);
	SAFE_DELETE(sprite);
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(shaderResourceView);
	SAFE_RELEASE(unorderedAccessView);
	SAFE_RELEASE(renderTargetView2);
	SAFE_RELEASE(shaderResourceView2);
	SAFE_RELEASE(unorderedAccessView2);

	debugRenderer.Destroy();
	gridRenderer.Destroy();
	waterSurface.Destroy();
	postEffectMan.Destroy();
	computeShaderMan.Destroy();
}


std::string g_type;

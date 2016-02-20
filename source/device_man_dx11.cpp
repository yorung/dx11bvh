#include "stdafx.h"

#ifdef __d3d11_h__

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")

DeviceMan11 deviceMan11;

DeviceMan11::~DeviceMan11()
{
	Destroy();
}

void DeviceMan11::Create(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = rc.right - rc.left;
	sd.BufferDesc.Height = rc.bottom - rc.top;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3 };
	D3D_FEATURE_LEVEL supportLevel;
#ifdef _DEBUG
	UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT flags = 0;
#endif
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, dimof(featureLevels), D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &supportLevel, &pImmediateContext);

	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	ID3D11Texture2D* pDepthStencil;
	pDevice->CreateTexture2D(&CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_D24_UNORM_S8_UINT, sd.BufferDesc.Width, sd.BufferDesc.Height, 1, 0, D3D11_BIND_DEPTH_STENCIL), nullptr, &pDepthStencil);
	pDevice->CreateDepthStencilView(pDepthStencil, &CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT), &pDepthStencilView);
	SAFE_RELEASE(pDepthStencil);

	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

	D3D11_VIEWPORT vp = { 0, 0, (float)sd.BufferDesc.Width, (float)sd.BufferDesc.Height, 0.0f, 1.0f };
	pImmediateContext->RSSetViewports(1, &vp);
}

void DeviceMan11::Present()
{
	pSwapChain->Present(0, 0);
}

void DeviceMan11::Destroy()
{
	if(pImmediateContext) {
		pImmediateContext->ClearState();
	}
	SAFE_RELEASE(pImmediateContext);
	SAFE_RELEASE(pRenderTargetView);
	SAFE_RELEASE(pDepthStencilView);
	SAFE_RELEASE(pSwapChain);

	if (pDevice) {
		int cnt;
		cnt = pDevice->Release();
		if (cnt) {
			ComPtr<ID3D11Debug> dbg;
			pDevice->QueryInterface(IID_PPV_ARGS(&dbg));
			dbg->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			dbg.Reset();
			MessageBoxA(GetActiveWindow(), SPrintf("%d DX11 interface leak detected.", cnt), "DX11 leaks", MB_OK);
		}
		pDevice = nullptr;
	}
}

#endif

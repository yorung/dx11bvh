class DeviceMan11
{
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pImmediateContext = nullptr;
	ID3D11RenderTargetView*	pRenderTargetView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;
public:
	~DeviceMan11();

	void Create(HWND hWnd);
	void Destroy();

	void Present();

	ID3D11Device* GetDevice() { return pDevice; }
	ID3D11DeviceContext* GetContext() { return pImmediateContext; }
	ID3D11RenderTargetView*	GetDefaultRenderTarget() { return pRenderTargetView; }
	ID3D11DepthStencilView* GetDefaultDepthStencil() { return pDepthStencilView; }
};

extern DeviceMan11 deviceMan11;

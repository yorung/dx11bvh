struct SolidConstantBuffer
{
	Matrix matW;
	Matrix matVP;
};

class GridRenderer
{
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11Buffer* pConstantBuffer;
	ID3D11SamplerState* pSamplerState;
	ID3D11DepthStencilState* pDSState;
	ShaderMan11::SMID shaderId;
	int lines;
public:
	GridRenderer();
	~GridRenderer();
	void Destroy();
	void Init();
	void Draw();
};

extern GridRenderer gridRenderer;

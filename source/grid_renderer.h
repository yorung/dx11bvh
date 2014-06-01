struct SolidConstantBuffer
{
	Mat matW;
	Mat matVP;
};

class GridRenderer
{
	static BufferMan::BMID constantBufferId;
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
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

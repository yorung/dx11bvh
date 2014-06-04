class WaterSurface
{
	static BufferMan::BMID constantBufferId;
	TexMan::TMID texId;
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11SamplerState* pSamplerState;
	ID3D11DepthStencilState* pDSState;
	ShaderMan11::SMID shaderId;
	int lines;
public:
	WaterSurface();
	~WaterSurface();
	void Destroy();
	void Init();
	void Draw();
};

extern WaterSurface waterSurface;

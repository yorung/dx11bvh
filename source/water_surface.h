struct WaterVert {
	Vec3 pos;
	Vec3 normal;
	DWORD color;
};

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
	void UpdateVert(std::vector<WaterVert>& vert);
	void Update();
public:
	WaterSurface();
	~WaterSurface();
	void Destroy();
	void Init();
	void Draw();
};

extern WaterSurface waterSurface;

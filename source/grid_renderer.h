struct SolidConstantBuffer
{
	Mat matW;
	Mat matVP;
};

class GridRenderer
{
	static BufferMan::BMID constantBufferId;
	VBOID vbo;
	IBOID ibo;
	VAOID vao;
	ID3D11SamplerState* pSamplerState;
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

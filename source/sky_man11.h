class SkyMan
{
	TexMan::TMID texId;
	ShaderMan::SMID shaderId;
	ID3D11SamplerState* sampler;
	ID3D11DepthStencilState* depthStencilState;
	static BufferMan::BMID constantBufferId;
public:
	SkyMan();
	~SkyMan();
	void Create(const char*);
	void Draw();
	void Destroy();
};

extern SkyMan skyMan;

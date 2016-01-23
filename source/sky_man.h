class SkyMan
{
	TexMan::TMID texId;
	ShaderMan::SMID shaderId;
	ID3D11SamplerState* sampler = nullptr;
	ID3D11DepthStencilState* depthStencilState = nullptr;
	ID3D11BlendState* blendState = nullptr;
	static BufferMan::BMID constantBufferId;
public:
	SkyMan();
	~SkyMan();
	void Create(const char *strCubeMapFile, const char *shader);
	void Draw();
	void Destroy();
};

extern SkyMan skyMan;

class SkyMan
{
	TexMan::TMID texId = TexMan::INVALID_TMID;
	ShaderMan::SMID shaderId = ShaderMan::INVALID_SMID;
	UBOID uboId;
	ID3D11SamplerState* sampler = nullptr;
	ID3D11DepthStencilState* depthStencilState = nullptr;
	ID3D11BlendState* blendState = nullptr;
public:
	~SkyMan();
	void Create(const char *texFileName, const char *shader);
	void Draw();
	void Destroy();
};

extern SkyMan skyMan;

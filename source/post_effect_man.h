class PostEffectMan
{
	ShaderMan::SMID shaderId;
	ID3D11SamplerState* sampler;
	ID3D11DepthStencilState* depthStencilState;
	static BufferMan::BMID constantBufferId;
public:
	PostEffectMan();
	~PostEffectMan();
	void Create(const char *shader);
	void Draw(ID3D11ShaderResourceView* shaderResourceView);
	void Destroy();
};

extern PostEffectMan postEffectMan;

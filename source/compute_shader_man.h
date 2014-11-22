class ComputeShaderMan
{
	ID3D11ComputeShader* computeShader;
	static BufferMan::BMID constantBufferId;
public:
	ComputeShaderMan();
	~ComputeShaderMan();
	void Create(const char *shader);
	void Draw(ID3D11ShaderResourceView* shaderResourceView, ID3D11UnorderedAccessView* unorderedAccessView);
	void Destroy();
};

extern ComputeShaderMan computeShaderMan;

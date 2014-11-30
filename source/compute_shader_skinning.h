class ComputeShaderSkinning
{
	ID3D11ComputeShader* computeShader;
	static BufferMan::BMID constantBufferId;
public:
	ComputeShaderSkinning();
	~ComputeShaderSkinning();
	void Create(const char *shader);
	void Dispatch(Mat bone[BONE_MAX], ID3D11ShaderResourceView* shaderResourceView, ID3D11UnorderedAccessView* unorderedAccessView);
	void Destroy();
};

extern ComputeShaderSkinning computeShaderSkinning;

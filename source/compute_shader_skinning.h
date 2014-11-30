class ComputeShaderSkinning
{
	ID3D11ComputeShader* computeShader;
	static BufferMan::BMID constantBufferId;
public:
	ComputeShaderSkinning();
	~ComputeShaderSkinning();
	void Create(const char *shader);
	void Dispatch(const Mat bone[BONE_MAX], ID3D11ShaderResourceView* srvPos, ID3D11ShaderResourceView* srvSkin, ID3D11UnorderedAccessView* unorderedAccessView, int nVert);
	void Destroy();
};

extern ComputeShaderSkinning computeShaderSkinning;

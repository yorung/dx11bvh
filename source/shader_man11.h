class ShaderMan11
{
public:
	typedef unsigned int SMID;
	static const SMID INVALID_SMID = 0;
private:
	struct Effect
	{
		ID3D11InputLayout* pInputLayout;
		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3DBlob* pBlobVS;
		const D3D11_INPUT_ELEMENT_DESC *elements;
		int numElements;
		Effect() { memset(this, 0, sizeof(*this)); }
	};
	std::map<std::string, SMID> m_nameToId;
	std::vector<Effect> m_effects;
public:
	ShaderMan11();
	~ShaderMan11();
	SMID Create(const char *name, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements);
	void Destroy();
	void Reload();
	void Apply(SMID id);
	ID3DBlob* GetVSBlob(SMID id);
};

class FakeVAO
{
	ID3D11InputLayout* inputLayout;
	std::vector<ID3D11Buffer*> vbos;
	std::vector<UINT> offsets;
	std::vector<UINT> strides;
	ID3D11Buffer* ibo;
public:
	FakeVAO(ShaderMan11::SMID shaderId, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements, int numBuffers, ID3D11Buffer* buffers[], const UINT strides[], const UINT offsets[], ID3D11Buffer* ibo);
	~FakeVAO();
	void Apply();
};

extern ShaderMan11 shaderMan;
typedef ShaderMan11 ShaderMan;


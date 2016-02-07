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
		BlendMode blendMode;
		bool useDepthBuffer;
		Effect() { memset(this, 0, sizeof(*this)); }
	};
	std::map<std::string, SMID> m_nameToId;
	std::vector<Effect> m_effects;
public:
	ShaderMan11();
	~ShaderMan11();
	SMID Create(const char *name, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements, BlendMode blendMode, bool useDepthBuffer);
	void Destroy();
	void Reload();
	void Apply(SMID id);
	ID3DBlob* GetVSBlob(SMID id);
};

class FakeVAO
{
	std::vector<VBOID> vbos;
	std::vector<ID3D11Buffer*> d3dBuffers;
	std::vector<UINT> offsets;
	std::vector<UINT> strides;
	ComPtr<ID3D11Buffer> ibo;
public:
	FakeVAO(int numBuffers, VBOID* const buffers, const int strides[], const UINT offsets[], IBOID ibo);
	void Apply();
};
typedef FakeVAO* VAOID;

extern ShaderMan11 shaderMan;
typedef ShaderMan11 ShaderMan;

VAOID afCreateVAO(const InputElement elements[], int numElements, int numBuffers, VBOID* const vertexBufferIds, const int* strides, IBOID ibo);
inline void afBindVAO(VAOID vao)
{
	if (vao) {
		vao->Apply();
	}
}

#define SF_R32G32_FLOAT DXGI_FORMAT_R32G32_FLOAT

typedef D3D11_INPUT_ELEMENT_DESC InputElement;

class CInputElement : public InputElement {
public:
	CInputElement(int inputSlot, const char* name, DXGI_FORMAT format, int offset) {
		SemanticName = name;
		SemanticIndex = 0;
		Format = format;
		InputSlot = inputSlot;
		AlignedByteOffset = offset;
		InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		InstanceDataStepRate = 0;
	}
};

class ShaderMan11
{
public:
	typedef int SMID;
	static const SMID INVALID_SMID = -1;
private:
	struct Effect
	{
		ID3D11InputLayout* pInputLayout;
		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		const D3D11_INPUT_ELEMENT_DESC *elements;
		int numElements;
	};
	std::map<std::string, SMID> m_nameToId;
	std::vector<Effect> m_effects;
public:
	SMID Create(const char *name, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements);
	void Destroy();
	void Reload();
	void Apply(SMID id);
};

extern ShaderMan11 shaderMan;
typedef ShaderMan11 ShaderMan;


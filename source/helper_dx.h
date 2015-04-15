#define SF_R32G32_FLOAT DXGI_FORMAT_R32G32_FLOAT
#define SF_R32G32B32_FLOAT DXGI_FORMAT_R32G32B32_FLOAT
#define SF_R8G8B8A8_UNORM DXGI_FORMAT_R8G8B8A8_UNORM
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


typedef unsigned short AFIndex;

#define AFIndexTypeToDevice DXGI_FORMAT_R16_UINT
typedef ID3D11Buffer* IBOID;
typedef ID3D11Buffer* VBOID;
typedef ID3D11Buffer* UBOID;

#define afSafeDeleteBuffer SAFE_RELEASE
#define afSafeDeleteSampler SAFE_RELEASE

void afWriteBuffer(ID3D11Buffer* p, const void* buf, int size);

IBOID afCreateIndexBuffer(const AFIndex* indi, int numIndi);
IBOID afCreateQuadListIndexBuffer(int numQuads);
VBOID afCreateVertexBuffer(int size, const void* buf);
VBOID afCreateDynamicVertexBuffer(int size);
UBOID afCreateUBO(int size);
void afBindBufferToBindingPoint(UBOID ubo, UINT uniformBlockBinding);
void afBindTextureToBindingPoint(TexMan::TMID tex, UINT textureBindingPoint);
void afBindSamplerToBindingPoint(ID3D11SamplerState*, UINT textureBindingPoint);

void afDrawIndexedTriangleList(IBOID ibo, int count, int start = 0);
void afEnableBackFaceCulling(bool cullBack);

enum BlendMode {
	BM_NONE,
	BM_ALPHA,
};
void afBlendMode(BlendMode mode);
void afDepthStencilMode(bool depth);

/*
struct VAOID {
	ShaderMan::SMID shaderId;
	IBOID ibo;
	std::vector<VBOID> vertexBufferIds;
	std::vector<size_t> strides;
};
GLuint afCreateVAO(GLuint program, const InputElement elements[], int numElements, int numBuffers, VBOID const *vertexBufferIds, const GLsizei* strides, IBOID ibo);
*/

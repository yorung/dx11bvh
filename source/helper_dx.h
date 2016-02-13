#define SF_R32G32_FLOAT DXGI_FORMAT_R32G32_FLOAT
#define SF_R32G32B32_FLOAT DXGI_FORMAT_R32G32B32_FLOAT
#define SF_R8G8B8A8_UNORM DXGI_FORMAT_R8G8B8A8_UNORM
typedef D3D11_INPUT_ELEMENT_DESC InputElement;

class CInputElement : public InputElement {
public:
	CInputElement(const char* name, DXGI_FORMAT format, int offset, int inputSlot = 0) {
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
typedef ComPtr<ID3D11Buffer> IBOID;
typedef ComPtr<ID3D11Buffer> VBOID;
typedef ComPtr<ID3D11Buffer> UBOID;
typedef ComPtr<ID3D11SamplerState> SAMPLERID;
typedef ComPtr<ID3D11ShaderResourceView> SRVID;
inline void afSafeDeleteBuffer(ComPtr<ID3D11Buffer>& p) { p.Reset(); }
inline void afSafeDeleteSampler(SAMPLERID& p) { p.Reset(); }
inline void afSafeDeleteTexture(SRVID& p) { p.Reset(); }
#define afSafeDeleteVAO SAFE_DELETE

void afWriteBuffer(const IBOID p, const void* buf, int size);
void afWriteTexture(SRVID srv, const struct TexDesc& desc, const void* buf);

IBOID afCreateIndexBuffer(const AFIndex* indi, int numIndi);
IBOID afCreateQuadListIndexBuffer(int numQuads);
VBOID afCreateVertexBuffer(int size, const void* buf);
VBOID afCreateDynamicVertexBuffer(int size);
UBOID afCreateUBO(int size);

#define SamplerWrap D3D11_TEXTURE_ADDRESS_MODE
#define SW_REPEAT D3D11_TEXTURE_ADDRESS_WRAP
#define SW_CLAMP D3D11_TEXTURE_ADDRESS_CLAMP

#define SamplerFilter D3D11_FILTER
#define SF_POINT D3D11_FILTER_MIN_MAG_MIP_POINT
#define SF_LINEAR D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT
#define SF_MIPMAP D3D11_FILTER_MIN_MAG_MIP_LINEAR

SAMPLERID afCreateSampler(SamplerFilter samplerFilter, SamplerWrap wrap);

void afBindBufferToBindingPoint(UBOID ubo, UINT uniformBlockBinding);
void afBindTextureToBindingPoint(SRVID srv, UINT textureBindingPoint);
#define afBindCubeMapToBindingPoint afBindTextureToBindingPoint
void afBindSamplerToBindingPoint(SAMPLERID sampler, UINT textureBindingPoint);

void afDrawIndexedTriangleStrip(int numIndices, int start = 0);
void afDrawIndexedTriangleList(int numIndices, int start = 0);
void afDrawTriangleStrip(int numVertices, int start = 0);
void afDrawLineList(int numVertices, int start = 0);

enum CullMode {
	CM_DISABLE,
	CM_CW,
	CM_CCW,
};
void afCullMode(CullMode cullMode);
enum BlendMode {
	BM_NONE,
	BM_ALPHA,
};
void afBlendMode(BlendMode mode);
enum DepthStencilMode {
	DSM_DISABLE,
	DSM_DEPTH_ENABLE,
	DSM_DEPTH_CLOSEREQUAL_READONLY,
};
void afDepthStencilMode(DepthStencilMode mode);

typedef D3D11_SUBRESOURCE_DATA AFTexSubresourceData;
typedef DXGI_FORMAT AFDTFormat;
#define AFDT_INVALID DXGI_FORMAT_UNKNOWN
#define AFDT_R8G8B8A8_UNORM DXGI_FORMAT_R8G8B8A8_UNORM
#define AFDT_R5G6B5_UINT DXGI_FORMAT_B5G6R5_UNORM
#define AFDT_R32G32B32A32_FLOAT DXGI_FORMAT_R32G32B32A32_FLOAT
#define AFDT_R16G16B16A16_FLOAT DXGI_FORMAT_R16G16B16A16_FLOAT
#define AFDT_DEPTH DXGI_FORMAT_D24_UNORM_S8_UINT
#define AFDT_DEPTH_STENCIL DXGI_FORMAT_D24_UNORM_S8_UINT
#define AFDT_BC1_UNORM DXGI_FORMAT_BC1_UNORM
#define AFDT_BC2_UNORM DXGI_FORMAT_BC2_UNORM
#define AFDT_BC3_UNORM DXGI_FORMAT_BC3_UNORM

SRVID afCreateTexture2D(AFDTFormat format, const IVec2& size, void *image);
SRVID afCreateTexture2D(AFDTFormat format, const IVec2& size, int arraySize, int mipCount, const AFTexSubresourceData datas[]);
SRVID afCreateDynamicTexture(AFDTFormat format, const IVec2& size);

class AFRenderTarget
{
	IVec2 texSize;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	ID3D11UnorderedAccessView* unorderedAccessView = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
public:
	~AFRenderTarget() { Destroy(); }
	void InitForDefaultRenderTarget();
	void Init(IVec2 size, DXGI_FORMAT colorFormat);
	void Destroy();
	void BeginRenderToThis();
	ID3D11ShaderResourceView* GetTexture() { return shaderResourceView; }
	ID3D11UnorderedAccessView* GetUnorderedAccessView() { return unorderedAccessView; }
};

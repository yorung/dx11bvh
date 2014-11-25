struct Block;
struct MeshVertex;
struct MeshSkin;

class MeshRenderer11
{
	static BufferMan::BMID constantBufferId;
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* skinBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11SamplerState* pSamplerState;
	ID3D11DepthStencilState* pDSState;
	ShaderMan11::SMID shaderId;
public:
	MeshRenderer11();
	~MeshRenderer11();
	void Destroy();
	void Init(int numVertices, const MeshVertex* vertices, const MeshSkin* skin, int numIndices, const unsigned* indices);
	void Init(const Block& block);
	void Draw(const Mat BoneMatrices[BONE_MAX], int nBones, const Block& block) const;
};

typedef MeshRenderer11 MeshRenderer;

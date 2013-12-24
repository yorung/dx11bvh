struct Block;

class MeshRenderer11
{
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;
	ID3D11Buffer* pConstantBuffer;
	ID3D11SamplerState* pSamplerState;
	ID3D11DepthStencilState* pDSState;
	ShaderMan11::SMID shaderId;
public:
	MeshRenderer11();
	~MeshRenderer11();
	void Destroy();
	void Init(int sizeVertices, int sizeIndices, void* vertices, void* indices);
	void Draw(XMMATRIX BoneMatrices[], int nBones, const Block& block);
};

typedef MeshRenderer11 MeshRenderer;

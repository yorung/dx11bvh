struct Block;
struct MeshVertex;
struct MeshColor;
struct MeshSkin;

class MeshRenderer11
{
	UBOID uboId;
	ID3D11Buffer* posBuffer;
	VBOID colorBuffer;
	ID3D11Buffer* skinBuffer;
	ID3D11Buffer* skinnedPosBuffer;
	IBOID iboId;
	SAMPLERID sampler;
	ShaderMan11::SMID shaderId;
	VAOID vao;
public:
	MeshRenderer11();
	~MeshRenderer11();
	void Destroy();
	void Init(int numVertices, const MeshVertex* vertices, const MeshColor* color, const MeshSkin* skin, int numIndices, const AFIndex* indices);
	void Init(const Block& block);
	void Calc(const Mat BoneMatrices[BONE_MAX], const Block& block) const;
	void Draw(const Mat BoneMatrices[BONE_MAX], int nBones, const Block& block) const;
};

typedef MeshRenderer11 MeshRenderer;

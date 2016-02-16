struct Block;
struct MeshVertex;
struct MeshColor;
struct MeshSkin;

typedef int MMID;

class Material
{
public:
	Material() {}
	Material(const Material& r) { *this = r; }
	const Material& operator=(const Material& r);
	bool operator==(const Material& r) const;
	Vec4 faceColor;
	Vec3 specular;
	float power = 0;
	Vec3 emissive;
	SRVID texture;
};

class MatMan
{
	std::vector<Material> mats;
public:
	~MatMan();
	MMID Create(const Material& mat);
	void Destroy();
	const Material* Get(MMID id);
};

extern MatMan matMan;

class MeshRenderer
{
	UBOID uboId;
	ID3D11Buffer* posBuffer;
	VBOID colorBuffer;
	ID3D11Buffer* skinBuffer;
	ID3D11Buffer* skinnedPosBuffer;
	IBOID iboId;
	SAMPLERID sampler;
	ShaderMan::SMID shaderId;
	VAOID vao;
public:
	MeshRenderer();
	~MeshRenderer();
	void Destroy();
	void Init(int numVertices, const MeshVertex* vertices, const MeshColor* color, const MeshSkin* skin, int numIndices, const AFIndex* indices);
	void Init(const Block& block);
	void Calc(const Mat BoneMatrices[BONE_MAX], const Block& block) const;
	void Draw(const Mat BoneMatrices[BONE_MAX], int nBones, const Block& block) const;
};

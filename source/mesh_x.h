class Mesh
{
public:
	virtual ~Mesh() {}
	virtual void Draw(int animId, double time) = 0;
	virtual const struct Block& GetRawDatas() const = 0;
};

struct TimedFloatKeys
{
	DWORD time;
	XMFLOAT4X4 mat;
};

typedef int BONE_ID;

struct AnimationKey
{
	int keyType;
	std::vector<TimedFloatKeys> timedFloatKeys;

	const AnimationKey& operator=(const AnimationKey& r)
	{
		keyType = r.keyType;
		timedFloatKeys = r.timedFloatKeys;
		return *this;
	}
	AnimationKey() {}
	AnimationKey(const AnimationKey& r)
	{
		*this = r;
	}
};

struct Animation
{
	std::vector<AnimationKey> animationKeys;
	std::string frameName;

	const Animation& operator=(const Animation& r)
	{
		animationKeys = r.animationKeys;
		frameName = r.frameName;
		return *this;
	}
	Animation() {}
	Animation(const Animation& r)
	{
		*this = r;
	}
};

struct AnimationSet
{
	std::map<BONE_ID, Animation> animations;

	const AnimationSet& operator=(const AnimationSet& r)
	{
		animations = r.animations;
		return *this;
	}
	AnimationSet() {}
	AnimationSet(const AnimationSet& r)
	{
		*this = r;
	}
};

struct ubyte4
{
	unsigned char x, y, z, w;
};

struct MeshVertex
{
	XMFLOAT3 xyz;
	XMFLOAT3 normal;
	DWORD color;
	XMFLOAT2 uv;
	XMFLOAT3 blendWeights;
	ubyte4 blendIndices;
};

struct MaterialMap
{
	MatMan::MMID materialId;
	int faceStartIndex;
	int faces;
};

struct Frame
{
	char name[32];
	XMFLOAT4X4 frameTransformMatrix;
	XMFLOAT4X4 boneOffsetMatrix;
	XMFLOAT4X4 result;
	BONE_ID parentId;
	BONE_ID childId;
	BONE_ID siblingId;
};

struct Block
{
	std::vector<MeshVertex> vertices;
	std::vector<unsigned> indices;
	std::vector<MaterialMap> materialMaps;
};

class MeshX : public Mesh
{
private:
	MeshRenderer m_meshRenderer;

private:
	bool ParseMesh(char* imgFrame, Block& block, BONE_ID frameId);
	void ParseFrame(char* p, BONE_ID parentFrameId);
	void ParseAnimationSets(char* p);
	void ParseAnimations(char* p, AnimationSet& animationSet);
	void LoadSub(const char* fileName);
	void _pushMaterialMap(Block& block, const MaterialMap& map);
	void _mergeBlocks(Block& d, const Block& s);
	BONE_ID _getFrameIdByName(const char* name);
	void _linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId);
	void _storeWeight(MeshVertex& v, int frameId, float weight);
	void CalcAnimation(int animId, double time);
	void CalcFrameMatrices(BONE_ID frameId, XMMATRIX& parent);
	void DumpFrames(BONE_ID frameId, int depth) const;

	std::vector<Frame> m_frames;
	std::vector<AnimationSet> m_animationSets;
	Block m_block;
	int m_animTicksPerSecond;
public:
	const Block& GetRawDatas() const { return m_block; }
	MeshX(const char *fileName);
	~MeshX();
	void Draw(int animId, double time);
	void DrawBvh(class Bvh* bvh, double time);
};


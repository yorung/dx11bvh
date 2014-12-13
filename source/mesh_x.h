class Bvh;

class Mesh
{
public:
	virtual ~Mesh() {}
	virtual const struct Block& GetRawDatas() const = 0;
};

struct TimedFloatKeys
{
	DWORD time;
	Mat mat;
};

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
	Vec3 xyz;
	Vec3 normal;
};

struct MeshColor
{
	DWORD color;
	Vec2 uv;
};

struct MeshSkin
{
	Vec3 blendWeights;
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
	Mat initialMatrix;
	Mat boneOffsetMatrix;
	BONE_ID parentId;
	BONE_ID childId;
	BONE_ID siblingId;
};

struct Block
{
	std::vector<MeshVertex> vertices;
	std::vector<MeshColor> color;
	std::vector<MeshSkin> skin;
	std::vector<unsigned> indices;
	std::vector<MaterialMap> materialMaps;
	void Clear()
	{
		vertices.clear();
		color.clear();
		skin.clear();
		indices.clear();
		materialMaps.clear();
	}
	void Verify() const
	{
		assert(vertices.size() == skin.size());
		assert(vertices.size() == color.size());
	}
};

struct MeshXAnimResult
{
	Mat boneMat[BONE_MAX];
};

struct MeshXBvhBinding
{
	Quat axisAlignQuats[BONE_MAX];
	Quat boneAlignQuats[BONE_MAX];
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
	BONE_ID GetOrCreateFrameIdByName(const char* name);
	BONE_ID GetFrameIdByName(const char* name) const;
	void _linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId);
	void _storeWeight(MeshSkin& v, int frameId, float weight);
	void CalcFrameMatrices(MeshXAnimResult& animResult, const Mat localMats[BONE_MAX]) const;
	void DumpFrames() const;
	void CreateBoneMesh();
	int GetDepth(BONE_ID id) const;
	void PrintStatistics() const;
	void GetVertStatistics(std::vector<int>& cnts) const;
	void GetAnimStatistics(std::vector<int>& animCnts) const;
	void DeleteDummyFrames();
	bool UnlinkFrame(BONE_ID id);
	void ApplyBvhInitialStance(const Bvh* bvh, MeshXBvhBinding& bind) const;

	std::vector<Frame> m_frames;
	std::vector<AnimationSet> m_animationSets;
	Block m_block;
	int m_animTicksPerSecond;

	MeshRenderer bonesRenderer;
	Block bones;
public:
	const Block& GetRawDatas() const { return m_block; }
	const std::vector<Frame>& GetFrames() const { return m_frames; }
	MeshX(const char *fileName);
	~MeshX();
	void CalcAnimation(int animId, double time, MeshXAnimResult& result) const;
	void CalcAnimationFromBvh(class Bvh* bvh, const MeshXBvhBinding& bind, double time, MeshXAnimResult& animResult, float translationScale) const;
	void Draw(const MeshXAnimResult& animResult) const;
	void SyncLocalAxisWithBvh(Bvh* bvh, MeshXBvhBinding& bind) const;
};


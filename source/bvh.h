struct Pose
{
	std::vector<Quat> quats;
};

struct Motion
{
	std::vector<Pose> poses;
};

struct BvhFrame
{
	char name[32];
	Vec3 offset;
	Vec3 offsetCombined;
	BONE_ID parentId;
	BONE_ID childId;
	BONE_ID siblingId;
	XMINT3 rotIndices;
	XMINT3 posIndices;
	Matrix frameTransformMatrix;
	Matrix boneOffsetMatrix;
	Matrix result;
};

class Bvh : public Mesh
{
private:
	void CalcBoneOffsetMatrix(BONE_ID frameId);
	void ParseFrame(const char* frameStr, char* p, BONE_ID parentFrameId);
	void LoadSub(const char* fileName);
	BONE_ID _getFrameIdByName(const char* name);
	void _linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId);
	void _storeWeight(MeshVertex& v, int frameId, float weight);
	void CalcFrameMatrices();
	void DumpFrames(BONE_ID frameId, int depth) const;
	void ParseMotion(const char *p);
	void PreCalculateMotion();
	int GetDepth(BONE_ID id);
	void CreateBoneMesh();
	void CalcCombinedOffsets();
	BONE_ID BoneNameToId(const char* name) const;
	void CreateBoneTypeToIdTbl();


	BONE_ID boneTypeToIdTbl[BT_MAX];
	std::vector<BvhFrame> m_frames;
	std::vector<float> rawMotion;
	Motion motion;

	int motionFrames;
	float frameTime;
	int channels;

	MeshRenderer m_meshRenderer;
	Block m_block;
public:
	const Block& GetRawDatas() const { return m_block; }
	const std::vector<BvhFrame>& GetFrames() const { return m_frames; }
	Bvh(const char *fileName);
	~Bvh();
	void Draw(int animId, double time);
	void ResetAnim();
	void GetRotAnim(Quat quats[BONE_MAX], double time) const;
	BONE_ID BoneTypeToId(BoneType type) const;
	void FixBones(const char* name);
	void LinkTo(const char* me, const char* linkTo);
	bool UnlinkFromParent(BONE_ID id);
	void CalcAnimation(double time);
};


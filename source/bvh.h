struct Pose
{
	std::vector<SimpleMath::Quaternion> quats;
};

struct Motion
{
	std::vector<Pose> poses;
};

struct BvhFrame
{
	char name[32];
	Vector3 offset;
	Matrix offsetCombined;
	BONE_ID parentId;
	BONE_ID childId;
	BONE_ID siblingId;
	XMINT3 rotIndies;
	XMINT3 posIndies;
	Quaternion axisAlignQuat;
	Matrix frameTransformMatrix;
	Matrix boneOffsetMatrix;
	Matrix result;
};

class Bvh : public Mesh
{
private:
	bool ParseMesh(char* imgFrame, Block& block, BONE_ID frameId);
	void CalcBoneOffsetMatrix(BONE_ID frameId, const Quaternion& axisAlignQuat);
	void ParseFrame(const char* frameStr, char* p, BONE_ID parentFrameId);
	void LoadSub(const char* fileName);
	BONE_ID _getFrameIdByName(const char* name);
	void _linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId);
	void _storeWeight(MeshVertex& v, int frameId, float weight);
	void CalcAnimation(int animId, double time);
	void CalcFrameMatrices(BONE_ID frameId, XMMATRIX& parent);
	void DumpFrames(BONE_ID frameId, int depth) const;
	void ParseMotion(const char *p);
	void PreCalculateMotion();
	void CalcAnimation(double time);
	int GetDepth(BONE_ID id);
	void CreateBoneMesh();

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
	void CalcRotAnimForAlignedAxis(XMMATRIX RotAnimMatrices[50], double time) const;
	BONE_ID BoneNameToId(const char* name);
	void SetLocalAxis(BONE_ID boneId, const Quaternion& q);
};


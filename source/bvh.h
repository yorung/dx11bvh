struct BvhFrame
{
	char name[32];
	XMFLOAT3 offset;
	XMFLOAT3 offsetCombined;
	BONE_ID parentId;
	BONE_ID childId;
	BONE_ID siblingId;
	XMINT3 rotIndices;
	XMINT3 posIndices;
	XMFLOAT4X4 frameTransformMatrix;
	XMFLOAT4X4 boneOffsetMatrix;
	XMFLOAT4X4 result;
};

class Bvh : public Mesh
{
private:
	bool ParseMesh(char* imgFrame, Block& block, BONE_ID frameId);
	void ParseFrame(const char* frameStr, char* p, BONE_ID parentFrameId);
	void LoadSub(const char* fileName);
	BONE_ID _getFrameIdByName(const char* name);
	void _linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId);
	void _storeWeight(MeshVertex& v, int frameId, float weight);
	void CalcAnimation(int animId, double time);
	void CalcFrameMatrices(BONE_ID frameId, XMMATRIX& parent);
	void DumpFrames(BONE_ID frameId, int depth) const;
	void ParseMotion(const char *p);
	void CalcAnimation(double time);
	int GetDepth(BONE_ID id);
	void CreateBoneMesh();

	std::vector<BvhFrame> m_frames;
	std::vector<float> motion;
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
};


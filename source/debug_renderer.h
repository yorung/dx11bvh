class DebugRenderer
{
	MeshRenderer pivotsRenderer;
	Block pivots;
	void CreatePivotMesh();
public:
	DebugRenderer();
	~DebugRenderer();
	void Init();
	void DrawPivots(const XMMATRIX mat[BONE_MAX], int num);
	void Destroy();
};

extern DebugRenderer debugRenderer;

void CreateCone(Block& b, const Vec3& v1, const Vec3& v2, BONE_ID boneId, DWORD color);

static const int BONE_MAX = 50;

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

void CreateCone(Block& b, XMVECTOR v1, XMVECTOR v2, BONE_ID boneId, DWORD color);

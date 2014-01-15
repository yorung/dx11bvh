static const int BONE_MAX = 50;

class DebugRenderer
{
public:
	DebugRenderer();
	~DebugRenderer();
	void Init();
	void DrawPivots(const XMMATRIX mat[BONE_MAX]);
	void Destroy();
};

extern DebugRenderer debugRenderer;

class App {
	MeshX* meshTiny;
	MeshXBvhBinding bind[3];
	Mesh* mesh[3];
	float radius;
	AFRenderTarget rt[3];
	int animationNumber;
	double trackTime;
	double lastTime;
	GridRenderer gridRenderer;
	int skyNum = 0;
	PostEffectMan postEffectCopy, postEffectMono, postEffectUAVTest;
public:
	App();
	~App();
	void ApplySky();
	void Init(const char* fileName);
	void Update();
	void Draw();
	void DrawBoneNames(Bvh* bvh);
	void DrawBoneNames(const MeshX* meshX, const MeshXAnimResult& result);
	void DrawCameraParams();
	void Destroy();
};

extern std::string g_type;
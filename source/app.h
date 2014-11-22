class App {
	MeshX* meshTiny;
	MeshXBvhBinding bind[3];
	Mesh* mesh[3];
	float scale;
	float radius;
	float lastX;
	float lastY;
	float rotX;
	float rotY;
	float height;
	SpriteBatch* sprite;
	SpriteFont* font;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11UnorderedAccessView* unorderedAccessView;
	int animationNumber;
	double trackTime;
	double lastTime;
	GridRenderer gridRenderer;
public:
	App();
	~App();
	void Init(const char* fileName);
	void Update();
	void Draw();
	void DrawBoneNames(Bvh* bvh);
	void DrawBoneNames(const MeshX* meshX, const MeshXAnimResult& result);
	void DrawCameraParams();
	void MouseWheel(float delta);
	void LButtonDown(float x, float y);
	void LButtonUp(float x, float y);
	void MouseMove(float x, float y);
	void Destroy();
};

extern std::string g_type;
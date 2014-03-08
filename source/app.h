class App {
	Mesh* mesh[3];
	float scale;
	float lastX;
	float lastY;
	float rotX;
	float rotY;
	float height;
	SpriteBatch* sprite;
	SpriteFont* font;
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
	void MouseWheel(float delta);
	void LButtonDown(float x, float y);
	void LButtonUp(float x, float y);
	void MouseMove(float x, float y);
	void Destroy();
};

extern std::string g_type;
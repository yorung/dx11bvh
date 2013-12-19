class App {
	Mesh* mesh;
	float scale;
	float lastX;
	float lastY;
	XMVECTOR quat;
public:
	App();
	~App();
	void Init(const char* fileName);
	void Draw();
	void MouseWheel(float delta);
	void LButtonDown(float x, float y);
	void LButtonUp(float x, float y);
	void MouseMove(float x, float y);
	void Destroy();
};
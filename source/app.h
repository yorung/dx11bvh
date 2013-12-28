class App {
	Mesh* mesh[3];
	float scale;
	float lastX;
	float lastY;
	XMVECTOR quat;
	SpriteBatch* sprite;
	SpriteFont* font;
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
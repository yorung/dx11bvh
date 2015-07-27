class App {
	MeshX* meshTiny;
	MeshXBvhBinding bind[3];
	Mesh* mesh[3];
	float radius;
	ID3D11RenderTargetView* renderTargetView2;
	ID3D11ShaderResourceView* shaderResourceView2;
	ID3D11UnorderedAccessView* unorderedAccessView2;
	AFRenderTarget rt[1];
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
	void Destroy();
};

extern std::string g_type;
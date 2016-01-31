struct WaterVert
{
	Vec3 pos;
	Vec3 normal;
};

struct WaterRipple
{
	double generatedTime;
	Vec2 centerPos;
};

class WaterSurface
{
	static BufferMan::BMID constantBufferId;
	SRVID srv;
	VBOID vbo;
	IBOID ibo;
	VAOID vao;
	ID3D11SamplerState* pSamplerState;
	ShaderMan11::SMID shaderId;
	int lines;
	void UpdateVert(std::vector<WaterVert>& vert);
	void Update();
	std::deque<WaterRipple> ripples;
public:
	WaterSurface();
	~WaterSurface();
	void Destroy();
	void Init();
	void Draw();
	void CreateRipple();
};

extern WaterSurface waterSurface;

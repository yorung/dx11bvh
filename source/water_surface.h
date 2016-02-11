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
	UBOID ubo;
	SRVID srv;
	VBOID vbo;
	IBOID ibo;
	VAOID vao = 0;
	SAMPLERID sampler = 0;
	ShaderMan::SMID shaderId;
	int lines;
	void UpdateVert(std::vector<WaterVert>& vert);
	void Update();
	std::deque<WaterRipple> ripples;
public:
	~WaterSurface();
	void Destroy();
	void Init();
	void Draw();
	void CreateRipple();
};

extern WaterSurface waterSurface;

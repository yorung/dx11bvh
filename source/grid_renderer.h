struct SolidConstantBuffer
{
	Mat matVP;
};

class GridRenderer
{
	UBOID ubo;
	VBOID vbo;
	IBOID ibo;
	VAOID vao;
	ShaderMan11::SMID shaderId;
	int lines;
public:
	GridRenderer();
	~GridRenderer();
	void Destroy();
	void Init();
	void Draw();
};

extern GridRenderer gridRenderer;

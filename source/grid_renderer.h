class GridRenderer
{
	UBOID ubo;
	VBOID vbo;
	IBOID ibo;
	VAOID vao = 0;
	ShaderMan::SMID shaderId;
	int lines;
public:
	~GridRenderer();
	void Destroy();
	void Init();
	void Draw();
};

extern GridRenderer gridRenderer;

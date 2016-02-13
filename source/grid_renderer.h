class GridRenderer
{
	UBOID ubo;
	VBOID vbo;
	IBOID ibo;
	VAOID vao;
	ShaderMan::SMID shaderId;
	int lines;
public:
	~GridRenderer();
	void Destroy();
	void Create(int numGrid, float pitch);
	void Draw();
};

extern GridRenderer gridRenderer;

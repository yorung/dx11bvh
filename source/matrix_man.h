class MatrixMan
{
public:
	enum Type{
		WORLD,
		VIEW,
		PROJ,

		MAX
	};
	XMFLOAT4X4 matrices[MAX];
public:
	void Get(Type type, XMMATRIX& m);
	void Set(Type type, const XMMATRIX& m);
};

extern MatrixMan matrixMan;

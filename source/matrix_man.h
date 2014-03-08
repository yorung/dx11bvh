class MatrixMan
{
public:
	enum Type{
		WORLD,
		VIEW,
		PROJ,

		MAX
	};
	Mat matrices[MAX];
public:
	void Get(Type type, Mat& m);
	void Set(Type type, const Mat& m);
	void Get(Type type, XMMATRIX& m) { Mat t; Get(type, t); m = t; }
};

extern MatrixMan matrixMan;

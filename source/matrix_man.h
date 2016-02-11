class MatrixMan
{
public:
	enum Type
	{
		VIEW,
		PROJ,

		MAX
	};
	Mat matrices[MAX];
public:
	void Get(Type type, Mat& m);
	void Set(Type type, const Mat& m);
};

extern MatrixMan matrixMan;

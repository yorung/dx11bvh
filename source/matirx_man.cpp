#include "stdafx.h"

MatrixMan matrixMan;

void MatrixMan::Get(Type type, Mat& m)
{
	m = matrices[type];
}

void MatrixMan::Set(Type type, const Mat& m)
{
	matrices[type] = m;
}

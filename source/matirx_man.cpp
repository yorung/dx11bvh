#include "stdafx.h"

MatrixMan matrixMan;

void MatrixMan::Get(Type type, XMMATRIX& m)
{
	m = XMLoadFloat4x4(&matrices[type]);
}

void MatrixMan::Set(Type type, const XMMATRIX& m)
{
	XMStoreFloat4x4(&matrices[type], m);
}

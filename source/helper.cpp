#include "stdafx.h"

#ifndef _MSC_VER
void *LoadFile(const char *fileName, int* size)
{
	jclass myview = jniEnv->FindClass(boundJavaClass);
	jmethodID method = jniEnv->GetStaticMethodID(myview, "loadIntoBytes", "(Ljava/lang/String;)[B");
	if (method == 0) {
		return nullptr;
	}

	jobject arrayAsJObject = jniEnv->CallStaticObjectMethod(myview, method, jniEnv->NewStringUTF(fileName));
	jbyteArray array = (jbyteArray)arrayAsJObject;

	jbyte* byteArray = jniEnv->GetByteArrayElements(array, NULL);
	jsize arrayLen = jniEnv->GetArrayLength(array);

	void* ptr = calloc(arrayLen + 1, 1);
	memcpy(ptr, byteArray, arrayLen);
	if (size) {
		*size = arrayLen;
	}

	jniEnv->ReleaseByteArrayElements(array, byteArray, 0);

	return ptr;
}
#endif

#ifndef _MSC_VER
double GetTime()
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (double)t.tv_nsec / 1000000000;
}
#endif

float Random()
{
	return (float)rand() / RAND_MAX;
}

#ifdef GL_TRUE
GLuint afCreateIndexBuffer(const AFIndex* indi, int numIndi)
{
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndi * sizeof(AFIndex), &indi[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}

AFBufObj afCreateVertexBuffer(int size, const void* buf)
{
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}

AFBufObj afCreateDynamicVertexBuffer(int size)
{
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}

void afWriteBuffer(GLuint bufName, const void* buf, int size)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufName);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buf);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#else
ID3D11Buffer* afCreateIndexBuffer(const AFIndex* indi, int numIndi)
{
	ID3D11Buffer* indexBuffer;
	D3D11_SUBRESOURCE_DATA subresData = { indi, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numIndi * sizeof(AFIndex), D3D11_BIND_INDEX_BUFFER), &subresData, &indexBuffer);
	return indexBuffer;
}

ID3D11Buffer* afCreateDynamicVertexBuffer(int size)
{
	ID3D11Buffer* vbo;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), nullptr, &vbo);
	return vbo;
}

void afWriteBuffer(ID3D11Buffer* p, const void* buf, int size)
{
#ifdef _DEBUG
	D3D11_BUFFER_DESC desc;
	p->GetDesc(&desc);
	if (size > (int)desc.ByteWidth) {
		return;
	}
#endif
	D3D11_MAPPED_SUBRESOURCE m;
	HRESULT hr = deviceMan11.GetContext()->Map(p, 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, buf, size);
	deviceMan11.GetContext()->Unmap(p, 0);
}

#endif

AFBufObj afCreateQuadListIndexBuffer(int numQuads)
{
	std::vector<AFIndex> indi;
	int numIndi = numQuads * 6;
	indi.resize(numIndi);
	for (int i = 0; i < numIndi; i++)
	{
		static int tbl[] = { 0, 1, 2, 1, 3, 2 };
		int rectIdx = i / 6;
		int vertIdx = i % 6;
		indi[i] = rectIdx * 4 + tbl[vertIdx];
	}
	return afCreateIndexBuffer(&indi[0], numIndi);
}

#ifdef GL_TRUE
void afDrawIndexedTriangleList(AFBufObj ibo, int count, int start)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, count, AFIndexTypeToDevice, (void*)(start));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#else
void afDrawIndexedTriangleList(AFBufObj ibo, int count, int start)
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceMan11.GetContext()->IASetIndexBuffer(ibo, AFIndexTypeToDevice, 0);
	deviceMan11.GetContext()->DrawIndexed(count, start, 0);
}
#endif

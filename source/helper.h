#define dimof(x) (sizeof(x) / sizeof(x[0]))

void *LoadFile(const char *fileName, int* size = nullptr);
bool SaveFile(const char *fileName, const uint8_t* buf, int size);
void GoMyDir();
double GetTime();
float Random();

template <class T> inline void SAFE_DELETE(T& p)
{
	delete p;
	p = nullptr;
}

template <class T> inline void SAFE_RELEASE(T& p)
{
	if (p) {
		p->Release();
		p = nullptr;
	}
}

#ifndef DIRECTX_MATH_VERSION
#define XM_PI 3.14159265359f
#define XM_2PI (XM_PI * 2)
#endif

typedef unsigned short AFIndex;
#ifdef GL_TRUE
#define AFIndexTypeToDevice GL_UNSIGNED_SHORT
typedef GLuint AFBufObj;
#else
#define AFIndexTypeToDevice DXGI_FORMAT_R16_UINT
typedef ID3D11Buffer* AFBufObj;
#endif

#ifdef GL_TRUE
inline void afSafeDeleteBuffer(GLuint& b)
{
	if (b != 0) {
		glDeleteBuffers(1, &b);
		b = 0;
	}
}
#else
inline void afSafeDeleteBuffer(AFBufObj& b)
{
	SAFE_RELEASE(b);
}
#endif

AFBufObj afCreateIndexBuffer(const AFIndex* indi, int numIndi);
AFBufObj afCreateQuadListIndexBuffer(int numQuads);
AFBufObj afCreateVertexBuffer(int size, const void* buf);
AFBufObj afCreateDynamicVertexBuffer(int size);
void afWriteBuffer(AFBufObj bo, const void* buf, int size);
void afDrawIndexedTriangleList(AFBufObj ibo, int count, int start = 0);

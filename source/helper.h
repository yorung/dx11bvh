#define dimof(x) (sizeof(x) / sizeof(x[0]))

void *LoadFile(const char *fileName, int* size = nullptr);

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

#ifdef GL_TRUE
typedef GLuint AFbufObj;
#else
typedef ID3D11Buffer* AFbufObj;
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
inline void afSafeDeleteBuffer(AFbufObj& b)
{
	SAFE_RELEASE(b);
}
#endif

AFbufObj afCreateIndexBuffer(const unsigned short* indi, int numIndi);
AFbufObj afCreateQuadListIndexBuffer(int numQuads);
AFbufObj afCreateDynamicVertexBuffer(int size);
void afWriteBuffer(AFbufObj bo, const void* buf, int size);

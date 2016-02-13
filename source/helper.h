//#define dimof(x) (sizeof(x) / sizeof(x[0]))

void *LoadFile(const char *fileName, int* size = nullptr);
bool SaveFile(const char *fileName, const uint8_t* buf, int size);
double GetTime();
float Random();
void GoMyDir();
void Toast(const char *text);
void PlayBgm(const char *fileName);
bool LoadImageViaGdiPlus(const char* name, IVec2& size, std::vector<uint32_t>& col);
SRVID LoadTextureViaOS(const char* name, IVec2& size);

template <class T, size_t N> inline size_t dimof(T(&)[N])
{
	return N;
}

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

IBOID afCreateTiledPlaneIBO(int numTiles, int* numIndies = nullptr);
VBOID afCreateTiledPlaneVBO(int numTiles);

struct TexDesc {
	IVec2 size;
	int arraySize = 1;
	bool isCubeMap = false;
};

SRVID afLoadTexture(const char* name, TexDesc& desc);

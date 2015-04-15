//#define dimof(x) (sizeof(x) / sizeof(x[0]))

void *LoadFile(const char *fileName, int* size = nullptr);
bool SaveFile(const char *fileName, const uint8_t* buf, int size);
void GoMyDir();
double GetTime();
float Random();

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


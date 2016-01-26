#include "stdafx.h"

bool SaveFile(const char *fileName, const uint8_t* buf, int size)
{
	bool result = false;
	FILE *f = nullptr;

	if (fopen_s(&f, fileName, "wb")) {
		return false;
	}
	if (!fwrite(buf, size, 1, f)) {
		goto DONE;
	}
	result = !fclose(f);
	f = nullptr;
DONE:
	if (f) {
		fclose(f);
	}
	return result;
}

void *LoadFile(const char *fileName, int* size)
{
	bool result = false;
	FILE *f = nullptr;
	int _size;
	void *ptr = NULL;

	if (fopen_s(&f, fileName, "rb")) {
		return nullptr;
	}

	if (fseek(f, 0, SEEK_END)) {
		goto DONE;
	}
	_size = ftell(f);
	if (_size < 0) {
		goto DONE;
	}
	if (fseek(f, 0, SEEK_SET)) {
		goto DONE;
	}
	ptr = calloc(_size + 1, 1);
	if (!ptr) {
		goto DONE;
	}
	if (_size > 0) {
		if (!fread(ptr, _size, 1, f)) {
			goto DONE;
		}
	}
	result = true;
	if (size) {
		*size = _size;
	}
DONE:
	if (f) {
		fclose(f);
	}
	if (result){
		return ptr;
	} else {
		if (ptr) {
			free(ptr);
		}
		return nullptr;
	}
}

void GoMyDir()
{
	char dir[MAX_PATH];
	GetModuleFileNameA(GetModuleHandleA(nullptr), dir, MAX_PATH);
	char* p = strrchr(dir, '\\');
	assert(p);
	*p = '\0';
	p = strrchr(dir, '\\');
	assert(p);
	*p = '\0';
	SetCurrentDirectoryA(dir);
}

#pragma comment(lib, "winmm.lib")
void PlayBgm(const char* fileName)
{
//	PlaySoundA(fileName, NULL, SND_ASYNC | SND_LOOP);
	mciSendStringA(SPrintf("open \"%s\" type mpegvideo", fileName), NULL, 0, 0);
	mciSendStringA(SPrintf("play \"%s\" repeat", fileName), NULL, 0, 0);
}

static UINT StrToType(const char* type)
{
	if (!strcmp(type, "okcancel")) {
		return MB_OKCANCEL;
	} else if (!strcmp(type, "yesno")) {
		return MB_YESNO;
	}
	return MB_OK;
}

static const char* IdToStr(int id)
{
	switch (id) {
	case IDOK: return "ok";
	case IDCANCEL: return "cancel";
	case IDYES: return "yes";
	case IDNO: return "no";
	}
	return "unknown";
}

const char* StrMessageBox(const char* txt, const char* type)
{
	return IdToStr(MessageBoxA(GetActiveWindow(), txt, "MessageBox", StrToType(type)));
}

namespace Gdiplus {
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

bool LoadImageViaGdiPlus(const char* name, ivec2& size, std::vector<uint32_t>& col)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	WCHAR wc[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, name, -1, wc, dimof(wc));
	Gdiplus::Bitmap* image = new Gdiplus::Bitmap(wc);

	int w = (int)image->GetWidth();
	int h = (int)image->GetHeight();
	size.x = w;
	size.y = h;
	Gdiplus::Rect rc(0, 0, w, h);

	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
	image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);

	col.resize(w * h);
	for (int y = 0; y < h; y++) {
		memcpy(&col[y * w], (char*)bitmapData->Scan0 + bitmapData->Stride * y, w * 4);
		for (int x = 0; x < w; x++) {
			uint32_t& c = col[y * w + x];
			c = (c & 0xff00ff00) | ((c & 0xff) << 16) | ((c & 0xff0000) >> 16);
		}
	}
	image->UnlockBits(bitmapData);
	delete bitmapData;
	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return w && h;
}

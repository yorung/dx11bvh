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

double GetTime()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / f.QuadPart;
}

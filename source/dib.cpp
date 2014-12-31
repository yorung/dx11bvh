#include "stdafx.h"

DIB::DIB()
{
	m_w = m_h = 0;
	m_pitch = 0;
	m_p = 0;
	m_hbm = NULL;
	m_hdc = NULL;
	m_hbmOld = NULL;
	memset(&m_ds, 0, sizeof(m_ds));
}

DIB::~DIB()
{
	Destroy();
}

void DIB::Destroy()
{
	if (m_hbmOld) {
		SelectObject(m_hdc, m_hbmOld);
		m_hbmOld = NULL;
	}
	if (m_hdc) {
		DeleteDC(m_hdc);
		m_hdc = NULL;
	}
	if (m_hbm) {
		DeleteObject(m_hbm);
		m_hbm = NULL;
	}
}

bool DIB::Create(int w, int h)
{
	return Create(w, h, 32);
}

bool DIB::Create(int w, int h, int bits)
{
	Destroy();

	switch(bits) {
		case 8:
		case 32:
			break;
		default:
			return false;
	}

	m_w = w;
	m_h = h;

	struct BMI256 : BITMAPINFO {
		RGBQUAD bmiAdditionalColors[255];
	}bmi;
	BITMAPINFOHEADER *bh = &bmi.bmiHeader;
	bh->biSize = sizeof(BITMAPINFOHEADER);
	bh->biWidth = w;
	bh->biHeight = -h;	// top down
	bh->biPlanes = 1;
	bh->biBitCount = bits;
	bh->biCompression = BI_RGB;

	if (bits == 8) {
		for (int i = 0; i < 256; i++) {
			bmi.bmiColors[i].rgbRed = i;
			bmi.bmiColors[i].rgbGreen = i;
			bmi.bmiColors[i].rgbBlue = i;
			bmi.bmiColors[i].rgbReserved = 0;
		}
	}
	HWND hWnd = GetDesktopWindow();
	HDC hdc = GetDC(hWnd);
	if (!hdc){
		ReleaseDC(hWnd, hdc);
		return false;
	}
	m_hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&m_p, NULL, 0);
	if (m_hbm == NULL) {
		ReleaseDC(hWnd, hdc);
		Destroy();
		return false;
	}
	m_hdc = CreateCompatibleDC(hdc);
	if (m_hdc == NULL) {
		ReleaseDC(hWnd, hdc);
		return false;
	}
	m_hbmOld = (HBITMAP)SelectObject(m_hdc, m_hbm);
	GetObject(m_hbm, sizeof(DIBSECTION), &m_ds);
	m_pitch = m_ds.dsBm.bmWidthBytes;
	ReleaseDC(hWnd, hdc);
	return true;
}

void DIB::DibToGL()
{
	for(int y = 0; y < getH(); y++) {
		for (int x = 0; x < getW(); x++) {
			pixel px = getPixel(x, y);
			std::swap(*((BYTE*)(&px) + 0), *((BYTE*)(&px) + 2));
			setPixel(x, y, px);
		}
	}
}

void DIB::DibToGLFont()
{
	for (int y = 0; y < getH(); y++) {
		for (int x = 0; x < getW(); x++) {
			pixel px = getPixel(x, y);
			px = (px << 8) | 0x00ffffff;
			setPixel(x, y, px);
		}
	}
}

void DIB::FillAlpha()
{
	for (int y = 0; y < getH(); y++) {
		for (int x = 0; x < getW(); x++) {
			*referPixel(x, y) |= 0xff000000;
		}
	}
}

void DIB::Clear()
{
	for (int y = 0; y < getH(); y++) {
		for (int x = 0; x < getW(); x++) {
			setPixel(x, y, 0);
		}
	}
}

static bool dibCopy32(DIB *dib, BITMAPINFO *bi, void *bits)
{
	int srcH = bi->bmiHeader.biHeight;
	if (dib->getW() != bi->bmiHeader.biWidth || dib->getH() != abs(srcH)) {
		return false;
	}

	if (bi->bmiHeader.biBitCount != 32) {
		return false;
	}

	for (int y = 0; y < dib->getH(); y++) {
		int srcY = srcH > 0 ? srcH - 1 - y : y;	// support both bottom-up and top-down
		void *dst = dib->referPixel(0, y);
		void *src = (DWORD*)bits + dib->getW() * srcY;
		memcpy(dst, src, dib->getW() * sizeof(DWORD));
	}
	return true;
}

bool DIB::LoadFromBmp(const char *file)
{
	bool result = false;
	BITMAPINFO *bi;
	void *bits;
	void *tmp = NULL;
	int from;

	int size;
	tmp = LoadFile(file, &size);
	if (!tmp) {
		return false;
	}

	BITMAPFILEHEADER *fh = (BITMAPFILEHEADER*)tmp;
	if (fh->bfType != *(WORD*)"BM") {
		goto end;
	}

	bi = (BITMAPINFO*)((BYTE*)tmp + sizeof(*fh));
	bits = (BYTE*)tmp + fh->bfOffBits;

	if (!Create(bi->bmiHeader.biWidth, abs(bi->bmiHeader.biHeight))) {
		goto end;
	}

	StretchDIBits(m_hdc, 0, 0, m_w, m_h, 0, 0, m_w, m_h, bits, bi, DIB_RGB_COLORS, SRCCOPY);

	// StretchDIBits may ignore alpha channel
	// fill the alpha channel for textures
	FillAlpha();

	result = true;
end:
	if (tmp) {
		free(tmp);
	}
	return result;
}

bool DIB::blt(DIB& target, int dstX, int dstY)
{
	if (dstX < 0 || dstY < 0) {
		return false;
	}
	if (dstX + getW() > target.getW()) {
		return false;
	}
	if (dstX + getH() > target.getH()) {
		return false;
	}
	for (int y = 0; y < getH(); y++) {
		for (int x = 0; x < getW(); x++) {
			target.setPixel(dstX + x, dstY + y, getPixel(x, y));
		}
	}
	return true;
}

bool DIB::applySoftAA(DIB& target)
{
	if (getW() != target.getW() * 2) {
		return false;
	}
	if (getH() != target.getH() * 2) {
		return false;
	}
	for (int y = 0; y < target.getH(); y++) {
		for (int x = 0; x < target.getW(); x++) {
			pixel color[] = { getPixel(x * 2, y * 2), getPixel(x * 2 + 1, y * 2), getPixel(x * 2, y * 2 + 1), getPixel(x * 2 + 1, y * 2 + 1) };
			ivec4 sum;
			for (int i = 0; i < dimof(color); i++) {
				sum += uint32ToIvec4(color[i]);
			}
			sum /= 4;
			target.setPixel(x, y, ivec4ToUint32(sum));
		}
	}
	return true;
}

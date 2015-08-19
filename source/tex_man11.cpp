#include "stdafx.h"

TexMan11 texMan;

namespace Gdiplus {
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

static ID3D11ShaderResourceView* LoadTextureViaOS(const char* name)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	WCHAR wc[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, name, -1, wc, dimof(wc));
	Gdiplus::Bitmap* image = new Gdiplus::Bitmap(wc);

	int w = (int)image->GetWidth();
	int h = (int)image->GetHeight();
	Gdiplus::Rect rc(0, 0, w, h);

	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
	image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);

	std::vector<uint32_t> col;
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

	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, w, h, 1, 1, D3D11_BIND_SHADER_RESOURCE);
	D3D11_SUBRESOURCE_DATA r = { &col[0], (uint32_t)w * 4, 0 };
	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	deviceMan11.GetDevice()->CreateTexture2D(&desc, &r, &tex);
	deviceMan11.GetDevice()->CreateShaderResourceView(tex, nullptr, &srv);
	SAFE_RELEASE(tex);
	return srv;
}

struct DDSHeader {
	uint32_t h3[3];
	uint32_t h, w;
	uint32_t h2[2];
	uint32_t mipCnt;
	uint32_t h13[13];
	uint32_t fourcc, bitsPerPixel, rMask, gMask, bMask, aMask, caps1, caps2;
	bool IsCubeMap() const { return caps2 == 0xFE00; }
	int GetArraySize() const { return IsCubeMap() ? 6 : 1; }
};

static void bitScanForward(uint32_t* result, uint32_t mask)
{
	//	DWORD dwd;
	//	_BitScanForward(&dwd, mask);
	//	*result = dwd;

	for (int i = 0; i < 32; i++) {
		if (mask & (1 << i)) {
			*result = i;
			return;
		}
	}
	*result = 0;
}

static ID3D11ShaderResourceView* CreateTextureFromRowDDS(const void* img, int size, ivec2& texSize)
{
	const DDSHeader* hdr = (DDSHeader*)img;
	int w = (int)hdr->w;
	int h = (int)hdr->h;
	const uint32_t* im = (uint32_t*)img + 128 / 4;
	std::vector<uint32_t> col;
	uint32_t rShift, gShift, bShift, aShift;
	bitScanForward(&rShift, hdr->rMask);
	bitScanForward(&gShift, hdr->gMask);
	bitScanForward(&bShift, hdr->bMask);
	bitScanForward(&aShift, hdr->aMask);
	int arraySize = hdr->GetArraySize();
	for (int i = 0; i < arraySize; i++) {
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				uint32_t c = *im++;
				col.push_back(
					((hdr->aMask & c) >> aShift << 24) +
					((hdr->bMask & c) >> bShift << 16) +
					((hdr->gMask & c) >> gShift << 8) +
					((hdr->rMask & c) >> rShift));
			}
		}
	}
	texSize.x = hdr->w;
	texSize.y = hdr->h;

	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, w, h, arraySize, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, hdr->IsCubeMap() ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(hdr->IsCubeMap() ? D3D_SRV_DIMENSION_TEXTURECUBE : D3D_SRV_DIMENSION_TEXTURE2D, desc.Format, 0, 1);
	std::vector<D3D11_SUBRESOURCE_DATA> r;
	for (int i = 0; i < arraySize; i++) {
		r.push_back({ &col[w * h * i], (uint32_t)w * 4, 0 });
	}
	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	deviceMan11.GetDevice()->CreateTexture2D(&desc, &r[0], &tex);
	deviceMan11.GetDevice()->CreateShaderResourceView(tex, &srvDesc, &srv);
	SAFE_RELEASE(tex);
	return srv;
}

static ID3D11ShaderResourceView* LoadDDSTexture(const char* name, ivec2& texSize)
{
	int size;
	ID3D11ShaderResourceView* srv = nullptr;
	void* img = LoadFile(name, &size);
	if (!img) {
		aflog("LoadDDSTexture failed! %s", name);
		return 0;
	}
	const DDSHeader* hdr = (DDSHeader*)img;

	int blockSize = 16;
	DXGI_FORMAT format;
	switch (hdr->fourcc) {
	case 0x31545844: //'1TXD':
		format = DXGI_FORMAT_BC1_UNORM;
		blockSize = 8;
		break;
		//	case 0x33545844; //'3TXD':
		//		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		//		break;
		//	case 0x35545844; //'5TXD':
		//		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		//		break;
	default:
		srv = CreateTextureFromRowDDS(img, size, texSize);
		goto END;
	}
	texSize.x = hdr->w;
	texSize.y = hdr->h;

	{
		int arraySize = hdr->GetArraySize();
		CD3D11_TEXTURE2D_DESC desc(format, hdr->w, hdr->h, arraySize, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, hdr->IsCubeMap() ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(hdr->IsCubeMap() ? D3D_SRV_DIMENSION_TEXTURECUBE : D3D_SRV_DIMENSION_TEXTURE2D, desc.Format, 0, -1);
		std::vector<D3D11_SUBRESOURCE_DATA> r;
		int pitch = blockSize * ((hdr->w + 3) / 4);
		int slice = pitch * ((hdr->h + 3) / 4);
		for (int i = 0; i < arraySize; i++) {
			r.push_back({ (char*)img + 128 + slice * i, (uint32_t)pitch, 0 });
		}
		ID3D11Texture2D* tex = nullptr;
		deviceMan11.GetDevice()->CreateTexture2D(&desc, &r[0], &tex);
		deviceMan11.GetDevice()->CreateShaderResourceView(tex, &srvDesc, &srv);
		SAFE_RELEASE(tex);
	}
END:
	free(img);
	return srv;
}

TexMan11::TexMan11()
{
	texs.push_back(nullptr);	// make ID 0 invalid
}

TexMan11::~TexMan11()
{
	Destroy();
}

TexMan11::TMID TexMan11::Create(const char *name)
{
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}

	ID3D11ShaderResourceView *tex = nullptr;

	WCHAR wname[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, name, -1, wname, dimof(wname));
	if (!_stricmp(".dds", name + strlen(name) - 4)) {
		ivec2 texSize;
		tex = LoadDDSTexture(name, texSize);
	//	CreateDDSTextureFromFileEx(deviceMan11.GetDevice(), wname, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, nullptr, &tex);
	} else {
		tex = LoadTextureViaOS(name);
	//	CreateWICTextureFromFileEx(deviceMan11.GetDevice(), deviceMan11.GetContext(), wname, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, nullptr, &tex);
	}
	if (!tex) {
		return INVALID_TMID;
	}
	texs.push_back(tex);
	return nameToId[name] = texs.size() - 1;
}

static ID3D11ShaderResourceView* CreateWhiteTexture()
{
	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1, 1, D3D11_BIND_SHADER_RESOURCE);
	uint32_t white = 0xffffffff;
	D3D11_SUBRESOURCE_DATA r = { &white, 4, 4 };
	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	deviceMan11.GetDevice()->CreateTexture2D(&desc, &r, &tex);
	deviceMan11.GetDevice()->CreateShaderResourceView(tex, nullptr, &srv);
	SAFE_RELEASE(tex);
	return srv;
}

static ID3D11ShaderResourceView* CreateDynamicTexture(int w, int h)
{
	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, w, h, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	deviceMan11.GetDevice()->CreateTexture2D(&desc, nullptr, &tex);
	deviceMan11.GetDevice()->CreateShaderResourceView(tex, nullptr, &srv);
	SAFE_RELEASE(tex);
	return srv;
}

TexMan11::TMID TexMan11::CreateDynamicTexture(const char* name, int w, int h)
{
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	ID3D11ShaderResourceView* tex = ::CreateDynamicTexture(w, h);
	if (!tex) {
		return INVALID_TMID;
	}
	texs.push_back(tex);
	return nameToId[name] = texs.size() - 1;
}


TexMan11::TMID TexMan11::CreateWhiteTexture()
{
	const std::string name = "$WHITE";
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	ID3D11ShaderResourceView* tex = ::CreateWhiteTexture();
	if (!tex) {
		return INVALID_TMID;
	}
	texs.push_back(tex);
	return nameToId[name] = texs.size() - 1;
}

void TexMan11::Destroy()
{
	for (auto it = texs.begin(); it != texs.end(); it++)
	{
		SAFE_RELEASE(*it);
	}
	texs.clear();
}

ID3D11ShaderResourceView* TexMan11::Get(TMID id)
{
	if (id >= 0 && id < (TMID)texs.size())
	{
		return texs[id];
	}
	return nullptr;
}

ivec2 TexMan11::GetSize(TMID id)
{
	ID3D11ShaderResourceView* view = Get(id);
	assert(view);
	ID3D11Resource* res;
	view->GetResource(&res);
	assert(res);
	ID3D11Texture2D* tx;
	res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&tx));
	assert(tx);
	SAFE_RELEASE(res);

	D3D11_TEXTURE2D_DESC desc;
	tx->GetDesc(&desc);
	SAFE_RELEASE(tx);

	ivec2 sz;
	sz.x = (int)desc.Width;
	sz.y = (int)desc.Height;

	return sz;
}

void TexMan11::Write(TMID id, const void* buf)
{
	ID3D11ShaderResourceView* view = Get(id);
	assert(view);
	ID3D11Resource* res;
	view->GetResource(&res);
	assert(res);
	ID3D11Texture2D* tx;
	res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&tx));
	assert(tx);
	SAFE_RELEASE(res);

	D3D11_TEXTURE2D_DESC desc;
	tx->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE m;
	HRESULT hr = deviceMan11.GetContext()->Map(tx, 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, buf, desc.Width * desc.Height * 4);
	deviceMan11.GetContext()->Unmap(tx, 0);

	SAFE_RELEASE(tx);
}

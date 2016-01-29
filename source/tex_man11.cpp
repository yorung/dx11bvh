#include "stdafx.h"

TexMan11 texMan;

struct DDSHeader {
	uint32_t h3[3];
	int h, w;
	uint32_t h2[2];
	int mipCnt;
	uint32_t h13[13];
	uint32_t fourcc, bitsPerPixel, rMask, gMask, bMask, aMask, caps1, caps2;
	bool IsCubeMap() const { return caps2 == 0xFE00; }
	int GetArraySize() const { return IsCubeMap() ? 6 : 1; }
	int GetMipCnt() const { return std::max(mipCnt, 1); }
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

static void ArrangeRawDDS(void* img, int size)
{
	const DDSHeader* hdr = (DDSHeader*)img;
	DWORD rShift, gShift, bShift, aShift;
	_BitScanForward(&rShift, hdr->rMask);
	_BitScanForward(&gShift, hdr->gMask);
	_BitScanForward(&bShift, hdr->bMask);
	_BitScanForward(&aShift, hdr->aMask);
	std::for_each((uint32_t*)img + 128 / 4, (uint32_t*)img + size / 4, [&](uint32_t& im) {
		im = ((hdr->aMask & im) >> aShift << 24) + ((hdr->bMask & im) >> bShift << 16) + ((hdr->gMask & im) >> gShift << 8) + ((hdr->rMask & im) >> rShift);
		if (hdr->aMask == 0) {
			im |= 0xff000000;
		}
	});
}

static ComPtr<ID3D11ShaderResourceView> LoadDDSTexture(const char* name, ivec2& texSize)
{
	int size;
	ComPtr<ID3D11ShaderResourceView> srv;
	void* img = LoadFile(name, &size);
	if (!img) {
		aflog("LoadDDSTexture failed! %s", name);
		return 0;
	}
	const DDSHeader* hdr = (DDSHeader*)img;

	DXGI_FORMAT format;
	int (*pitchCalcurator)(int, int) = nullptr;
	switch (hdr->fourcc) {
	case 0x31545844: //'1TXD':
		format = DXGI_FORMAT_BC1_UNORM;
		pitchCalcurator = [](int w, int h) { return ((w + 3) / 4) * ((h + 3) / 4) * 8; };
		break;
	case 0x33545844: //'3TXD':
		format = DXGI_FORMAT_BC2_UNORM;
		pitchCalcurator = [](int w, int h) { return ((w + 3) / 4) * ((h + 3) / 4) * 16; };
		break;
	case 0x35545844: //'5TXD':
		format = DXGI_FORMAT_BC3_UNORM;
		pitchCalcurator = [](int w, int h) { return ((w + 3) / 4) * ((h + 3) / 4) * 16; };
		break;
	default:
		ArrangeRawDDS(img, size);
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
		pitchCalcurator = [](int w, int h) { return w * h * 4; };
		break;
	}
	texSize.x = hdr->w;
	texSize.y = hdr->h;

	int arraySize = hdr->GetArraySize();
	int mipCnt = hdr->GetMipCnt();
	CD3D11_TEXTURE2D_DESC desc(format, hdr->w, hdr->h, arraySize, hdr->GetMipCnt(), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, hdr->IsCubeMap() ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(hdr->IsCubeMap() ? D3D_SRV_DIMENSION_TEXTURECUBE : D3D_SRV_DIMENSION_TEXTURE2D, desc.Format, 0, -1);
	std::vector<D3D11_SUBRESOURCE_DATA> r;
	int offset = 128;
	for (int a = 0; a < arraySize; a++) {
		for (int m = 0; m < mipCnt; m++) {
			int w = std::max(1, hdr->w >> m);
			int h = std::max(1, hdr->h >> m);
			r.push_back({ (char*)img + offset, (uint32_t)pitchCalcurator(w, 1), 0 });
			offset += pitchCalcurator(w, h);
		}
	}
	ComPtr<ID3D11Texture2D> tex;
	deviceMan11.GetDevice()->CreateTexture2D(&desc, &r[0], &tex);
	deviceMan11.GetDevice()->CreateShaderResourceView(tex.Get(), &srvDesc, &srv);

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

	SRVID tex;

	WCHAR wname[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, name, -1, wname, dimof(wname));
	ivec2 texSize;
	if (!_stricmp(".dds", name + strlen(name) - 4)) {
		tex = LoadDDSTexture(name, texSize);
	//	CreateDDSTextureFromFileEx(deviceMan11.GetDevice(), wname, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, nullptr, &tex);
	} else {
		tex = LoadTextureViaOS(name, texSize);
	//	CreateWICTextureFromFileEx(deviceMan11.GetDevice(), deviceMan11.GetContext(), wname, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, nullptr, &tex);
	}
	if (!tex) {
		return INVALID_TMID;
	}
	texs.push_back(tex);
	return nameToId[name] = texs.size() - 1;
}

TexMan11::TMID TexMan11::CreateDynamicTexture(const char* name, const ivec2& size)
{
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	SRVID tex = afCreateDynamicTexture(AFDT_R8G8B8A8_UNORM, size);
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
	uint32_t white = 0xffffffff;
	SRVID tex = afCreateTexture2D(AFDT_R8G8B8A8_UNORM, ivec2(1, 1), &white);
	if (!tex) {
		return INVALID_TMID;
	}
	texs.push_back(tex);
	return nameToId[name] = texs.size() - 1;
}

void TexMan11::Destroy()
{
	texs.clear();
}

SRVID TexMan11::Get(TMID id)
{
	if (id >= 0 && id < (TMID)texs.size())
	{
		return texs[id];
	}
	return nullptr;
}

ivec2 TexMan11::GetSize(TMID id)
{
	ComPtr<ID3D11ShaderResourceView> view = Get(id);
	assert(view);
	ComPtr<ID3D11Resource> res;
	view->GetResource(&res);
	assert(res);
	ComPtr<ID3D11Texture2D> tx;
	res.As(&tx);
	assert(tx);

	D3D11_TEXTURE2D_DESC desc;
	tx->GetDesc(&desc);

	ivec2 sz;
	sz.x = (int)desc.Width;
	sz.y = (int)desc.Height;

	return sz;
}

void TexMan11::Write(TMID id, const void* buf)
{
	ComPtr<ID3D11ShaderResourceView> view = Get(id);
	assert(view);
	ComPtr<ID3D11Resource> res;
	view->GetResource(&res);
	assert(res);
	ComPtr<ID3D11Texture2D> tx;
	res.As(&tx);
	assert(tx);

	D3D11_TEXTURE2D_DESC desc;
	tx->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE m;
	HRESULT hr = deviceMan11.GetContext()->Map(tx.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, buf, desc.Width * desc.Height * 4);
	deviceMan11.GetContext()->Unmap(tx.Get(), 0);
}

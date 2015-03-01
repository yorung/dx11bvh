#include "stdafx.h"

TexMan11 texMan;

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
		CreateDDSTextureFromFileEx(deviceMan11.GetDevice(), wname, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, nullptr, &tex);
	} else {
		CreateWICTextureFromFileEx(deviceMan11.GetDevice(), deviceMan11.GetContext(), wname, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, nullptr, &tex);
	}
	if (!tex) {
		return INVALID_TMID;
	}
	texs.push_back(tex);
	return nameToId[name] = texs.size() - 1;
}

static ID3D11ShaderResourceView* CreateWhiteTexture()
{
	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1);
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

SIZE TexMan11::GetSize(TMID id)
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

	SIZE sz;
	sz.cx = (int)desc.Width;
	sz.cy = (int)desc.Height;

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

#include "stdafx.h"

TexMan texMan;

TexMan::TexMan()
{
	texs.push_back(nullptr);	// make ID 0 invalid
}

TexMan::~TexMan()
{
	Destroy();
}

TexMan::TMID TexMan::Create(const char *name)
{
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}

	TexDesc desc;
	SRVID tex = afLoadTexture(name, desc);
	if (!tex) {
		return INVALID_TMID;
	}
	texs.push_back(tex);
	return nameToId[name] = texs.size() - 1;
}

TexMan::TMID TexMan::CreateDynamicTexture(const char* name, const ivec2& size)
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


TexMan::TMID TexMan::CreateWhiteTexture()
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

void TexMan::Destroy()
{
	texs.clear();
}

SRVID TexMan::Get(TMID id)
{
	if (id >= 0 && id < (TMID)texs.size())
	{
		return texs[id];
	}
	return nullptr;
}

ivec2 TexMan::GetSize(TMID id)
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

void TexMan::Write(TMID id, const void* buf)
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

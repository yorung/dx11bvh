#include "stdafx.h"

BufferMan bufferMan;

BufferMan::BMID BufferMan::Create(int size)
{
	ID3D11Buffer *p = nullptr;
	HRESULT hr = deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), nullptr, &p);
	if (!p) {
		return -1;
	}
	insts.push_back(p);
	return insts.size() - 1;
}

void BufferMan::Destroy()
{
	for (auto it = insts.begin(); it != insts.end(); it++)
	{
		(*it)->Release();
	}
	insts.clear();
}

ID3D11Buffer* BufferMan::Get(BMID id)
{
	if (id >= 0 && id < (BMID)insts.size())
	{
		return insts[id];
	}
	return nullptr;
}

void BufferMan::Write(BMID id, const void* buf)
{
	auto p = Get(id);
	D3D11_BUFFER_DESC desc;
	p->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE m;
	HRESULT hr = deviceMan11.GetContext()->Map(p, 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, buf, desc.ByteWidth);
	deviceMan11.GetContext()->Unmap(p, 0);
}

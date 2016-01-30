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

#include "stdafx.h"

TexMan texMan;

TexMan::~TexMan()
{
	assert(nameToId.empty());
}

SRVID TexMan::Create(const char *name)
{
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	return nameToId[name] = afLoadTexture(name, TexDesc());
}

SRVID TexMan::CreateWhiteTexture()
{
	const std::string name = "$WHITE";
	auto it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}
	uint32_t white = 0xffffffff;
	return nameToId[name] = afCreateTexture2D(AFDT_R8G8B8A8_UNORM, IVec2(1, 1), &white);
}

void TexMan::Destroy()
{
	nameToId.clear();
}

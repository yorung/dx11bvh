#include "stdafx.h"

BufferMan bufferMan;

BufferMan::BMID BufferMan::Create(int size)
{
	UBOID id = afCreateUBO(size);
	if (!id) {
		return -1;
	}
	insts.push_back(id);
	return insts.size() - 1;
}

void BufferMan::Destroy()
{
	for (auto& it : insts) {
		afSafeDeleteBuffer(it);
	}
	insts.clear();
}

UBOID BufferMan::Get(BMID id)
{
	if (id >= 0 && id < (BMID)insts.size())	{
		return insts[id];
	}
	return UBOID();
}

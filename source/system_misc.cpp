#include "stdafx.h"

SystemMisc systemMisc;

void SystemMisc::SetScreenSize(IVec2 size)
{
	screenSize = size;
}

IVec2 SystemMisc::GetScreenSize()
{
	return screenSize;
}

#include "stdafx.h"

SystemMisc systemMisc;

void SystemMisc::SetScreenSize(ivec2 size)
{
	screenSize = size;
}

ivec2 SystemMisc::GetScreenSize()
{
	return screenSize;
}

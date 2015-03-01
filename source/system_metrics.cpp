#include "stdafx.h"

SystemMetrics systemMetrics;

void SystemMetrics::SetScreenSize(ivec2 size)
{
	screenSize = size;
}

ivec2 SystemMetrics::GetScreenSize()
{
	return screenSize;
}

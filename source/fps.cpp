#include "stdafx.h"

FPS fps;

FPS::FPS()
{
	std::fill_n(samples, dimof(samples), 0.0);
	ptr = 0;
}

void FPS::Update()
{
	samples[ptr] = GetTime();
	ptr = (ptr + 1) % dimof(samples);
}

float FPS::Get()
{
	int last = (ptr + dimof(samples) - 1) % dimof(samples);
	double diff = samples[last] - samples[ptr];
	return (float)(dimof(samples) / diff);
}

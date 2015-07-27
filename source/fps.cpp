#include "stdafx.h"

FPS fps;

FPS::FPS()
{
	std::fill_n(samples, dimof(samples), std::chrono::high_resolution_clock::now());
	ptr = 0;
}

void FPS::Update()
{
	samples[ptr] = std::chrono::high_resolution_clock::now();
	ptr = (ptr + 1) % dimof(samples);
}

float FPS::Get()
{
	int last = (ptr + dimof(samples) - 1) % dimof(samples);
	auto diff = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1>>>(samples[last] - samples[ptr]).count();
	return (float)dimof(samples) / diff;
}

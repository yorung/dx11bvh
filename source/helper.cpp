#include "stdafx.h"

float Random()
{
	static std::mt19937 seed{ std::random_device()() };
	return std::uniform_real_distribution<float>(0.0, 1.0)(seed);
}

double GetTime()
{
	static auto start = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(now - start).count();
}

IBOID afCreateTiledPlaneIBO(int numTiles, int* numIndies)
{
	const int numVert = numTiles + 1;

	std::vector<AFIndex> indi;
	for (int y = 0; y < numTiles; y++) {
		if (y != 0) {
			indi.push_back(y * numVert);
		}
		indi.push_back(y * numVert);
		for (int x = 0; x < numTiles; x++) {
			indi.push_back((y + 1) * numVert + x);
			indi.push_back(y * numVert + x + 1);
		}
		indi.push_back((y + 1) * numVert + numVert - 1);
		if (y != numTiles - 1) {
			indi.push_back((y + 1) * numVert + numVert - 1);
		}
	}

	if (numIndies) {
		*numIndies = indi.size();
	}

	return afCreateIndexBuffer(&indi[0], indi.size());
}

VBOID afCreateTiledPlaneVBO(int numTiles)
{
	std::vector<Vec2> v;
	for (int y = 0; y <= numTiles; y++) {
		for (int x = 0; x <= numTiles; x++) {
			v.push_back((Vec2)ivec2(x, y) / (float)numTiles * 2 - Vec2(1, 1));
		}
	}
	return afCreateVertexBuffer(v.size() * sizeof(v[0]), &v[0]);
}

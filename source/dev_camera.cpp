#include "stdafx.h"

DevCamera devCamera;

static float INVALID_POS = -99999.f;

DevCamera::DevCamera()
{
	dist = 1;
	lastX = lastY = INVALID_POS;
	rotX = rotY = 0;
	height = 0;
}

void DevCamera::SetDistance(float dist_)
{
	dist = dist_;
}

float DevCamera::GetDistance()
{
	return dist;
}

void DevCamera::MouseWheel(float delta)
{
	if (delta > 0) {
		dist /= 1.1f;
	}
	if (delta < 0) {
		dist *= 1.1f;
	}
}

void DevCamera::LButtonDown(float x, float y)
{
	lastX = x;
	lastY = y;
}

void DevCamera::LButtonUp(float x, float y)
{
	MouseMove(x, y);
	lastX = lastY = INVALID_POS;
}

void DevCamera::MouseMove(float x, float y)
{
	if (lastX <= INVALID_POS || lastY <= INVALID_POS) {
		return;
	}
	rotX += (x - lastX) * (float)M_PI * 2.0f;
	rotY += (y - lastY) * (float)M_PI * 2.0f;

	lastX = x;
	lastY = y;
}

Mat DevCamera::CalcViewMatrix()
{
	Mat cam = translate(0, height, -dist) * q2m(Quat(Vec3(1, 0, 0), rotY)) * q2m(Quat(Vec3(0, 1, 0), rotX));
	return fastInv(cam);
}

void DevCamera::SetHeight(float height)
{
	height = height;
}

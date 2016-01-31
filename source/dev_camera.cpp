#include "stdafx.h"

DevCamera devCamera;

static float INVALID_POS = -99999.f;

DevCamera::DevCamera()
{
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
	lButton = true;
	lastX = x;
	lastY = y;
}

void DevCamera::LButtonUp(float x, float y)
{
	MouseMove(x, y);
	lButton = false;
}

void DevCamera::RButtonDown(float x, float y)
{
	rButton = true;
	lastX = x;
	lastY = y;
}

void DevCamera::RButtonUp(float x, float y)
{
	MouseMove(x, y);
	rButton = false;
}

void DevCamera::MouseMove(float x, float y)
{
	if (lButton) {
		rotX -= (x - lastX) * (float)M_PI * 1.8f;
		rotY -= (y - lastY) * (float)M_PI * 1.0f;
	}
	if (rButton) {
		fov += (y - lastY) * 10.0f;
	}

	lastX = x;
	lastY = y;
}

Mat DevCamera::GetViewMatrix()
{
	Mat cam = translate(0, height, -dist) * q2m(Quat(Vec3(1, 0, 0), rotY) * Quat(Vec3(0, 1, 0), rotX));
	return fastInv(cam);
}

Mat DevCamera::GetProjMatrix()
{
	float dist = GetDistance();
	float f = dist * 1000;
	float n = dist / 1000;
	return perspectiveLH(fov * (float)M_PI / 180.f, (float)SCR_W / SCR_H, n, f);
}

void DevCamera::SetHeight(float height_)
{
	height = height_;
}

class DevCamera
{
	float scale;
	float lastX;
	float lastY;
	float rotX;
	float rotY;
	float height;
public:
	DevCamera();
	void SetScale(float scale);
	void MouseWheel(float delta);
	void LButtonDown(float x, float y);
	void LButtonUp(float x, float y);
	void MouseMove(float x, float y);
	float GetScale();
	Mat CalcViewMatrix();
	void SetHeight(float height);
};
extern DevCamera devCamera;

class DevCamera
{
	float dist;
	float lastX;
	float lastY;
	float rotX;
	float rotY;
	float height;
public:
	DevCamera();
	void SetDistance(float dist);
	void MouseWheel(float delta);
	void LButtonDown(float x, float y);
	void LButtonUp(float x, float y);
	void MouseMove(float x, float y);
	float GetDistance();
	Mat GetViewMatrix();
	Mat GetProjMatrix();
	void SetHeight(float height);
};
extern DevCamera devCamera;

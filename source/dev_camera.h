class DevCamera
{
	float dist = 1.0f;
	float lastX = 0;
	float lastY = 0;
	float rotX = 2.83f;
	float rotY = 0.429f;
	float height = 0;
	bool lButton = false;
	bool rButton = false;
	float fov = 60.0f;
public:
	DevCamera();
	void SetDistance(float dist);
	void MouseWheel(float delta);
	void RButtonDown(float x, float y);
	void RButtonUp(float x, float y);
	void LButtonDown(float x, float y);
	void LButtonUp(float x, float y);
	void MouseMove(float x, float y);
	float GetDistance();
	Mat GetViewMatrix();
	Mat GetProjMatrix();
	void SetHeight(float height);
};
extern DevCamera devCamera;

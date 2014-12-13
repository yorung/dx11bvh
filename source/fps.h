class FPS
{
	const static int SAMPLE_MAX = 60;
	double samples[SAMPLE_MAX];
	int ptr;
public:
	FPS();
	void Update();
	float Get();
};

extern FPS fps;

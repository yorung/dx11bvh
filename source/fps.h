class FPS
{
	const static int SAMPLE_MAX = 60;
	std::chrono::high_resolution_clock::time_point samples[SAMPLE_MAX];
	int ptr;
public:
	FPS();
	void Update();
	float Get();
};

extern FPS fps;

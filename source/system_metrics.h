class SystemMetrics
{
	ivec2 screenSize;
public:
	void SetScreenSize(ivec2 size);
	ivec2 GetScreenSize();
};
extern SystemMetrics systemMetrics;
class PostEffectMan
{
	ShaderMan::SMID shaderId;
	SAMPLERID sampler;
	UBOID ubo;
public:
	PostEffectMan();
	~PostEffectMan();
	void Create(const char *shader);
	void Draw(SRVID srv);
	void Destroy();
};

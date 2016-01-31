class SkyMan
{
	SRVID texId = 0;
	TexDesc texDesc;
	ShaderMan::SMID shaderId = ShaderMan::INVALID_SMID;
	UBOID uboId;
	SAMPLERID sampler;
public:
	~SkyMan();
	void Create(const char *texFileName, const char* shader);
	void Draw();
	void Destroy();
};

extern SkyMan skyMan;

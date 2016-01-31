class TexMan
{
public:
	~TexMan();
	SRVID Create(const char *name);
	SRVID CreateWhiteTexture();
	void Destroy();
private:
	std::map<std::string, SRVID> nameToId;
};

extern TexMan texMan;

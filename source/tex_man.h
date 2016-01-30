class TexMan
{
public:
	typedef unsigned int TMID;
	static const TMID INVALID_TMID = 0;
	TexMan();
	~TexMan();
	TMID Create(const char *name);
	TMID CreateWhiteTexture();
	void Destroy();
	SRVID Get(TMID id);
private:
	std::map<std::string, TMID> nameToId;
	std::vector<SRVID> texs;
};

extern TexMan texMan;

inline void afBindTextureToBindingPoint(TexMan::TMID tex, UINT textureBindingPoint)
{
	afBindTextureToBindingPoint(texMan.Get(tex), textureBindingPoint);
}

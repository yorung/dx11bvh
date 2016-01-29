class TexMan
{
public:
	typedef unsigned int TMID;
	static const TMID INVALID_TMID = 0;
	TexMan();
	~TexMan();
	TMID Create(const char *name);
	TMID CreateWhiteTexture();
	TMID CreateDynamicTexture(const char* name, const ivec2& size);
	void Destroy();
	SRVID Get(TMID id);
	ivec2 GetSize(TMID id);
	void Write(TMID id, const void* buf);
private:
	std::map<std::string, TMID> nameToId;
	std::vector<SRVID> texs;
};

extern TexMan texMan;

inline void afBindTextureToBindingPoint(TexMan::TMID tex, UINT textureBindingPoint)
{
	afBindTextureToBindingPoint(texMan.Get(tex), textureBindingPoint);
}

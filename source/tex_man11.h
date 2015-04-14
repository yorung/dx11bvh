class TexMan11
{
public:
	typedef unsigned int TMID;
	static const TMID INVALID_TMID = 0;
	TexMan11();
	~TexMan11();
	TMID Create(const char *name);
	TMID CreateWhiteTexture();
	TMID CreateDynamicTexture(const char* name, int w, int h);
	void Destroy();
	ID3D11ShaderResourceView* Get(TMID id);
	ivec2 GetSize(TMID id);
	void Write(TMID id, const void* buf);
private:
	std::map<std::string, TMID> nameToId;
	std::vector<ID3D11ShaderResourceView*> texs;
};

extern TexMan11 texMan;
typedef TexMan11 TexMan;

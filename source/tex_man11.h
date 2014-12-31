class TexMan11
{
public:
	typedef int TMID;
	static const TMID INVALID_TMID = 0;
	TexMan11();
	~TexMan11();
	TMID Create(const char *name);
	TMID CreateWhiteTexture();
	void Destroy();
	ID3D11ShaderResourceView* Get(TMID id);
	SIZE GetSize(TMID id);
private:
	std::map<std::string, TMID> nameToId;
	std::vector<ID3D11ShaderResourceView*> texs;
};

extern TexMan11 texMan;
typedef TexMan11 TexMan;

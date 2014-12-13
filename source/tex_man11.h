class TexMan11
{
public:
	typedef int TMID;
	TMID Create(const char *name);
	void Destroy();
	ID3D11ShaderResourceView* Get(TMID id);
	SIZE GetSize(TMID id);
private:
	std::map<std::string, TMID> nameToId;
	std::vector<ID3D11ShaderResourceView*> texs;
};

extern TexMan11 texMan;
typedef TexMan11 TexMan;

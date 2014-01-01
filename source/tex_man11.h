class TexMan11
{
public:
	typedef int TMID;
	std::map<std::string, TMID> m_nameToId;
	std::vector<ID3D11ShaderResourceView*> m_texs;
public:
	TMID Create(const char *name, bool for3D);
	void Destroy();
	ID3D11ShaderResourceView* Get(TMID id);
	SIZE GetSize(TMID id);
};

extern TexMan11 texMan;
typedef TexMan11 TexMan;

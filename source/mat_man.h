struct Material
{
	Vec4 faceColor;
	float power;
	Vec4 specular;
	Vec4 emissive;
	TexMan::TMID tmid;
	bool operator==(const Material& r) const;
};

class MatMan
{
public:
	typedef int MMID;
	std::vector<Material> m_mats;
public:
	MMID Create(const Material& mat);
	void Destroy();
	const Material* Get(MMID id);
};

extern MatMan matMan;

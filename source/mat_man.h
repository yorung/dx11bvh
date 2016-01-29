class Material
{
public:
	Material() {}
	Material(const Material& r) { *this = r; }
	const Material& operator=(const Material& r);
	bool operator==(const Material& r) const;
	Vec4 faceColor;
	float power = 0;
	Vec4 specular;
	Vec4 emissive;
	TexMan::TMID tmid = 0;
};

class MatMan
{
public:
	typedef int MMID;
	std::vector<Material> mats;
public:
	MMID Create(const Material& mat);
	void Destroy();
	const Material* Get(MMID id);
};

extern MatMan matMan;

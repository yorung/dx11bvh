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
	SRVID texture;
};

class MatMan
{
	std::vector<Material> mats;
public:
	~MatMan();
	typedef int MMID;
	MMID Create(const Material& mat);
	void Destroy();
	const Material* Get(MMID id);
};

extern MatMan matMan;

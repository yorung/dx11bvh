#include "stdafx.h"

MatMan matMan;

const Material& Material::operator=(const Material& r)
{
	faceColor = r.faceColor;
	power = r.power;
	specular = r.specular;
	emissive = r.emissive;
	tmid = r.tmid;
	return *this;
}

bool Material::operator==(const Material& r) const
{
	return !memcmp(this, &r, sizeof(Material));
}

MatMan::MMID MatMan::Create(const Material& mat)
{
	auto it = std::find_if(mats.begin(), mats.end(), [&mat] (const Material& m) { return m == mat; });
	if (it != mats.end()) {
		int n = (int)std::distance(mats.begin(), it);
		return n;
	}
	mats.push_back(mat);
	return mats.size() - 1;
}

void MatMan::Destroy()
{
	mats.clear();
}

const Material* MatMan::Get(MMID id)
{
	if (id >= 0 && id < (MMID)mats.size())
	{
		return &mats[id];
	}
	return nullptr;
}

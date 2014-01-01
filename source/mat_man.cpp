#include "stdafx.h"

MatMan matMan;

bool Material::operator==(const Material& r) const
{
	return !memcmp(this, &r, sizeof(Material));
}

MatMan::MMID MatMan::Create(const Material& mat)
{
	auto it = std::find_if(m_mats.begin(), m_mats.end(), [&mat] (const Material& m) { return m == mat; });
	if (it != m_mats.end()) {
		int n = (int)std::distance(m_mats.begin(), it);
		return n;
	}
	m_mats.push_back(mat);
	return m_mats.size() - 1;
}

void MatMan::Destroy()
{
	m_mats.clear();
}

const Material* MatMan::Get(MMID id)
{
	if (id >= 0 && id < (MMID)m_mats.size())
	{
		return &m_mats[id];
	}
	return nullptr;
}

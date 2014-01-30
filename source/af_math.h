template <class VEC3> inline VEC3 normalize(const VEC3& v)
{
	VEC3 r = v;
	r.Normalize();
	return r;
}

template <class VEC3> inline float dot(const VEC3& l, const VEC3& r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

template <class VEC3> inline VEC3 cross(const VEC3& l, const VEC3& r)
{
	return l.Cross(r);
}

inline Matrix inv(const Matrix& m)
{
	XMVECTOR dummy;
	return XMMatrixInverse(&dummy, m);
}

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
#define _(u,v) (l.u * r.v - l.v * r.u)
	return VEC3(_(y,z), _(z,x), _(x,y));
#undef _
}

template <class VEC3> inline float length(const VEC3& v)
{
	return v.Length();
}

inline Matrix inv(const Matrix& m)
{
	XMVECTOR dummy;
	return XMMatrixInverse(&dummy, m);
}

inline Matrix q2m(const Quaternion& q)
{
	return Matrix::CreateFromQuaternion(q);
}

inline Quaternion m2q(const Matrix& m)
{
	return Quaternion::CreateFromRotationMatrix(m);
}

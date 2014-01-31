typedef float affloat;

struct Vec3
{
	affloat x, y, z;
	Vec3() : Vec3(0, 0, 0) {}
	Vec3(affloat X, affloat Y, affloat Z) : x(X), y(Y), z(Z) {}

	Vec3(const Vector3& v) : Vec3(v.x, v.y, v.z) {}
	operator Vector3() const { return Vector3(x, y, z); }

	Vec3 operator+(const Vec3& r) const { return Vec3(x + r.x, y + r.y, z + r.z); }
	Vec3 operator-(const Vec3& r) const { return Vec3(x - r.x, y - r.y, z - r.z); }
	Vec3 operator*(affloat r) const { return Vec3(x * r, y * r, z * r); }
	Vec3 operator/(affloat r) const { return Vec3(x / r, y / r, z / r); }
};

template <class VEC3> inline affloat dot(const VEC3& l, const VEC3& r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

template <class VEC3> inline VEC3 cross(const VEC3& l, const VEC3& r)
{
#define _(u,v) (l.u * r.v - l.v * r.u)
	return VEC3(_(y,z), _(z,x), _(x,y));
#undef _
}

template <class VEC3> inline affloat length(const VEC3& v)
{
	return sqrtf(dot(v, v));
}

template <class VEC3> inline VEC3 normalize(const VEC3& v)
{
	return v / length(v);
}

inline Matrix inv(const Matrix& m)
{
	XMVECTOR dummy;
	return XMMatrixInverse(&dummy, m);
}

inline Quaternion inv(const Quaternion& q)
{
	Quaternion r;
	q.Inverse(r);
	return r;
}

inline Matrix q2m(const Quaternion& q)
{
	return Matrix::CreateFromQuaternion(q);
}

inline Quaternion m2q(const Matrix& m)
{
	return Quaternion::CreateFromRotationMatrix(m);
}

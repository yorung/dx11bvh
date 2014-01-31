typedef float affloat;

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

struct Quat
{
	Vec3 v;
	affloat w;
	Quat() : Quat(0, Vec3()) {}
	Quat(float W, const Vec3& V) : w(W), v(V) {}

	Quat(const Quaternion& q) : Quat(q.w, Vec3(q.x, q.y, q.z)) {}
	operator Quaternion() const { return Quaternion(v.x, v.y, v.z, w); }

	Quat operator*(const Quat& r) const { return Quat(w * r.w - dot(v, r.v), w * r.v + r.w * v + cross(v, r.v)); }
	const Quat& operator*=(const Quat& r) { *this = *this * r; return *this; }
};

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

inline Vec3 transform(const Vec3& v, const Matrix& m)
{
#define _(c) (m._1##c## * v.x + m._2##c## * v.y + m._3##c## * v.z + m._4##c##)
	return Vec3(_(1), _(2), _(3));
#undef _
}

inline Matrix translate(affloat x, affloat y, affloat z)
{
	Matrix m;
	m._41 = x;
	m._42 = y;
	m._43 = z;
	return m;
}
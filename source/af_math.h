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

inline float afsqrt(float s) { return sqrtf(s); }
inline double afsqrt(double s) { return sqrt(s); }
inline float afsin(float s) { return sinf(s); }
inline double afsin(double s) { return sin(s); }
inline float afcos(float s) { return cosf(s); }
inline double afcos(double s) { return cos(s); }

template <class VEC3> inline affloat length(const VEC3& v)
{
	return afsqrt(dot(v, v));
}

template <class VEC3> inline VEC3 normalize(const VEC3& v)
{
	return v / length(v);
}

template <class QUAT> inline QUAT inv(const QUAT& q)
{
	QUAT c = q.Conjugate();
	affloat lenSq = q.w * q.w + dot(q.v, q.v);
	return QUAT(c.w / lenSq, c.v / lenSq);
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

	Vec3 operator-() const { return Vec3(-x, -y, -z); }
};

struct Quat
{
	Vec3 v;
	affloat w;
	Quat() : Quat(0, Vec3()) {}
	Quat(affloat W, const Vec3& V) : w(W), v(V) {}
	Quat(const Vec3& axis, affloat angle) { w = afcos(angle / 2); v = axis * afsin(angle / 2); }

	Quat(const Quaternion& q) : Quat(q.w, Vec3(q.x, q.y, q.z)) {}
	operator Quaternion() const { return Quaternion(v.x, v.y, v.z, w); }

	Quat operator*(const Quat& r) const { return Quat(w * r.w - dot(v, r.v), r.v * w + v * r.w + cross(r.v, v)); }
	const Quat& operator*=(const Quat& r) { *this = *this * r; return *this; }
	Quat Conjugate() const { return Quat(w, -v); }
};

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
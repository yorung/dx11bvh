#define USE_DXMATH

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

template <class T> inline void swap(T& a, T& b) { T t = a; a = b; b = t; }

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

	Vec3 operator-=(const Vec3& r) { return *this = *this - r; }
	Vec3 operator*=(affloat r) { return *this = *this * r; }
};

struct Quat
{
	Vec3 v;
	affloat w;
	Quat() : Quat(1, Vec3()) {}
	Quat(affloat W, const Vec3& V) : w(W), v(V) {}
	Quat(const Vec3& axis, affloat angle) { w = afcos(angle / 2); v = axis * afsin(angle / 2); }

	Quat(const Quaternion& q) : Quat(q.w, Vec3(q.x, q.y, q.z)) {}
	operator Quaternion() const { return Quaternion(v.x, v.y, v.z, w); }

	Quat operator*(const Quat& r) const { return Quat(w * r.w - dot(v, r.v), r.v * w + v * r.w + cross(r.v, v)); }
	const Quat& operator*=(const Quat& r) { *this = *this * r; return *this; }
	Quat Conjugate() const { return Quat(w, -v); }
};

struct Mat
{
	union {
		affloat m[4][4];
		struct {
			affloat _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44;
		};
	};

	Mat() {
		_11 = _22 = _33 = _44 = 1;
		_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0;
	};

	Mat(affloat m11, affloat m12, affloat m13, affloat m14, affloat m21, affloat m22, affloat m23, affloat m24, affloat m31, affloat m32, affloat m33, affloat m34, affloat m41, affloat m42, affloat m43, affloat m44) :
		_11(m11), _12(m12), _13(m13), _14(m14),
		_21(m21), _22(m22), _23(m23), _24(m24),
		_31(m31), _32(m32), _33(m33), _34(m34),
		_41(m41), _42(m42), _43(m43), _44(m44) {}

	Mat(const Matrix& mtx) { assert(sizeof(float) == sizeof(affloat)); memcpy(m, mtx.m, sizeof(m)); }
	operator Matrix() const { return Matrix(_11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44); }
};

inline Matrix inv(const Matrix& mtx)
{
#ifdef USE_DXMATH
	return mtx.Invert();
#else
	Mat l = mtx;
	Mat r;
	for (int j = 0; j < 4; j++) {
		int d = j;

		affloat maxf = 0;
		int maxi = -1;
		for (int i = d; i < 4; i++) {
			affloat v = abs(l.m[i][j]);
			if (v > maxf) {
				maxf = v;
				maxi = i;
			}
		}
		if (maxi < 0) {
			return Mat();	// fail
		}
		if (maxi != d) {
			for (int jj = 0; jj < 4; jj++) {
				swap(l.m[d][jj], l.m[maxi][jj]);
				swap(r.m[d][jj], r.m[maxi][jj]);
			}
		}

		affloat diag = l.m[d][j];
		assert(diag);
		for (int jj = 0; jj < 4; jj++) {
			l.m[d][jj] /= diag;
			r.m[d][jj] /= diag;
		}

		for (int i = 0; i < 4; i++) {
			if (i == j) {
				continue;
			}
			affloat mult = -l.m[i][j];
			for (int jj = 0; jj < 4; jj++) {
				l.m[i][jj] += l.m[d][jj] * mult;
				r.m[i][jj] += r.m[d][jj] * mult;
			}
		}
	}
	return r;
#endif
}

inline Mat q2m(const Quat& q)
{
#define D(a,b) (1 - 2 * (q.v.a * q.v.a + q.v.b * q.v.b)) // diagonal
#define P(a,b,c) (2 * (q.v.a * q.v.b + q.v.c * q.w)) // positive
#define N(a,b,c) (2 * (q.v.a * q.v.b - q.v.c * q.w)) // negative
	return Mat(D(y,z), P(x,y,z), N(x,z,y), 0,
		   N(x,y,z), D(x,z), P(y,z,x), 0,
		   P(x,z,y), N(y,z,x), D(x,y), 0, 0,0,0,1);
#undef D
#undef P
#undef N
}

inline Quat m2q(const Matrix& m_)
{
	Mat m = m_ * Matrix::CreateScale(1.0f / length(Vec3(m_._11, m_._12, m_._13)));	// kill scaling if needed
#ifdef USE_DXMATH
	return Quaternion::CreateFromRotationMatrix(m);
#else

	affloat x, y, z, w = afsqrt(m._11 + m._22 + m._33 + 1) / 2;
	if (w > 0.5f) {							 // w is the largest
		z = (m._12 - m._21) / (w * 4);
		y = (m._31 - m._13) / (w * 4);
		x = (m._23 - m._32) / (w * 4);
	} else if (m._11 > m._22 && m._11 > m._33) { // x is the largest
		x = afsqrt((-m._11 + m._22 + m._33 - 1) / -4);
		y = (m._12 + m._21) / (x * 4);
		z = (m._31 + m._13) / (x * 4);
		w = (m._23 - m._32) / (x * 4);
	} else if (m._22 > m._33) {					// y is the largest
		y = afsqrt((m._11 - m._22 + m._33 - 1) / -4);
		x = (m._12 + m._21) / (y * 4);
		z = (m._31 - m._13) / (y * 4);
		w = (m._23 + m._32) / (y * 4);
	} else {									// z is the largest
		z = afsqrt((m._11 + m._22 - m._33 - 1) / -4);
		w = (m._12 - m._21) / (z * 4);
		x = (m._31 + m._13) / (z * 4);
		y = (m._23 + m._32) / (z * 4);
	}
	return Quat(w, Vec3(x,y,z));
#endif
}

inline Vec3 transform(const Vec3& v, const Matrix& m)
{
#define _(c) (m._1##c## * v.x + m._2##c## * v.y + m._3##c## * v.z + m._4##c##)
	return Vec3(_(1), _(2), _(3));
#undef _
}

inline Mat translate(affloat x, affloat y, affloat z)
{
	Mat m;
	m._41 = x;
	m._42 = y;
	m._43 = z;
	return m;
}

inline Matrix v2m(const Vec3& v)
{
	return translate(v.x, v.y, v.z);
}


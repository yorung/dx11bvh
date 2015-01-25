//#define USE_DXMATH
//#define USE_SIMPLE_MATH

typedef float affloat;

inline affloat clamp(affloat x, affloat mi, affloat ma) { return std::max(std::min(x, ma), mi); }


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
inline float afacos(float s) { return acosf(s); }
inline double afacos(double s) { return acos(s); }

struct Vec2
{
	affloat x, y;
	Vec2() : x(0), y(0) {}
	Vec2(affloat X, affloat Y) : x(X), y(Y) {}
#ifdef USE_SIMPLE_MATH
	Vec2(const Vector2& v) : Vec2(v.x, v.y) {}
	operator Vector2() const { return Vector2(x, y); }
#endif
	Vec2 operator+(const Vec2& r) const { return Vec2(x + r.x, y + r.y); }
	Vec2 operator-(const Vec2& r) const { return Vec2(x - r.x, y - r.y); }
	Vec2 operator*(const Vec2& r) const { return Vec2(x * r.x, y * r.y); }
	Vec2 operator/(const Vec2& r) const { return Vec2(x / r.x, y / r.y); }
	Vec2 operator*(affloat r) const { return Vec2(x * r, y * r); }
	Vec2 operator/(affloat r) const { return Vec2(x / r, y / r); }

	Vec2 operator-() const { return Vec2(-x, -y); }

	Vec2 operator-=(const Vec2& r) { return *this = *this - r; }
	Vec2 operator*=(affloat r) { return *this = *this * r; }
};

struct Vec3
{
	affloat x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(affloat X, affloat Y, affloat Z) : x(X), y(Y), z(Z) {}
#ifdef USE_SIMPLE_MATH
	Vec3(const Vector3& v) : Vec3(v.x, v.y, v.z) {}
	operator Vector3() const { return Vector3(x, y, z); }
#endif
	Vec3 operator+(const Vec3& r) const { return Vec3(x + r.x, y + r.y, z + r.z); }
	Vec3 operator-(const Vec3& r) const { return Vec3(x - r.x, y - r.y, z - r.z); }
	Vec3 operator*(const Vec3& r) const { return Vec3(x * r.x, y * r.y, z * r.z); }
	Vec3 operator*(affloat r) const { return Vec3(x * r, y * r, z * r); }
	Vec3 operator/(affloat r) const { return Vec3(x / r, y / r, z / r); }

	Vec3 operator-() const { return Vec3(-x, -y, -z); }

	Vec3 operator-=(const Vec3& r) { return *this = *this - r; }
	Vec3 operator+=(const Vec3& r) { return *this = *this + r; }
	Vec3 operator*=(affloat r) { return *this = *this * r; }
};

struct Vec4
{
	affloat x, y, z, w;
	Vec4() : x(0), y(0), z(0), w(0) {}
	Vec4(affloat X, affloat Y, affloat Z, affloat W) : x(X), y(Y), z(Z), w(W) {}
#ifdef USE_SIMPLE_MATH
	Vec4(const Vector4& v) : Vec4(v.x, v.y, v.z, v.w) {}
#endif
};

struct ivec3
{
	int x, y, z;
	ivec3() : x(0), y(0), z(0) {}
	ivec3(int X, int Y, int Z) : x(X), y(Y), z(Z) {}
};

struct ivec4
{
	int x, y, z, w;
	ivec4() : x(0), y(0), z(0), w(0) {}
	ivec4(int X, int Y, int Z, int W) : x(X), y(Y), z(Z), w(W) {}
	ivec4 operator+(const ivec4& r) const { return ivec4(x + r.x, y + r.y, z + r.z, w + r.w); }
	ivec4 operator+=(const ivec4& r) { return *this = *this + r; }
	ivec4 operator/(int r) const { return ivec4(x / r, y / r, z / r, w / r); }
	ivec4 operator/=(int r) { return *this = *this / r; }
};

inline affloat dot(const Vec3& l, const Vec3& r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

inline affloat dot(const Vec2& l, const Vec2& r)
{
	return l.x * r.x + l.y * r.y;
}

inline float frac(float v)
{
	float i;
	return modff(v, &i);
}

inline double frac(double v)
{
	double i;
	return modf(v, &i);
}

inline Vec2 frac(const Vec2& v)
{
	return Vec2(frac(v.x), frac(v.y));
}

inline Vec2 floor(const Vec2& v)
{
	return Vec2(floor(v.x), floor(v.y));
}

inline Vec2 ceil(const Vec2& v)
{
	return Vec2(ceil(v.x), ceil(v.y));
}

inline Vec2 max(const Vec2& a, const Vec2& b) {
	return Vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}

inline Vec2 min(const Vec2& a, const Vec2& b) {
	return Vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}

template <class V> inline affloat lengthSq(const V& v)
{
	return dot(v, v);
}

template <class V3> inline affloat length(const V3& v)
{
	return afsqrt(lengthSq(v));
}

template <class V> inline V normalize(const V& v)
{
	return v / length(v);
}

template <class T> inline T lerp(const T& x, const T& y, affloat s)
{
	return x + (y - x) * s;
}

struct Quat
{
	Vec3 v;
	affloat w;
	Quat() { *this = Quat(1, Vec3()); }
	Quat(affloat W, const Vec3& V) : w(W), v(V) {}
	Quat(const Vec3& axis, affloat angle) { w = afcos(angle / 2); v = normalize(axis) * afsin(angle / 2); }
#ifdef USE_SIMPLE_MATH
	Quat(const Quaternion& q) : Quat(q.w, Vec3(q.x, q.y, q.z)) {}
	operator Quaternion() const { return Quaternion(v.x, v.y, v.z, w); }
#endif
	Quat operator*(const Quat& r) const { return Quat(w * r.w - dot(v, r.v), r.v * w + v * r.w + cross(r.v, v)); }
	Quat operator+(const Quat& r) const { return Quat(w + r.w, v + r.v); }
	Quat operator*(affloat s) const { return Quat(w * s, v * s); }
	const Quat& operator*=(const Quat& r) { *this = *this * r; return *this; }
	Quat Conjugate() const { return Quat(w, -v); }
};

inline affloat dot(const Quat& l, const Quat& r)
{
	return dot(l.v, r.v) + l.w * r.w;
}

inline Quat slerp(const Quat& l, Quat r, affloat ratio)
{
#ifdef USE_DXMATH
	return Quaternion::Slerp(l, r, ratio);
#else
	affloat dotlr = dot(l, r);
	if (dotlr < 0) {
		dotlr = -dotlr;
		r = r.Conjugate();
		r.w = -r.w;
	}

	affloat angle = afacos(clamp(dotlr, -1, 1));
	if (angle == 0) {
		return l;
	}
	affloat sinangle = afsin(angle);
	if (sinangle == 0) {
		return l;
	}
	affloat reciprocal = 1 / sinangle;
	Quat afr = l * afsin((1 - ratio) * angle) * reciprocal + r * afsin(ratio * angle) * reciprocal;
#ifdef USE_SIMPLE_MATH
	Quat dx = Quaternion::Slerp(l, r, ratio);
	assert(abs(afr.w - dx.w) < 0.01f);
	assert(abs(afr.v.x - dx.v.x) < 0.01f);
	assert(abs(afr.v.y - dx.v.y) < 0.01f);
	assert(abs(afr.v.z - dx.v.z) < 0.01f);
#endif
	return afr;
#endif
}

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

	Mat operator*(const Mat& r) const {
#define m(i,j) (_##i##1 * r._##1##j + _##i##2 * r._##2##j + _##i##3 * r._##3##j +_##i##4 * r._##4##j)
		return Mat(m(1,1), m(1,2), m(1,3), m(1,4), m(2,1), m(2,2), m(2,3), m(2,4), m(3,1), m(3,2), m(3,3), m(3,4), m(4,1), m(4,2), m(4,3), m(4,4));
#undef m
	}
	const Mat& operator*=(const Mat& r) { *this = *this * r; return *this; }
	const bool operator==(const Mat& r) { return !memcmp(m, r.m, sizeof(m)); }
#ifdef USE_SIMPLE_MATH
	Mat(const Matrix& mtx) { assert(sizeof(float) == sizeof(affloat)); memcpy(m, mtx.m, sizeof(m)); }
	operator Matrix() const { return Matrix(_11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44); }

	Mat(const XMMATRIX& mtx) : Mat(Matrix(mtx)) {}
	operator XMMATRIX() const { return Matrix(*this); }
#endif
	Vec3 GetRow(int i) const { return Vec3(m[i][0], m[i][1], m[i][2]); }
	void SetRow(int i, const Vec3& v) { m[i][0] = v.x; m[i][1] = v.y; m[i][2] = v.z; }
};


// http://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process
inline Vec3 proj(Vec3 u, Vec3 v)
{
	return u * (dot(u, v) / dot(u, u));
}

inline Mat orthogonalize(Mat v)
{
	Vec3 vx = v.GetRow(0);
	Vec3 vy = v.GetRow(1);
	Vec3 vz = v.GetRow(2);

	Vec3 ux = vx;
	Vec3 uy = vy - proj(ux, vy);
	Vec3 uz = vz - proj(ux, vz) - proj(uy, vz);

	ux = normalize(ux);
	uy = normalize(uy);
	uz = normalize(uz);
	ux *= length(vx);
	uy *= length(vx);
	uz *= length(vx);

	Mat u;
	u.SetRow(0, ux);
	u.SetRow(1, uy);
	u.SetRow(2, uz);
	u.SetRow(3, v.GetRow(3));
	return u;
}

inline Quat inv(const Quat& q)
{
	Quat c = q.Conjugate();
	affloat lenSq = q.w * q.w + dot(q.v, q.v);
	return Quat(c.w / lenSq, c.v / lenSq);
}

inline Mat inv(const Mat& mtx)
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
				std::swap(l.m[d][jj], l.m[maxi][jj]);
				std::swap(r.m[d][jj], r.m[maxi][jj]);
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

inline Mat scale(affloat x, affloat y, affloat z)
{
	return Mat(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
}

inline Mat scale(affloat s)
{
	return scale(s, s, s);
}

inline Mat q2m(const Quat& q)
{
#ifdef USE_DXMATH
	return Matrix::CreateFromQuaternion(q);
#else
#define D(a,b) (1 - 2 * (q.v.a * q.v.a + q.v.b * q.v.b)) // diagonal
#define P(a,b,c) (2 * (q.v.a * q.v.b + q.v.c * q.w)) // positive
#define N(a,b,c) (2 * (q.v.a * q.v.b - q.v.c * q.w)) // negative
	return Mat(D(y,z), P(x,y,z), N(x,z,y), 0,
		   N(x,y,z), D(x,z), P(y,z,x), 0,
		   P(x,z,y), N(y,z,x), D(x,y), 0, 0,0,0,1);
#undef D
#undef P
#undef N
#endif
}

inline Quat m2q(const Mat& m_)
{
	Mat m = m_ * scale(1.0f / length(Vec3(m_._11, m_._12, m_._13)));	// kill scaling if needed
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
		w = (m._31 - m._13) / (y * 4);
		z = (m._23 + m._32) / (y * 4);
	} else {									// z is the largest
		z = afsqrt((m._11 + m._22 - m._33 - 1) / -4);
		w = (m._12 - m._21) / (z * 4);
		x = (m._31 + m._13) / (z * 4);
		y = (m._23 + m._32) / (z * 4);
	}
	return Quat(w, Vec3(x,y,z));
#endif
}

inline Vec3 transform(const Vec3& v, const Mat& m)
{
#define _(c) (m._1##c * v.x + m._2##c * v.y + m._3##c * v.z + m._4##c)
	return Vec3(_(1), _(2), _(3));
#undef _
}

inline Vec3 transform(const Vec3& v, const Quat& q)
{
	return (q.Conjugate() * Quat(0, v) * q).v;
}

inline Mat translate(affloat x, affloat y, affloat z)
{
	Mat m;
	m._41 = x;
	m._42 = y;
	m._43 = z;
	return m;
}

inline Mat v2m(const Vec3& v)
{
	return translate(v.x, v.y, v.z);
}

inline Mat transpose(const Mat& mtx)
{
#define m(i) mtx._1##i, mtx._2##i, mtx._3##i, mtx._4##i
	return Mat(m(1), m(2), m(3), m(4));
#undef m
}

inline Mat fastInv(const Mat& mtx)
{
#define m(i) mtx._1##i, mtx._2##i, mtx._3##i, 0
	Mat r(m(1), m(2), m(3), 0, 0, 0, 1);
#undef m
	r.SetRow(3, -transform(mtx.GetRow(3), r));
	return r;
}

inline ivec4 uint32ToIvec4(uint32_t col) {
	return ivec4(col >> 24, (col & 0x00ff0000) >> 16, (col & 0xff00) >> 8, col & 0xff);
}

inline uint32_t ivec4ToUint32(const ivec4& v) {
	return (uint32_t(0xff && v.x) << 24) | (uint32_t(0xff & v.y) << 16) | (uint32_t(0xff & v.z) << 8) | (uint32_t(v.w) & 0xff);
}

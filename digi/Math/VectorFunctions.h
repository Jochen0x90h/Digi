#ifndef digi_Math_VectorFunctions_h
#define digi_Math_VectorFunctions_h

#include <algorithm>

#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>


namespace digi {

/// @addtogroup Math
/// @{

/**
	vector functions are applied component-wise
	
	getXY(a)
	getXYZ(a)
	
	abs(a) absolute value
	sign(a) sign
	sqrt(a) square root
	sqr(a) square
	floor(a)
	ceil(a)
	round(a)
	trunc(a) truncate fractional part
	frac(a) fractional part
	sin(a)
	cos(a)
	tan(a)
	atan2(a, b)
	exp(a)
	pow(a, b)
	log(a) logarithm base e
	log10(a) logarithm base 10
	log2(a) logarithm base 2
	conj(a) complex conjugate
	min(a, b)
	max(a, b)
	min(a)
	max(a)
	min(a, b, c)
	max(a, b, c)
	sum(a)
	prod(a)
	mod(a, b)
	dot(a, b) dot product
	mac(a, b) multiply accumulate
	select(a, b, c) c ? b : a
	
	normalize(a) returns a normalized vector
	lengthSquared(a) squared length of vector
	length(a) length of vector
	distance(a, b) distance between two vectors
	reflect(a, n)
	perp(a)
	cross(a, b)
	cross3(a, b, c)
	angle(a, b)

	any(a)
	all(a)
*/

// ----------------------------------------------------------------------------
// getXY(a), getXYZ(a)

#ifndef BUILTIN_VECTORS

template <typename Type>
Vector2<Type> getXY(const Vector3<Type>& a)
{
	return vector2(a.x, a.y);
}

template <typename Type>
Vector2<Type> getXY(const Vector4<Type>& a)
{
	return vector2(a.x, a.y);
}

template <typename Type>
Vector3<Type> getXYZ(const Vector4<Type>& a)
{
	return vector3(a.x, a.y, a.z);
}

#else

template <typename Type>
Vector3<Type> getXYZ(const Vector4<Type>& a)
{
	return a.xyz;
}

#endif


// ----------------------------------------------------------------------------
/// abs(a)

// scalar
using std::abs;

// vector2
template <typename Type>
Vector2<DIGI_REAL(Type)> abs(const Vector2<Type>& a)
{
	return vector2(abs(a.x), abs(a.y));
}

// vector3
template <typename Type>
Vector3<DIGI_REAL(Type)> abs(const Vector3<Type>& a)
{
	return vector3(abs(a.x), abs(a.y), abs(a.z));
}

// vector4
template <typename Type>
Vector4<DIGI_REAL(Type)> abs(const Vector4<Type>& a)
{
	return vector4(abs(a.x), abs(a.y), abs(a.z), abs(a.w));
}


// ----------------------------------------------------------------------------
/// sign(a)

// scalar
static inline float sign(float a)
{
	return (a > 0.0f) ? 1.0f : (a < 0.0f) ? -1.0f : 0.0f;
}

static inline double sign(double a)
{
	return (a > 0.0) ? 1.0f : (a < 0.0) ? -1.0f : 0.0;
}

// vector2
template <typename Type>
Vector2<Type> sign(const Vector2<Type>& a)
{
	return vector2(sign(a.x), sign(a.y));
}

// vector3
template <typename Type>
Vector3<Type> sign(const Vector3<Type>& a)
{
	return vector3(sign(a.x), sign(a.y), sign(a.z));
}

// vector4
template <typename Type>
Vector4<Type> sign(const Vector4<Type>& a)
{
	return vector4(sign(a.x), sign(a.y), sign(a.z), sign(a.w));
}


// ----------------------------------------------------------------------------
/// sqrt(a)

// scalar
using std::sqrt;

// vector2
template <typename Type>
Vector2<Type> sqrt(const Vector2<Type>& a)
{
	return vector2(sqrt(a.x), sqrt(a.y));
}

// vector3
template <typename Type>
Vector3<Type> sqrt(const Vector3<Type>& a)
{
	return vector3(sqrt(a.x), sqrt(a.y), sqrt(a.z));
}

// vector4
template <typename Type>
Vector4<Type> sqrt(const Vector4<Type>& a)
{
	return vector4(sqrt(a.x), sqrt(a.y), sqrt(a.z), sqrt(a.w));
}


// ----------------------------------------------------------------------------
/// sqr(a)

// scalar
static inline int sqr(int a)
{
	return a * a;
}

static inline float sqr(float a)
{
	return a * a;
}

static inline double sqr(double a)
{
	return a * a;
}


// ----------------------------------------------------------------------------
/// floor(a)

// scalar
using std::floor;

// vector2
template <typename Type>
Vector2<Type> floor(const Vector2<Type>& a)
{
	return vector2(floor(a.x), floor(a.y));
}

// vector3
template <typename Type>
Vector3<Type> floor(const Vector3<Type>& a)
{
	return vector3(floor(a.x), floor(a.y), floor(a.z));
}

// vector4
template <typename Type>
Vector4<Type> floor(const Vector4<Type>& a)
{
	return vector4(floor(a.x), floor(a.y), floor(a.z), floor(a.w));
}


// ----------------------------------------------------------------------------
/// ceil(a)

// scalar
using std::ceil;

// vector2
template <typename Type>
Vector2<Type> ceil(const Vector2<Type>& a)
{
	return vector2(ceil(a.x), ceil(a.y));
}

// vector3
template <typename Type>
Vector3<Type> ceil(const Vector3<Type>& a)
{
	return vector3(ceil(a.x), ceil(a.y), ceil(a.z));
}

// vector4
template <typename Type>
Vector4<Type> ceil(const Vector4<Type>& a)
{
	return vector4(ceil(a.x), ceil(a.y), ceil(a.z), ceil(a.w));
}


// ----------------------------------------------------------------------------
/// round(a)

// scalar
#ifdef HAVE_C99_MATH
	static inline float round(float a)
	{
		return ::roundf(a);
	}
	
	using ::round;
#else
	static inline float round(float a)
	{
		return a < 0.0f ? ceil(a - 0.5f) : floor(a + 0.5f);
	}

	static inline double round(double a)
	{
		return a < 0.0 ? ceil(a - 0.5) : floor(a + 0.5);
	}
#endif

// vector2
template <typename Type>
Vector2<Type> round(const Vector2<Type>& a)
{
	return vector2(round(a.x), round(a.y));
}

// vector3
template <typename Type>
Vector3<Type> round(const Vector3<Type>& a)
{
	return vector3(round(a.x), round(a.y), round(a.z));
}

// vector4
template <typename Type>
Vector4<Type> round(const Vector4<Type>& a)
{
	return vector4(round(a.x), round(a.y), round(a.z), round(a.w));
}


// ----------------------------------------------------------------------------
/// trunc(a)

// scalar
#ifdef HAVE_C99_MATH
	static inline float trunc(float a)
	{
		return ::truncf(a);
	}
	
	using ::trunc;
#else
	static inline float trunc(float a)
	{
		return a < 0.0f ? ceil(a) : floor(a);
	}
	static inline double trunc(double a)
	{
		return a < 0.0 ? ceil(a) : floor(a);
	}
#endif


// ----------------------------------------------------------------------------
/// frac(a)

// scalar
static inline float frac(float a)
{
	return a - floor(a);
}

static inline double frac(double a)
{
	return a - floor(a);
}

// vector2
template <typename Type>
Vector2<Type> frac(const Vector2<Type>& a)
{
	return vector2(frac(a.x), frac(a.y));
}

// vector3
template <typename Type>
Vector3<Type> frac(const Vector3<Type>& a)
{
	return vector3(frac(a.x), frac(a.y), frac(a.z));
}

// vector4
template <typename Type>
Vector4<Type> frac(const Vector4<Type>& a)
{
	return vector4(frac(a.x), frac(a.y), frac(a.z), frac(a.w));
}


// ----------------------------------------------------------------------------
// sin(a)

// scalar
using std::sin;

// vector2
template <typename Type>
Vector2<Type> sin(const Vector2<Type>& a)
{
	return vector2(sin(a.x), sin(a.y));
}

// vector3
template <typename Type>
Vector3<Type> sin(const Vector3<Type>& a)
{
	return vector3(sin(a.x), sin(a.y), sin(a.z));
}

// vector4
template <typename Type>
Vector4<Type> sin(const Vector4<Type>& a)
{
	return vector4(sin(a.x), sin(a.y), sin(a.z), sin(a.w));
}


// ----------------------------------------------------------------------------
// cos(a)

// scalar
using std::cos;

// vector2
template <typename Type>
Vector2<Type> cos(const Vector2<Type>& a)
{
	return vector2(cos(a.x), cos(a.y));
}

// vector3
template <typename Type>
Vector3<Type> cos(const Vector3<Type>& a)
{
	return vector3(cos(a.x), cos(a.y), cos(a.z));
}

// vector4
template <typename Type>
Vector4<Type> cos(const Vector4<Type>& a)
{
	return vector4(cos(a.x), cos(a.y), cos(a.z), cos(a.w));
}


// ----------------------------------------------------------------------------
// tan(a)

// scalar
using std::tan;


// ----------------------------------------------------------------------------
// atan2(a, b)

// scalar
using std::atan2;


// ----------------------------------------------------------------------------
// exp(a)

// scalar
using std::exp;

// vector2
template <typename Type>
Vector2<Type> exp(const Vector2<Type>& a)
{
	return vector2(exp(a.x), exp(a.y));
}

// vector3
template <typename Type>
Vector3<Type> exp(const Vector3<Type>& a)
{
	return vector3(exp(a.x), exp(a.y), exp(a.z));
}

// vector4
template <typename Type>
Vector4<Type> exp(const Vector4<Type>& a)
{
	return vector4(exp(a.x), exp(a.y), exp(a.z), exp(a.w));
}


// ----------------------------------------------------------------------------
// pow(a, b)

// scalar
using std::pow;

// vector2
template <typename Type>
Vector2<Type> pow(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(pow(a.x, b.x), pow(a.y, b.y));
}

// vector3
template <typename Type>
Vector3<Type> pow(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z));
}

// vector4
template <typename Type>
Vector4<Type> pow(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z), pow(a.w, b.w));
}


// ----------------------------------------------------------------------------
// log(a)

// scalar
using std::log;

// vector2
template <typename Type>
Vector2<Type> log(const Vector2<Type>& a)
{
	return vector2(log(a.x), log(a.y));
}

// vector3
template <typename Type>
Vector3<Type> log(const Vector3<Type>& a)
{
	return vector3(log(a.x), log(a.y), log(a.z));
}

// vector4
template <typename Type>
Vector4<Type> log(const Vector4<Type>& a)
{
	return vector4(log(a.x), log(a.y), log(a.z), log(a.w));
}


// ----------------------------------------------------------------------------
// log10(a)

// scalar
using std::log10;

// vector2
template <typename Type>
Vector2<Type> log10(const Vector2<Type>& a)
{
	return vector2(log10(a.x), log10(a.y));
}

// vector3
template <typename Type>
Vector3<Type> log10(const Vector3<Type>& a)
{
	return vector3(log10(a.x), log10(a.y), log10(a.z));
}

// vector4
template <typename Type>
Vector4<Type> log10(const Vector4<Type>& a)
{
	return vector4(log10(a.x), log10(a.y), log10(a.z), log10(a.w));
}


// ----------------------------------------------------------------------------
// log2(a)

// scalar
static inline float log2(float a)
{
	return log(a) * 1.442695040888963f;
}

static inline double log2(double a)
{
	return log(a) * 1.442695040888963;
}

// vector2
template <typename Type>
Vector2<Type> log2(const Vector2<Type>& a)
{
	return vector2(log2(a.x), log2(a.y));
}

// vector3
template <typename Type>
Vector3<Type> log2(const Vector3<Type>& a)
{
	return vector3(log2(a.x), log2(a.y), log2(a.z));
}

// vector4
template <typename Type>
Vector4<Type> log2(const Vector4<Type>& a)
{
	return vector4(log2(a.x), log2(a.y), log2(a.z), log2(a.w));
}


// ----------------------------------------------------------------------------
/// conj(a)

// scalar
static inline float conj(float a)
{
	return a;
}

static inline double conj(double a)
{
	return a;
}


// ----------------------------------------------------------------------------
// min(a, b)

// scalar (define for all arithmetic types)
template <typename Type>
Type min(const Type& a, const Type& b, typename boost::enable_if_c<boost::is_arithmetic<Type>::value>::type* dummy = 0)
{
	return a < b ? a : b;
}

// vector2
template <typename Type>
Vector2<Type> min(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(min(a.x, b.x), min(a.y, b.y));
}

template <typename Type>
Vector2<Type> min(const Vector2<Type>& a, const Type& b)
{
	return vector2(min(a.x, b), min(a.y, b));
}

// vector3
template <typename Type>
Vector3<Type> min(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

template <typename Type>
Vector3<Type> min(const Vector3<Type>& a, const Type& b)
{
	return vector3(min(a.x, b), min(a.y, b), min(a.z, b));
}

// vector4
template <typename Type>
Vector4<Type> min(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
}

template <typename Type>
Vector4<Type> min(const Vector4<Type>& a, const Type& b)
{
	return vector4(min(a.x, b), min(a.y, b), min(a.z, b), min(a.w, b));
}


// ----------------------------------------------------------------------------
// max(a, b)

// scalar (define for all arithmetic types)
template <typename Type>
Type max(const Type& a, const Type& b, typename boost::enable_if_c<boost::is_arithmetic<Type>::value>::type* dummy = 0)
{
	return a > b ? a : b;
}

// vector2
template <typename Type>
Vector2<Type> max(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(max(a.x, b.x), max(a.y, b.y));
}

template <typename Type>
Vector2<Type> max(const Vector2<Type>& a, const Type& b)
{
	return vector2(max(a.x, b), max(a.y, b));
}

// vector3
template <typename Type>
Vector3<Type> max(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

template <typename Type>
Vector3<Type> max(const Vector3<Type>& a, const Type& b)
{
	return vector3(max(a.x, b), max(a.y, b), max(a.z, b));
}

// vector4
template <typename Type>
Vector4<Type> max(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
}

template <typename Type>
Vector4<Type> max(const Vector4<Type>& a, const Type& b)
{
	return vector4(max(a.x, b), max(a.y, b), max(a.z, b), max(a.w, b));
}


// ----------------------------------------------------------------------------
/// min(a)

// scalar (define for all arithmetic types)
template <typename Type>
Type min(const Type& a, typename boost::enable_if_c<boost::is_arithmetic<Type>::value>::type* dummy = 0)
{
	return a;
}

// vector2
template <typename Type>
Type min(const Vector2<Type>& a)
{
	return min(a.x, a.y);
}

// vector3
template <typename Type>
Type min(const Vector3<Type>& a)
{
	return min(min(a.x, a.y), a.z);
}

// vector4
template <typename Type>
Type min(const Vector4<Type>& a)
{
	return min(min(a.x, a.y), min(a.z, a.w));
}


// ----------------------------------------------------------------------------
/// max(a)

// scalar (define for all arithmetic types)
template <typename Type>
Type max(const Type& a, typename boost::enable_if_c<boost::is_arithmetic<Type>::value>::type* dummy = 0)
{
	return a;
}

// vector2
template <typename Type>
Type max(const Vector2<Type>& a)
{
	return max(a.x, a.y);
}

// vector3
template <typename Type>
Type max(const Vector3<Type>& a)
{
	return max(max(a.x, a.y), a.z);
}

// vector4
template <typename Type>
Type max(const Vector4<Type>& a)
{
	return max(max(a.x, a.y), max(a.z, a.w));
}


// ----------------------------------------------------------------------------
// min(a, b, c)

template <typename Type1, typename Type2, typename Type3>
Type1 min(const Type1& a, const Type2& b, const Type3& c)
{
	return min(min(a, b), c);
}


// ----------------------------------------------------------------------------
// max(a, b, c)

template <typename Type1, typename Type2, typename Type3>
Type1 max(const Type1& a, const Type2& b, const Type3& c)
{
	return max(max(a, b), c);
}


// ----------------------------------------------------------------------------
/// sum(a)

// scalar
static inline float sum(float a)
{
	return a;
}

static inline double sum(double a)
{
	return a;
}

// vector2
template <typename Type>
Type sum(const Vector2<Type>& a)
{
	return a.x + a.y;
}

// vector3
template <typename Type>
Type sum(const Vector3<Type>& a)
{
	return a.x + a.y + a.z;
}

// vector4
template <typename Type>
Type sum(const Vector4<Type>& a)
{
	return a.x + a.y + a.z + a.w;
}


// ----------------------------------------------------------------------------
/// prod(a)

// scalar
static inline float prod(float a)
{
	return a;
}

static inline double prod(double a)
{
	return a;
}

// vector2
template <typename Type>
Type prod(const Vector2<Type>& a)
{
	return a.x * a.y;
}

// vector3
template <typename Type>
Type prod(const Vector3<Type>& a)
{
	return a.x * a.y * a.z;
}

// vector4
template <typename Type>
Type prod(const Vector4<Type>& a)
{
	return a.x * a.y * a.z * a.w;
}


// ----------------------------------------------------------------------------
// mod(a, b)

// scalar
static inline int mod(int a, int b)
{
	return a % b;
}

static inline float mod(float a, float b)
{
	return std::fmod(a, b);
}

static inline double mod(double a, double b)
{
	return std::fmod(a, b);
}

// vector2
template <typename Type>
Vector2<Type> mod(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(mod(a.x, b.x), mod(a.y, b.y));
}

template <typename Type>
Vector2<Type> mod(const Vector2<Type>& a, const Type& b)
{
	return vector2(mod(a.x, b), mod(a.y, b));
}

// vector3
template <typename Type>
Vector3<Type> mod(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(mod(a.x, b.x), mod(a.y, b.y), mod(a.z, b.z));
}

template <typename Type>
Vector3<Type> mod(const Vector3<Type>& a, const Type& b)
{
	return vector3(mod(a.x, b), mod(a.y, b), mod(a.z, b));
}

// vector4
template <typename Type>
Vector4<Type> mod(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(mod(a.x, b.x), mod(a.y, b.y), mod(a.z, b.z), mod(a.w, b.w));
}

template <typename Type>
Vector4<Type> mod(const Vector4<Type>& a, const Type& b)
{
	return vector4(mod(a.x, b), mod(a.y, b), mod(a.z, b), mod(a.w, b));
}


// ----------------------------------------------------------------------------
// dot(a, b)

// scalar
static inline float dot(float a, float b)
{
	return a * b;
}

static inline double dot(double a, double b)
{
	return a * b;
}

// vector2
template <typename Type>
Type dot(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return dot(a.x, b.x) + dot(a.y, b.y);
}

// vector3
template <typename Type>
Type dot(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return dot(a.x, b.x) + dot(a.y, b.y) + dot(a.z, b.z);
}

// vector4
template <typename Type>
Type dot(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return dot(a.x, b.x) + dot(a.y, b.y) + dot(a.z, b.z) + dot(a.w, b.w);
}


// ----------------------------------------------------------------------------
// mac(a, b)

// vector2
template <typename Type>
Type mac(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return a.x * b.x + a.y * b.y;
}

// vector3
template <typename Type>
Type mac(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// vector4
template <typename Type>
Type mac(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}


// ----------------------------------------------------------------------------
// select(a, b, c)

// scalar
static inline float select(float a, float b, bool c)
{
	return c ? b : a;
}

static inline double select(double a, double b, bool c)
{
	return c ? b : a;
}

// vector2
template <typename Type>
Vector2<Type> select(const Vector2<Type>& a, const Vector2<Type>& b, const Vector2<bool>& c)
{
	return vector2(
		c.x ? b.x : a.x,
		c.y ? b.y : a.y);
}

// vector3
template <typename Type>
Vector3<Type> select(const Vector3<Type>& a, const Vector3<Type>& b, const Vector3<bool>& c)
{
	return vector3(
		c.x ? b.x : a.x,
		c.y ? b.y : a.y,
		c.z ? b.z : a.z);
}

// vector4
template <typename Type>
Vector4<Type> select(const Vector4<Type>& a, const Vector4<Type>& b, const Vector4<bool>& c)
{
	return vector4(
		c.x ? b.x : a.x,
		c.y ? b.y : a.y,
		c.z ? b.z : a.z,
		c.w ? b.w : a.w);
}


// ----------------------------------------------------------------------------
// lengthSquared(a)

// vector2
template <typename Type>
Type lengthSquared(const Vector2<Type>& a)
{
	return mac(a, a);
}

// vector3
template <typename Type>
Type lengthSquared(const Vector3<Type>& a)
{
	return mac(a, a);
}

// vector4
template <typename Type>
Type lengthSquared(const Vector4<Type>& a)
{
	return mac(a, a);
}


// ----------------------------------------------------------------------------
// length(a)

// vector2
template <typename Type>
Type length(const Vector2<Type>& a)
{
	return sqrt(mac(a, a));
}

// vector3
template <typename Type>
Type length(const Vector3<Type>& a)
{
	return sqrt(mac(a, a));
}

// vector4
template <typename Type>
Type length(const Vector4<Type>& a)
{
	return sqrt(mac(a, a));
}


// ----------------------------------------------------------------------------
// normalize(a)

// vector2
template <typename Type>
Vector2<Type> normalize(const Vector2<Type>& a)
{
	return a / length(a);
}

// vector3
template <typename Type>
Vector3<Type> normalize(const Vector3<Type>& a)
{
	return a / length(a);
}

// vector4
template <typename Type>
Vector4<Type> normalize(const Vector4<Type>& a)
{
	return a / length(a);
}


// ----------------------------------------------------------------------------
// distance(a, b)

// vector2
template <typename Type>
Type distance(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return length(b - a);
}

// vector3
template <typename Type>
Type distance(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return length(b - a);
}

// vector4
template <typename Type>
Type distance(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return length(b - a);
}


// ----------------------------------------------------------------------------
// reflect(a, n)

// vector2
template <typename Type>
Vector2<Type> reflect(const Vector2<Type>& a, const Vector2<Type>& n)
{
	return a - (2.0f * dot(a, n)) * n;
}

// vector3
template <typename Type>
Vector3<Type> reflect(const Vector3<Type>& a, const Vector3<Type>& n)
{
	return a - (2.0f * dot(a, n)) * n;
}

// vector4
template <typename Type>
Vector4<Type> reflect(const Vector4<Type>& a, const Vector4<Type>& n)
{
	return a - (2.0f * dot(a, n)) * n;
}


// ----------------------------------------------------------------------------
// perp(a)

/// perpendicular-operator: perp(a) = dual(a)
template <typename Type>
Vector2<Type> perp(const Vector2<Type>& a)
{
	return vector2(a.y, -a.x);
}


// ----------------------------------------------------------------------------
// cross(a, b)

/// cross product for 3D-Vectors: cross(a, b) = dual(a ^ b)
template <typename Type>
Vector3<Type> cross(const Vector3<Type>& a, const Vector3<Type>& b)
{ 
	return vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}


// ----------------------------------------------------------------------------
// cross3(a, b, c)

/// generalized cross product for 4D-Vectors: cross3(a, b, c) = dual(a ^ b ^ c)
template <typename Type>
Vector4<Type> cross3(const Vector4<Type>& a, const Vector4<Type>& b, const Vector4<Type>& c)
{ 
	Type bcxw = b.x * c.w - b.w * c.x;
	Type bcyw = b.y * c.w - b.w * c.y;
	Type bczw = b.z * c.w - b.w * c.z;
	Type bcxz = b.x * c.z - b.z * c.x;
	Type bcyx = b.y * c.x - b.x * c.y;
	Type bczy = b.z * c.y - b.y * c.z;

	return vector4(
		+ a.y * bczw - a.z * bcyw - a.w * bczy,
		+ a.z * bcxw - a.w * bcxz - a.x * bczw,
		- a.w * bcyx + a.x * bcyw - a.y * bcxw,
		+ a.x * bczy + a.y * bcxz + a.z * bcyx);
}


// ----------------------------------------------------------------------------
// angle(a, b)

template <typename Type>
Type angle(const Vector3<Type>& a, const Vector3<Type>& b)
{ 
	return atan2(length(cross(a, b)), dot(a, b));
}


// ----------------------------------------------------------------------------
// any(a)

// scalar
static inline bool any(bool a)
{
	return a;
}

// vector2
template <typename Type>
inline bool any(const Vector2<Type>& a)
{
	return (a.x | a.y) != 0;
}

// vector3
template <typename Type>
inline bool any(const Vector3<Type>& a)
{
	return (a.x | a.y | a.z) != 0;
}

// vector4
template <typename Type>
inline bool any(const Vector4<Type>& a)
{
	return (a.x | a.y | a.z | a.w) != 0;
}


// ----------------------------------------------------------------------------
// all(a)

// scalar
static inline bool all(bool a)
{
	return a;
}

// vector2
template <typename Type>
inline bool all(const Vector2<Type>& a)
{
	return (a.x & a.y) != 0;
}

// vector3
template <typename Type>
inline bool all(const Vector3<Type>& a)
{
	return (a.x & a.y & a.z) != 0;
}

// vector4
template <typename Type>
inline bool all(const Vector4<Type>& a)
{
	return (a.x & a.y & a.z & a.w) != 0;
}

/// @}

} // namespace digi

#endif

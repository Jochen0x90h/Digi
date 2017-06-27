/**
	vector functions are applied component-wise

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
	asin(a)
	acos(a)
	atan(a)
	atan2(y, x)
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
	
	vs(a) force execution in vertex shader
*/

// ----------------------------------------------------------------------------
/// abs(a)

// these are implemented by the virtual machine
__attribute__((const)) SCALAR abs(SCALAR a);
__attribute__((const)) VECTOR2 abs(VECTOR2 a);
__attribute__((const)) VECTOR3 abs(VECTOR3 a);
__attribute__((const)) VECTOR4 abs(VECTOR4 a);

/*
// scalar
inline SCALAR abs(SCALAR a)
{
	return fabs(a);
}

// vector2
inline VECTOR2 abs(VECTOR2 a)
{
	return vector2(abs(a.x), abs(a.y));
}

// vector3
inline VECTOR3 abs(VECTOR3 a)
{
	return vector3(abs(a.x), abs(a.y), abs(a.z));
}

// vector4
inline VECTOR4 abs(VECTOR4 a)
{
	return vector4(abs(a.x), abs(a.y), abs(a.z), abs(a.w));
}
*/

// ----------------------------------------------------------------------------
/// sign(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR sign(SCALAR a);
__attribute__((const)) VECTOR2 sign(VECTOR2 a);
__attribute__((const)) VECTOR3 sign(VECTOR3 a);
__attribute__((const)) VECTOR4 sign(VECTOR4 a);

#else

// scalar
inline SCALAR sign(SCALAR a)
{
	return (a > 0) ? ONE : (a < 0) ? -ONE : ZERO;
}

// vector2
inline VECTOR2 sign(VECTOR2 a)
{
	return vector2(sign(a.x), sign(a.y));
}

// vector3
inline VECTOR3 sign(VECTOR3 a)
{
	return vector3(sign(a.x), sign(a.y), sign(a.z));
}

// vector4
inline VECTOR4 sign(VECTOR4 a)
{
	return vector4(sign(a.x), sign(a.y), sign(a.z), sign(a.w));
}

#endif


// ----------------------------------------------------------------------------
/// sqrt(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR sqrt(SCALAR a);
__attribute__((const)) VECTOR2 sqrt(VECTOR2 a);
__attribute__((const)) VECTOR3 sqrt(VECTOR3 a);
__attribute__((const)) VECTOR4 sqrt(VECTOR4 a);

#else

// vector2
inline VECTOR2 sqrt(VECTOR2 a)
{
	return vector2(sqrt(a.x), sqrt(a.y));
}

// vector3
inline VECTOR3 sqrt(VECTOR3 a)
{
	return vector3(sqrt(a.x), sqrt(a.y), sqrt(a.z));
}

// vector4
inline VECTOR4 sqrt(VECTOR4 a)
{
	return vector4(sqrt(a.x), sqrt(a.y), sqrt(a.z), sqrt(a.w));
}

#endif


// ----------------------------------------------------------------------------
/// sqr(a)

// scalar
inline SCALAR sqr(SCALAR a)
{
	return a * a;
}


// ----------------------------------------------------------------------------
/// floor(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR floor(SCALAR a);
__attribute__((const)) VECTOR2 floor(VECTOR2 a);
__attribute__((const)) VECTOR3 floor(VECTOR3 a);
__attribute__((const)) VECTOR4 floor(VECTOR4 a);

#else

// vector2
inline VECTOR2 floor(VECTOR2 a)
{
	return vector2(floor(a.x), floor(a.y));
}

// vector3
inline VECTOR3 floor(VECTOR3 a)
{
	return vector3(floor(a.x), floor(a.y), floor(a.z));
}

// vector4
inline VECTOR4 floor(VECTOR4 a)
{
	return vector4(floor(a.x), floor(a.y), floor(a.z), floor(a.w));
}

#endif


// ----------------------------------------------------------------------------
/// ceil(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR ceil(SCALAR a);
__attribute__((const)) VECTOR2 ceil(VECTOR2 a);
__attribute__((const)) VECTOR3 ceil(VECTOR3 a);
__attribute__((const)) VECTOR4 ceil(VECTOR4 a);

#else

// vector2
inline VECTOR2 ceil(VECTOR2 a)
{
	return vector2(ceil(a.x), ceil(a.y));
}

// vector3
inline VECTOR3 ceil(VECTOR3 a)
{
	return vector3(ceil(a.x), ceil(a.y), ceil(a.z));
}

// vector4
inline VECTOR4 ceil(VECTOR4 a)
{
	return vector4(ceil(a.x), ceil(a.y), ceil(a.z), ceil(a.w));
}

#endif


// ----------------------------------------------------------------------------
/// round(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR round(SCALAR a);
__attribute__((const)) VECTOR2 round(VECTOR2 a);
__attribute__((const)) VECTOR3 round(VECTOR3 a);
__attribute__((const)) VECTOR4 round(VECTOR4 a);

#else

// vector2
inline VECTOR2 round(VECTOR2 a)
{
	return vector2(round(a.x), round(a.y));
}

// vector3
inline VECTOR3 round(VECTOR3 a)
{
	return vector3(round(a.x), round(a.y), round(a.z));
}

// vector4
inline VECTOR4 round(VECTOR4 a)
{
	return vector4(round(a.x), round(a.y), round(a.z), round(a.w));
}

#endif


// ----------------------------------------------------------------------------
/// trunc(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR trunc(SCALAR a);
__attribute__((const)) VECTOR2 trunc(VECTOR2 a);
__attribute__((const)) VECTOR3 trunc(VECTOR3 a);
__attribute__((const)) VECTOR4 trunc(VECTOR4 a);

#else

// vector2
inline VECTOR2 trunc(VECTOR2 a)
{
	return vector2(trunc(a.x), trunc(a.y));
}

// vector3
inline VECTOR3 trunc(VECTOR3 a)
{
	return vector3(trunc(a.x), trunc(a.y), trunc(a.z));
}

// vector4
inline VECTOR4 trunc(VECTOR4 a)
{
	return vector4(trunc(a.x), trunc(a.y), trunc(a.z), trunc(a.w));
}

#endif


// ----------------------------------------------------------------------------
/// frac(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR frac(SCALAR a);
__attribute__((const)) VECTOR2 frac(VECTOR2 a);
__attribute__((const)) VECTOR3 frac(VECTOR3 a);
__attribute__((const)) VECTOR4 frac(VECTOR4 a);

#else

// scalar
inline SCALAR frac(SCALAR a)
{
	return a - floor(a);
}

// vector2
inline VECTOR2 frac(VECTOR2 a)
{
	return vector2(frac(a.x), frac(a.y));
}

// vector3
inline VECTOR3 frac(VECTOR3 a)
{
	return vector3(frac(a.x), frac(a.y), frac(a.z));
}

// vector4
inline VECTOR4 frac(VECTOR4 a)
{
	return vector4(frac(a.x), frac(a.y), frac(a.z), frac(a.w));
}

#endif


// ----------------------------------------------------------------------------
// sin(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR sin(SCALAR a);
__attribute__((const)) VECTOR2 sin(VECTOR2 a);
__attribute__((const)) VECTOR3 sin(VECTOR3 a);
__attribute__((const)) VECTOR4 sin(VECTOR4 a);

#else

// vector2
inline VECTOR2 sin(VECTOR2 a)
{
	return vector2(sin(a.x), sin(a.y));
}

// vector3
inline VECTOR3 sin(VECTOR3 a)
{
	return vector3(sin(a.x), sin(a.y), sin(a.z));
}

// vector4
inline VECTOR4 sin(VECTOR4 a)
{
	return vector4(sin(a.x), sin(a.y), sin(a.z), sin(a.w));
}

#endif


// ----------------------------------------------------------------------------
// cos(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR cos(SCALAR a);
__attribute__((const)) VECTOR2 cos(VECTOR2 a);
__attribute__((const)) VECTOR3 cos(VECTOR3 a);
__attribute__((const)) VECTOR4 cos(VECTOR4 a);

#else

// vector2
inline VECTOR2 cos(VECTOR2 a)
{
	return vector2(cos(a.x), cos(a.y));
}

// vector3
inline VECTOR3 cos(VECTOR3 a)
{
	return vector3(cos(a.x), cos(a.y), cos(a.z));
}

// vector4
inline VECTOR4 cos(VECTOR4 a)
{
	return vector4(cos(a.x), cos(a.y), cos(a.z), cos(a.w));
}

#endif


// ----------------------------------------------------------------------------
// tan(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR tan(SCALAR a);
__attribute__((const)) VECTOR2 tan(VECTOR2 a);
__attribute__((const)) VECTOR3 tan(VECTOR3 a);
__attribute__((const)) VECTOR4 tan(VECTOR4 a);

#else

#endif


// ----------------------------------------------------------------------------
// asin(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR asin(SCALAR a);
__attribute__((const)) VECTOR2 asin(VECTOR2 a);
__attribute__((const)) VECTOR3 asin(VECTOR3 a);
__attribute__((const)) VECTOR4 asin(VECTOR4 a);

#else

#endif


// ----------------------------------------------------------------------------
// acos(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR acos(SCALAR a);
__attribute__((const)) VECTOR2 acos(VECTOR2 a);
__attribute__((const)) VECTOR3 acos(VECTOR3 a);
__attribute__((const)) VECTOR4 acos(VECTOR4 a);

#else

#endif


// ----------------------------------------------------------------------------
// atan(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR atan(SCALAR a);
__attribute__((const)) VECTOR2 atan(VECTOR2 a);
__attribute__((const)) VECTOR3 atan(VECTOR3 a);
__attribute__((const)) VECTOR4 atan(VECTOR4 a);

#else

#endif


// ----------------------------------------------------------------------------
// atan2(y, x)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR atan2(SCALAR y, SCALAR x);
__attribute__((const)) VECTOR2 atan2(VECTOR2 y, VECTOR2);
__attribute__((const)) VECTOR3 atan2(VECTOR3 y, VECTOR3);
__attribute__((const)) VECTOR4 atan2(VECTOR4 y, VECTOR4);

#else

#endif


// ----------------------------------------------------------------------------
// exp(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR exp(SCALAR a);
__attribute__((const)) VECTOR2 exp(VECTOR2 a);
__attribute__((const)) VECTOR3 exp(VECTOR3 a);
__attribute__((const)) VECTOR4 exp(VECTOR4 a);

#else

// vector2
inline VECTOR2 exp(VECTOR2 a)
{
	return vector2(exp(a.x), exp(a.y));
}

// vector3
inline VECTOR3 exp(VECTOR3 a)
{
	return vector3(exp(a.x), exp(a.y), exp(a.z));
}

// vector4
inline VECTOR4 exp(VECTOR4 a)
{
	return vector4(exp(a.x), exp(a.y), exp(a.z), exp(a.w));
}

#endif


// ----------------------------------------------------------------------------
// pow(a, b)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR pow(SCALAR a, SCALAR b);
__attribute__((const)) VECTOR2 pow(VECTOR2 a, VECTOR2 b);
__attribute__((const)) VECTOR3 pow(VECTOR3 a, VECTOR3 b);
__attribute__((const)) VECTOR4 pow(VECTOR4 a, VECTOR4 b);

#else

// vector2
inline VECTOR2 pow(VECTOR2 a, VECTOR2 b)
{
	return vector2(pow(a.x, b.x), pow(a.y, b.y));
}

// vector3
inline VECTOR3 pow(VECTOR3 a, VECTOR3 b)
{
	return vector3(pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z));
}

// vector4
inline VECTOR4 pow(VECTOR4 a, VECTOR4 b)
{
	return vector4(pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z), pow(a.w, b.w));
}

#endif


// ----------------------------------------------------------------------------
// log(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR log(SCALAR a);
__attribute__((const)) VECTOR2 log(VECTOR2 a);
__attribute__((const)) VECTOR3 log(VECTOR3 a);
__attribute__((const)) VECTOR4 log(VECTOR4 a);

#else

// vector2
inline VECTOR2 log(VECTOR2 a)
{
	return vector2(log(a.x), log(a.y));
}

// vector3
inline VECTOR3 log(VECTOR3 a)
{
	return vector3(log(a.x), log(a.y), log(a.z));
}

// vector4
inline VECTOR4 log(VECTOR4 a)
{
	return vector4(log(a.x), log(a.y), log(a.z), log(a.w));
}

#endif


// ----------------------------------------------------------------------------
// log10(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR log10(SCALAR a);
__attribute__((const)) VECTOR2 log10(VECTOR2 a);
__attribute__((const)) VECTOR3 log10(VECTOR3 a);
__attribute__((const)) VECTOR4 log10(VECTOR4 a);

#else

// vector2
inline VECTOR2 log10(VECTOR2 a)
{
	return vector2(log10(a.x), log10(a.y));
}

// vector3
inline VECTOR3 log10(VECTOR3 a)
{
	return vector3(log10(a.x), log10(a.y), log10(a.z));
}

// vector4
inline VECTOR4 log10(VECTOR4 a)
{
	return vector4(log10(a.x), log10(a.y), log10(a.z), log10(a.w));
}

#endif


// ----------------------------------------------------------------------------
// log2(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR log2(SCALAR a);
__attribute__((const)) VECTOR2 log2(VECTOR2 a);
__attribute__((const)) VECTOR3 log2(VECTOR3 a);
__attribute__((const)) VECTOR4 log2(VECTOR4 a);

#else

// scalar
inline SCALAR log2(SCALAR a)
{
	return log(a) * SCALAR(1.442695040888963);
}

// vector2
inline VECTOR2 log2(VECTOR2 a)
{
	return vector2(log2(a.x), log2(a.y));
}

// vector3
inline VECTOR3 log2(VECTOR3 a)
{
	return vector3(log2(a.x), log2(a.y), log2(a.z));
}

// vector4
inline VECTOR4 log2(VECTOR4 a)
{
	return vector4(log2(a.x), log2(a.y), log2(a.z), log2(a.w));
}

#endif


// ----------------------------------------------------------------------------
/// conj(a)


// ----------------------------------------------------------------------------
// min(a, b)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR min(SCALAR a, SCALAR b);
__attribute__((const)) VECTOR2 min(VECTOR2 a, VECTOR2 b);
__attribute__((const)) VECTOR2 min(VECTOR2 a, SCALAR b);
__attribute__((const)) VECTOR3 min(VECTOR3 a, VECTOR3 b);
__attribute__((const)) VECTOR3 min(VECTOR3 a, SCALAR b);
__attribute__((const)) VECTOR4 min(VECTOR4 a, VECTOR4 b);
__attribute__((const)) VECTOR4 min(VECTOR4 a, SCALAR b);

#else

// scalar
inline SCALAR min(SCALAR a, SCALAR b)
{
	return (a < b) ? a : b; 
}

// vector2
inline VECTOR2 min(VECTOR2 a, VECTOR2 b)
{
	return vector2(min(a.x, b.x), min(a.y, b.y));
}

inline VECTOR2 min(VECTOR2 a, SCALAR b)
{
	return vector2(min(a.x, b), min(a.y, b));
}

// vector3
inline VECTOR3 min(VECTOR3 a, VECTOR3 b)
{
	return vector3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

inline VECTOR3 min(VECTOR3 a, SCALAR b)
{
	return vector3(min(a.x, b), min(a.y, b), min(a.z, b));
}

// vector4
inline VECTOR4 min(VECTOR4 a, VECTOR4 b)
{
	return vector4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
}

inline VECTOR4 min(VECTOR4 a, SCALAR b)
{
	return vector4(min(a.x, b), min(a.y, b), min(a.z, b), min(a.w, b));
}

#endif


// ----------------------------------------------------------------------------
// max(a, b)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR max(SCALAR a, SCALAR b);
__attribute__((const)) VECTOR2 max(VECTOR2 a, VECTOR2 b);
__attribute__((const)) VECTOR2 max(VECTOR2 a, SCALAR b);
__attribute__((const)) VECTOR3 max(VECTOR3 a, VECTOR3 b);
__attribute__((const)) VECTOR3 max(VECTOR3 a, SCALAR b);
__attribute__((const)) VECTOR4 max(VECTOR4 a, VECTOR4 b);
__attribute__((const)) VECTOR4 max(VECTOR4 a, SCALAR b);

#else

// scalar
inline SCALAR max(SCALAR a, SCALAR b)
{
	return (a > b) ? a : b; 
}

// vector2
inline VECTOR2 max(VECTOR2 a, VECTOR2 b)
{
	return vector2(max(a.x, b.x), max(a.y, b.y));
}

inline VECTOR2 max(VECTOR2 a, SCALAR b)
{
	return vector2(max(a.x, b), max(a.y, b));
}

// vector3
inline VECTOR3 max(VECTOR3 a, VECTOR3 b)
{
	return vector3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

inline VECTOR3 max(VECTOR3 a, SCALAR b)
{
	return vector3(max(a.x, b), max(a.y, b), max(a.z, b));
}

// vector4
inline VECTOR4 max(VECTOR4 a, VECTOR4 b)
{
	return vector4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
}

inline VECTOR4 max(VECTOR4 a, SCALAR b)
{
	return vector4(max(a.x, b), max(a.y, b), max(a.z, b), max(a.w, b));
}

#endif


// ----------------------------------------------------------------------------
/// min(a)

// vector2
inline SCALAR min(VECTOR2 a)
{
	return min(a.x, a.y);
}

// vector3
inline SCALAR min(VECTOR3 a)
{
	return min(min(a.x, a.y), a.z);
}

// vector4
inline SCALAR min(VECTOR4 a)
{
	return min(min(a.x, a.y), min(a.z, a.w));
}


// ----------------------------------------------------------------------------
/// max(a)

// vector2
inline SCALAR max(VECTOR2 a)
{
	return max(a.x, a.y);
}

// vector3
inline SCALAR max(VECTOR3 a)
{
	return max(max(a.x, a.y), a.z);
}

// vector4
inline SCALAR max(VECTOR4 a)
{
	return max(max(a.x, a.y), max(a.z, a.w));
}


// ----------------------------------------------------------------------------
/// sum(a)

// vector2
inline SCALAR sum(VECTOR2 a)
{
	return a.x + a.y;
}

// vector3
inline SCALAR sum(VECTOR3 a)
{
	return a.x + a.y + a.z;
}

// vector4
inline SCALAR sum(VECTOR4 a)
{
	return a.x + a.y + a.z + a.w;
}


// ----------------------------------------------------------------------------
/// prod(a)

// vector2
inline SCALAR prod(VECTOR2 a)
{
	return a.x * a.y;
}

// vector3
inline SCALAR prod(VECTOR3 a)
{
	return a.x * a.y * a.z;
}

// vector4
inline SCALAR prod(VECTOR4 a)
{
	return a.x * a.y * a.z * a.w;
}


// ----------------------------------------------------------------------------
// mod(a, b)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR mod(SCALAR a, SCALAR b);
__attribute__((const)) VECTOR2 mod(VECTOR2 a, VECTOR2 b);
__attribute__((const)) VECTOR2 mod(VECTOR2 a, SCALAR b);
__attribute__((const)) VECTOR3 mod(VECTOR3 a, VECTOR3 b);
__attribute__((const)) VECTOR3 mod(VECTOR3 a, SCALAR b);
__attribute__((const)) VECTOR4 mod(VECTOR4 a, VECTOR4 b);
__attribute__((const)) VECTOR4 mod(VECTOR4 a, SCALAR b);

#else

// scalar
inline SCALAR mod(SCALAR a, SCALAR b)
{
	return fmod(a, b);
}

// vector2
inline VECTOR2 mod(VECTOR2 a, VECTOR2 b)
{
	return vector2(mod(a.x, b.x), mod(a.y, b.y));
}

inline VECTOR2 mod(VECTOR2 a, SCALAR b)
{
	return vector2(mod(a.x, b), mod(a.y, b));
}

// vector3
inline VECTOR3 mod(VECTOR3 a, VECTOR3 b)
{
	return vector3(mod(a.x, b.x), mod(a.y, b.y), mod(a.z, b.z));
}

inline VECTOR3 mod(VECTOR3 a, SCALAR b)
{
	return vector3(mod(a.x, b), mod(a.y, b), mod(a.z, b));
}

// vector4
inline VECTOR4 mod(VECTOR4 a, VECTOR4 b)
{
	return vector4(mod(a.x, b.x), mod(a.y, b.y), mod(a.z, b.z), mod(a.w, b.w));
}

inline VECTOR4 mod(VECTOR4 a, SCALAR b)
{
	return vector4(mod(a.x, b), mod(a.y, b), mod(a.z, b), mod(a.w, b));
}

#endif


// ----------------------------------------------------------------------------
// dot(a, b)

// vector2
inline SCALAR dot(VECTOR2 a, VECTOR2 b)
{
	return sum(a * b);
}

// vector3
inline SCALAR dot(VECTOR3 a, VECTOR3 b)
{
	return sum(a * b);
}

// vector4
inline SCALAR dot(VECTOR4 a, VECTOR4 b)
{
	return sum(a * b);
}


// ----------------------------------------------------------------------------
// mac(a, b)

// vector2
inline SCALAR mac(VECTOR2 a, VECTOR2 b)
{
	return sum(a * b);
}

// vector3
inline SCALAR mac(VECTOR3 a, VECTOR3 b)
{
	return sum(a * b);
}

// vector4
inline SCALAR mac(VECTOR4 a, VECTOR4 b)
{
	return sum(a * b);
}


// ----------------------------------------------------------------------------
// select(a, b, c)

// scalar
inline SCALAR select(SCALAR a, SCALAR b, bool c)
{
	return c ? b : a;
}

// vector2
inline VECTOR2 select(VECTOR2 a, VECTOR2 b, int2 c)
{
	return vector2(
		c.x ? b.x : a.x,
		c.y ? b.y : a.y);
}

// vector3
inline VECTOR3 select(VECTOR3 a, VECTOR3 b, int3 c)
{
	return vector3(
		c.x ? b.x : a.x,
		c.y ? b.y : a.y,
		c.z ? b.z : a.z);
}

// vector4
inline VECTOR4 select(VECTOR4 a, VECTOR4 b, int4 c)
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
inline SCALAR lengthSquared(VECTOR2 a)
{
	return mac(a, a);
}

// vector3
inline SCALAR lengthSquared(VECTOR3 a)
{
	return mac(a, a);
}

// vector4
inline SCALAR lengthSquared(VECTOR4 a)
{
	return mac(a, a);
}


// ----------------------------------------------------------------------------
// length(a)

// vector2
inline SCALAR length(VECTOR2 a)
{
	return sqrt(mac(a, a));
}

// vector3
inline SCALAR length(VECTOR3 a)
{
	return sqrt(mac(a, a));
}

// vector4
inline SCALAR length(VECTOR4 a)
{
	return sqrt(mac(a, a));
}


// ----------------------------------------------------------------------------
// normalize(a)

// vector2
inline VECTOR2 normalize(VECTOR2 a)
{
	return a / length(a);
}

// vector3
inline VECTOR3 normalize(VECTOR3 a)
{
	return a / length(a);
}

// vector4
inline VECTOR4 normalize(VECTOR4 a)
{
	return a / length(a);
}


// ----------------------------------------------------------------------------
// distance(a, b)

// vector2
inline SCALAR distance(VECTOR2 a, VECTOR2 b)
{
	return length(b - a);
}

// vector3
inline SCALAR distance(VECTOR3 a, VECTOR3 b)
{
	return length(b - a);
}

// vector4
inline SCALAR distance(VECTOR4 a, VECTOR4 b)
{
	return length(b - a);
}


// ----------------------------------------------------------------------------
// reflect(a, n)

// vector2
inline VECTOR2 reflect(VECTOR2 a, VECTOR2 n)
{
	return a - (2.0f * dot(a, n)) * n;
}

// vector3
inline VECTOR3 reflect(VECTOR3 a, VECTOR3 n)
{
	return a - (2.0f * dot(a, n)) * n;
}

// vector4
inline VECTOR4 reflect(VECTOR4 a, VECTOR4 n)
{
	return a - (2.0f * dot(a, n)) * n;
}


// ----------------------------------------------------------------------------
// perp(a)

inline VECTOR2 perp(VECTOR2 a)
{
	return vector2(a.y, -a.x);
}


// ----------------------------------------------------------------------------
// cross(a, b)

inline VECTOR3 cross(VECTOR3 a, VECTOR3 b)
{ 
	return a.yzx * b.zxy - a.zxy * b.yzx;
}


// ----------------------------------------------------------------------------
// cross3(a, b, c)

inline VECTOR4 cross3(VECTOR4 a, VECTOR4 b, VECTOR4 c)
{ 
	SCALAR bcxw = b.x * c.w - b.w * c.x;
	SCALAR bcyw = b.y * c.w - b.w * c.y;
	SCALAR bczw = b.z * c.w - b.w * c.z;
	SCALAR bcxz = b.x * c.z - b.z * c.x;
	SCALAR bcyx = b.y * c.x - b.x * c.y;
	SCALAR bczy = b.z * c.y - b.y * c.z;

	return vector4(
		+ a.y * bczw - a.z * bcyw - a.w * bczy,
		+ a.z * bcxw - a.w * bcxz - a.x * bczw,
		- a.w * bcyx + a.x * bcyw - a.y * bcxw,
		+ a.x * bczy + a.y * bcxz + a.z * bcyx);
}


// ----------------------------------------------------------------------------
// angle(a, b)

inline SCALAR angle(VECTOR3 a, VECTOR3 b)
{ 
	return atan2(length(cross(a, b)), dot(a, b));
}


// ----------------------------------------------------------------------------
/// vs(a)

#ifdef __INTERMEDIATE__

__attribute__((const)) SCALAR vs(SCALAR a);
__attribute__((const)) VECTOR2 vs(VECTOR2 a);
__attribute__((const)) VECTOR3 vs(VECTOR3 a);
__attribute__((const)) VECTOR4 vs(VECTOR4 a);

#endif

#ifndef digi_VectorMath_ScalarFunctions_h
#define digi_VectorMath_ScalarFunctions_h

#undef min
#undef max

#include <algorithm>


namespace digi {

/// @addtogroup VectorMath
/// @{


/**
	scalar functions
	
	abs(a) absolute value
	sign(a) sign
	sqrt(a) square root
	sqr(a) square
	floor(a)
	ceil(a)
	round(a) round to nearest int, away from zero for .5
	trunc(a) truncate fractional part
	frac(a) fractional part
	sin(a)
	cos(a)
	exp(a)
	log(a) logarithm base e
	log10(a) logarithm base 10
	log2(a) logarithm base 2
	conj(a) complex conjugate
	min(a)
	max(a)
	sum(a)
	prod(a)
	min(a, b)
	max(a, b)
	mod(a, b)
	dot(a, b) dot product
	
	any(a)
	all(a)
	select(a, b, c) c ? b : a
	
	inv(a) inverse (1/a)
*/

// ----------------------------------------------------------------------------
/// abs(a)

using std::abs;


// ----------------------------------------------------------------------------
/// sign(a)

static inline float sign(float a)
{
	return (a > 0.0f) ? 1.0f : (a < 0.0f) ? -1.0f : 0.0f;
}

static inline double sign(double a)
{
	return (a > 0.0) ? 1.0f : (a < 0.0) ? -1.0f : 0.0;
}


// ----------------------------------------------------------------------------
/// sqrt(a)

using std::sqrt;


// ----------------------------------------------------------------------------
/// sqr(a)

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

using std::floor;


// ----------------------------------------------------------------------------
/// ceil(a)

using std::ceil;


// ----------------------------------------------------------------------------
/// round(a)

#ifdef HAVE_C99_MATH
	static inline float round(float a) {return ::roundf(a);}
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


// ----------------------------------------------------------------------------
/// trunc(a)

#ifdef HAVE_C99_MATH
	static inline float trunc(float a) {return ::truncf(a);}
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

static inline float frac(float a)
{
	return a - floor(a);
}

static inline double frac(double a)
{
	return a - floor(a);
}


// ----------------------------------------------------------------------------
// sin(a)

using std::sin;


// ----------------------------------------------------------------------------
// cos(a)

using std::cos;


// ----------------------------------------------------------------------------
// exp(a)

using std::exp;


// ----------------------------------------------------------------------------
// log(a)

using std::log;


// ----------------------------------------------------------------------------
// log10(a)

using std::log10;


// ----------------------------------------------------------------------------
// log2(a)

static inline float log2(float a)
{
	return log(a) * 1.442695040888963f;
}

static inline double log2(double a)
{
	return log(a) * 1.442695040888963;
}


// ----------------------------------------------------------------------------
/// conj(a)

static inline float conj(float a)
{
	return a;
}

static inline double conj(double a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// min(a)

static inline float min(float a)
{
	return a;
}

static inline double min(double a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// max(a)

static inline float max(float a)
{
	return a;
}

static inline double max(double a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// sum(a)

static inline float sum(float a)
{
	return a;
}

static inline double sum(double a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// prod(a)

static inline float prod(float a)
{
	return a;
}

static inline double prod(double a)
{
	return a;
}


// ----------------------------------------------------------------------------
// min(a, b)

using std::min;


// ----------------------------------------------------------------------------
// max(a, b)

using std::max;


// ----------------------------------------------------------------------------
// mod(a, b)

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


// ----------------------------------------------------------------------------
// dot(a, b)

static inline float dot(float a, float b)
{
	return a * b;
}

static inline double dot(double a, double b)
{
	return a * b;
}


// ----------------------------------------------------------------------------
// any(a)

static inline bool any(bool a)
{
	return a;
}


// ----------------------------------------------------------------------------
// all(a)

static inline bool all(bool a)
{
	return a;
}


// ----------------------------------------------------------------------------
// select(a, b, c)

static inline float select(float a, float b, bool c)
{
	return c ? b : a;
}

static inline double select(double a, double b, bool c)
{
	return c ? b : a;
}


// ----------------------------------------------------------------------------
// inv(a)

static inline float inv(float a)
{
	return 1.0f / a;
}

static inline double inv(double a)
{
	return 1.0 / a;
}


/// @}

} // namespace digi

#endif

#ifndef digi_Math_MathBase_h
#define digi_Math_MathBase_h

#ifdef HAVE_C99_MATH
	#include <math.h>
#endif

#include <cmath>

#ifdef _WIN32_WCE
namespace std
{
	inline float abs(float a) {return float(std::fabsf(a));}
	inline float atan2(float a, float b) {return float(std::atan2(double(a), double(b)));}
	inline float ceil(float a) {return float(std::ceil(double(a)));}
	inline float cos(float a) {return float(std::cos(double(a)));}
	inline float exp(float a) {return float(std::exp(double(a)));}
	inline float floor(float a) {return float(std::floor(double(a)));}
	inline float fmod(float a, float b) {return float(std::fmod(double(a), double(b)));}
	inline float log(float a) {return float(std::log(double(a)));}
	inline float pow(float a, float b) {return float(std::pow(double(a), double(b)));}
	inline float sin(float a) {return float(std::sin(double(a)));}
	inline float sqrt(float a) {return float(std::sqrt(double(a)));}
	inline float tan(float a) {return float(std::tan(double(a)));}

	inline long double pow(long double a, long double b) {return std::pow(double(a), double(b));}
}
#endif


#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Math
/// @{


// constants
static const float pif = 3.1415926535897932384626433832795f;
static const double pi = 3.1415926535897932384626433832795;



// rint for float/double input and int result
// http://publib.boulder.ibm.com/infocenter/zos/v1r10/index.jsp?topic=/com.ibm.zos.r10.bpxbd00/lrint.htm
template <bool b>
struct RoundHelper
{
	static int rint(float x)
	{
		#ifdef HAVE_C99_MATH 
			return int(::lrintf(x));
		#else
			float x1 = x + 0.5f;
			float x2 = std::floor(x1);
			int i = (int)x2;
			if (x1 == x2) i &= ~1;
			return i;
		#endif
	}
	
	static int rint(double x)
	{
		#ifdef HAVE_C99_MATH 
			return int(::lrint(x));
		#else
			double x1 = x + 0.5;
			double x2 = std::floor(x1);
			int i = (int)x2;
			if (x1 == x2) i &= ~1;
			return i;
		#endif
	}
};

// rint for float/double input and long long result
template <>
struct RoundHelper<true>
{
	static long long rint(float x)
	{
		#ifdef HAVE_C99_MATH 
			return ::llrintf(x);
		#else
			float x1 = x + 0.5f;
			float x2 = std::floor(x1);
			long long i = (long long)x2;
			if (x1 == x2) i &= ~1LL;
			return i;
		#endif
	}
	
	static long long rint(double x)
	{
		#ifdef HAVE_C99_MATH 
			return ::llrint(x);
		#else
			double x1 = x + 0.5;
			double x2 = std::floor(x1);
			long long i = (long long)x2;
			if (x1 == x2) i &= ~1LL;
			return i;
		#endif
	}
};

template <typename Type1, typename Type2>
struct IsEqualType
{
	enum {RESULT = 0};
};

template <typename Type>
struct IsEqualType<Type, Type>
{
	enum {RESULT = 1};
};

template <typename Type1, typename Type2>
Type1 rint(Type2 a)
{
	// use long long version of rint (llrint) if Type1 is larger than int or is unsigned int
	return Type1(RoundHelper<
		(sizeof(Type1) > sizeof(int))
		|| IsEqualType<Type1, unsigned int>::RESULT
		|| IsEqualType<Type1, unsigned long>::RESULT
		>::rint(a));
}



// traits that define some properties of scalars/vectors/quaternions
template <typename T>
struct MathTraits
{
	// indicates if it is a compound type (Vector2, Matrix4 etc.)
	enum
	{
		COMPOUND = 0
	};

	// the type itself
	typedef T Type;
	
	// the base scalar number type (float, double etc.)
	typedef T BaseType;
	
	// the element type. for vectors the type of the components (scalar, complex etc.)
	typedef T ElementType;
	
	// the real part of the type. for vector it is a vector of RealType of the ElementType. e.g. abs(Type) returns RealType
	typedef T RealType;
};



#define DIGI_BASE(T) typename MathTraits<T>::BaseType
#define DIGI_ELEMENT(T) typename MathTraits<T>::ElementType
#define DIGI_REAL(T) typename MathTraits<T>::RealType

#define DIGI_MATRIX2X2(T) Matrix2<Vector2<T> >
#define DIGI_MATRIX3X2(T) Matrix2<Vector3<T> >
#define DIGI_MATRIX4X2(T) Matrix2<Vector4<T> >

#define DIGI_MATRIX2X3(T) Matrix3<Vector2<T> >
#define DIGI_MATRIX3X3(T) Matrix3<Vector3<T> >
#define DIGI_MATRIX4X3(T) Matrix3<Vector4<T> >

#define DIGI_MATRIX2X4(T) Matrix4<Vector2<T> >
#define DIGI_MATRIX3X4(T) Matrix4<Vector3<T> >
#define DIGI_MATRIX4X4(T) Matrix4<Vector4<T> >


/// @}

} // namespace digi

#endif

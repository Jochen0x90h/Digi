
typedef Vector2<bool> bool2;
typedef Vector2<int8_t> byte2;
typedef Vector2<uint8_t> ubyte2;
typedef Vector2<int16_t> short2;
typedef Vector2<uint16_t> ushort2;
typedef Vector2<int32_t> int2;
typedef Vector2<uint32_t> uint2;
typedef Vector2<int64_t> long2;
typedef Vector2<uint64_t> ulong2;
typedef Vector2<half> half2;
typedef Vector2<float> float2;
typedef Vector2<double> double2;

typedef Vector3<bool> bool3;
typedef Vector3<int8_t> byte3;
typedef Vector3<uint8_t> ubyte3;
typedef Vector3<int16_t> short3;
typedef Vector3<uint16_t> ushort3;
typedef Vector3<int32_t> int3;
typedef Vector3<uint32_t> uint3;
typedef Vector3<int64_t> long3;
typedef Vector3<uint64_t> ulong3;
typedef Vector3<half> half3;
typedef Vector3<float> float3;
typedef Vector3<double> double3;

typedef Vector4<bool> bool4;
typedef Vector4<int8_t> byte4;
typedef Vector4<uint8_t> ubyte4;
typedef Vector4<int16_t> short4;
typedef Vector4<uint16_t> ushort4;
typedef Vector4<int32_t> int4;
typedef Vector4<uint32_t> uint4;
typedef Vector4<int64_t> long4;
typedef Vector4<uint64_t> ulong4;
typedef Vector4<half> half4;
typedef Vector4<float> float4;
typedef Vector4<double> double4;

typedef bool2 packed_bool2;
typedef byte2 packed_byte2;
typedef ubyte2 packed_ubyte2;
typedef short2 packed_short2;
typedef ushort2 packed_ushort2;
typedef int2 packed_int2;
typedef uint2 packed_uint2;
typedef long2 packed_long2;
typedef ulong2 packed_ulong2;
typedef half2 packed_half2;
typedef float2 packed_float2;
typedef double2 packed_double2;

typedef bool3 packed_bool3;
typedef byte3 packed_byte3;
typedef ubyte3 packed_ubyte3;
typedef short3 packed_short3;
typedef ushort3 packed_ushort3;
typedef int3 packed_int3;
typedef uint3 packed_uint3;
typedef long3 packed_long3;
typedef ulong3 packed_ulong3;
typedef half3 packed_half3;
typedef float3 packed_float3;
typedef double3 packed_double3;

typedef bool4 packed_bool4;
typedef byte4 packed_byte4;
typedef ubyte4 packed_ubyte4;
typedef short4 packed_short4;
typedef ushort4 packed_ushort4;
typedef int4 packed_int4;
typedef uint4 packed_uint4;
typedef long4 packed_long4;
typedef ulong4 packed_ulong4;
typedef half4 packed_half4;
typedef float4 packed_float4;
typedef double4 packed_double4;

template <typename T0, typename T1> bool2 make_bool2(T0 x, T1 y) {return vector2(bool(x), bool(y));}
template <typename T0, typename T1> byte2 make_byte2(T0 x, T1 y) {return vector2(int8_t(x), int8_t(y));}
template <typename T0, typename T1> ubyte2 make_ubyte2(T0 x, T1 y) {return vector2(uint8_t(x), uint8_t(y));}
template <typename T0, typename T1> short2 make_short2(T0 x, T1 y) {return vector2(int16_t(x), int16_t(y));}
template <typename T0, typename T1> ushort2 make_ushort2(T0 x, T1 y) {return vector2(uint16_t(x), uint16_t(y));}
template <typename T0, typename T1> int2 make_int2(T0 x, T1 y) {return vector2(int32_t(x), int32_t(y));}
template <typename T0, typename T1> uint2 make_uint2(T0 x, T1 y) {return vector2(uint32_t(x), uint32_t(y));}
template <typename T0, typename T1> long2 make_long2(T0 x, T1 y) {return vector2(int64_t(x), int64_t(y));}
template <typename T0, typename T1> ulong2 make_ulong2(T0 x, T1 y) {return vector2(uint64_t(x), uint64_t(y));}
template <typename T0, typename T1> half2 make_half2(T0 x, T1 y) {return vector2(half(x), half(y));}
template <typename T0, typename T1> float2 make_float2(T0 x, T1 y) {return vector2(float(x), float(y));}
template <typename T0, typename T1> double2 make_double2(T0 x, T1 y) {return vector2(double(x), double(y));}

template <typename T0, typename T1, typename T2> bool3 make_bool3(T0 x, T1 y, T2 z) {return vector3(bool(x), bool(y), bool(z));}
template <typename T0, typename T1, typename T2> byte3 make_byte3(T0 x, T1 y, T2 z) {return vector3(int8_t(x), int8_t(y), int8_t(z));}
template <typename T0, typename T1, typename T2> ubyte3 make_ubyte3(T0 x, T1 y, T2 z) {return vector3(uint8_t(x), uint8_t(y), uint8_t(z));}
template <typename T0, typename T1, typename T2> short3 make_short3(T0 x, T1 y, T2 z) {return vector3(int16_t(x), int16_t(y), int16_t(z));}
template <typename T0, typename T1, typename T2> ushort3 make_ushort3(T0 x, T1 y, T2 z) {return vector3(uint16_t(x), uint16_t(y), uint16_t(z));}
template <typename T0, typename T1, typename T2> int3 make_int3(T0 x, T1 y, T2 z) {return vector3(int32_t(x), int32_t(y), int32_t(z));}
template <typename T0, typename T1, typename T2> uint3 make_uint3(T0 x, T1 y, T2 z) {return vector3(uint32_t(x), uint32_t(y), uint32_t(z));}
template <typename T0, typename T1, typename T2> long3 make_long3(T0 x, T1 y, T2 z) {return vector3(int64_t(x), int64_t(y), int64_t(z));}
template <typename T0, typename T1, typename T2> ulong3 make_ulong3(T0 x, T1 y, T2 z) {return vector3(uint64_t(x), uint64_t(y), uint64_t(z));}
template <typename T0, typename T1, typename T2> half3 make_half3(T0 x, T1 y, T2 z) {return vector3(half(x), half(y), half(z));}
template <typename T0, typename T1, typename T2> float3 make_float3(T0 x, T1 y, T2 z) {return vector3(float(x), float(y), float(z));}
template <typename T0, typename T1, typename T2> double3 make_double3(T0 x, T1 y, T2 z) {return vector3(double(x), double(y), double(z));}

template <typename T0, typename T1, typename T2, typename T3> bool4 make_bool4(T0 x, T1 y, T2 z, T3 w) {return vector4(bool(x), bool(y), bool(z), bool(w));}
template <typename T0, typename T1, typename T2, typename T3> byte4 make_byte4(T0 x, T1 y, T2 z, T3 w) {return vector4(int8_t(x), int8_t(y), int8_t(z), int8_t(w));}
template <typename T0, typename T1, typename T2, typename T3> ubyte4 make_ubyte4(T0 x, T1 y, T2 z, T3 w) {return vector4(uint8_t(x), uint8_t(y), uint8_t(z), uint8_t(w));}
template <typename T0, typename T1, typename T2, typename T3> short4 make_short4(T0 x, T1 y, T2 z, T3 w) {return vector4(int16_t(x), int16_t(y), int16_t(z), int16_t(w));}
template <typename T0, typename T1, typename T2, typename T3> ushort4 make_ushort4(T0 x, T1 y, T2 z, T3 w) {return vector4(uint16_t(x), uint16_t(y), uint16_t(z), uint16_t(w));}
template <typename T0, typename T1, typename T2, typename T3> int4 make_int4(T0 x, T1 y, T2 z, T3 w) {return vector4(int32_t(x), int32_t(y), int32_t(z), int32_t(w));}
template <typename T0, typename T1, typename T2, typename T3> uint4 make_uint4(T0 x, T1 y, T2 z, T3 w) {return vector4(uint32_t(x), uint32_t(y), uint32_t(z), uint32_t(w));}
template <typename T0, typename T1, typename T2, typename T3> long4 make_long4(T0 x, T1 y, T2 z, T3 w) {return vector4(int64_t(x), int64_t(y), int64_t(z), int64_t(w));}
template <typename T0, typename T1, typename T2, typename T3> ulong4 make_ulong4(T0 x, T1 y, T2 z, T3 w) {return vector4(uint64_t(x), uint64_t(y), uint64_t(z), uint64_t(w));}
template <typename T0, typename T1, typename T2, typename T3> half4 make_half4(T0 x, T1 y, T2 z, T3 w) {return vector4(half(x), half(y), half(z), half(w));}
template <typename T0, typename T1, typename T2, typename T3> float4 make_float4(T0 x, T1 y, T2 z, T3 w) {return vector4(float(x), float(y), float(z), float(w));}
template <typename T0, typename T1, typename T2, typename T3> double4 make_double4(T0 x, T1 y, T2 z, T3 w) {return vector4(double(x), double(y), double(z), double(w));}


inline int8_t convert_byte(bool v) {return int8_t(v);}
inline int8_t convert_byte(int8_t v) {return v;}
inline int8_t convert_byte(uint8_t v) {return int8_t(v);}
inline int8_t convert_byte(int16_t v) {return int8_t(v);}
inline int8_t convert_byte(uint16_t v) {return int8_t(v);}
inline int8_t convert_byte(int32_t v) {return int8_t(v);}
inline int8_t convert_byte(uint32_t v) {return int8_t(v);}
inline int8_t convert_byte(int64_t v) {return int8_t(v);}
inline int8_t convert_byte(uint64_t v) {return int8_t(v);}
inline int8_t convert_byte(half v) {return int8_t(v);}
inline int8_t convert_byte(float v) {return int8_t(v);}
inline int8_t convert_byte(double v) {return int8_t(v);}

inline uint8_t convert_ubyte(bool v) {return uint8_t(v);}
inline uint8_t convert_ubyte(int8_t v) {return uint8_t(v);}
inline uint8_t convert_ubyte(uint8_t v) {return v;}
inline uint8_t convert_ubyte(int16_t v) {return uint8_t(v);}
inline uint8_t convert_ubyte(uint16_t v) {return uint8_t(v);}
inline uint8_t convert_ubyte(int32_t v) {return uint8_t(v);}
inline uint8_t convert_ubyte(uint32_t v) {return uint8_t(v);}
inline uint8_t convert_ubyte(int64_t v) {return uint8_t(v);}
inline uint8_t convert_ubyte(uint64_t v) {return uint8_t(v);}
inline uint8_t convert_ubyte(half v) {return uint8_t(v);}
inline uint8_t convert_ubyte(float v) {return uint8_t(v);}
inline uint8_t convert_ubyte(double v) {return uint8_t(v);}

inline int16_t convert_short(bool v) {return int16_t(v);}
inline int16_t convert_short(int8_t v) {return int16_t(v);}
inline int16_t convert_short(uint8_t v) {return int16_t(v);}
inline int16_t convert_short(int16_t v) {return v;}
inline int16_t convert_short(uint16_t v) {return int16_t(v);}
inline int16_t convert_short(int32_t v) {return int16_t(v);}
inline int16_t convert_short(uint32_t v) {return int16_t(v);}
inline int16_t convert_short(int64_t v) {return int16_t(v);}
inline int16_t convert_short(uint64_t v) {return int16_t(v);}
inline int16_t convert_short(half v) {return int16_t(v);}
inline int16_t convert_short(float v) {return int16_t(v);}
inline int16_t convert_short(double v) {return int16_t(v);}

inline uint16_t convert_ushort(bool v) {return uint16_t(v);}
inline uint16_t convert_ushort(int8_t v) {return uint16_t(v);}
inline uint16_t convert_ushort(uint8_t v) {return uint16_t(v);}
inline uint16_t convert_ushort(int16_t v) {return uint16_t(v);}
inline uint16_t convert_ushort(uint16_t v) {return v;}
inline uint16_t convert_ushort(int32_t v) {return uint16_t(v);}
inline uint16_t convert_ushort(uint32_t v) {return uint16_t(v);}
inline uint16_t convert_ushort(int64_t v) {return uint16_t(v);}
inline uint16_t convert_ushort(uint64_t v) {return uint16_t(v);}
inline uint16_t convert_ushort(half v) {return uint16_t(v);}
inline uint16_t convert_ushort(float v) {return uint16_t(v);}
inline uint16_t convert_ushort(double v) {return uint16_t(v);}

inline int32_t convert_int(bool v) {return int32_t(v);}
inline int32_t convert_int(int8_t v) {return int32_t(v);}
inline int32_t convert_int(uint8_t v) {return int32_t(v);}
inline int32_t convert_int(int16_t v) {return int32_t(v);}
inline int32_t convert_int(uint16_t v) {return int32_t(v);}
inline int32_t convert_int(int32_t v) {return v;}
inline int32_t convert_int(uint32_t v) {return int32_t(v);}
inline int32_t convert_int(int64_t v) {return int32_t(v);}
inline int32_t convert_int(uint64_t v) {return int32_t(v);}
inline int32_t convert_int(half v) {return int32_t(v);}
inline int32_t convert_int(float v) {return int32_t(v);}
inline int32_t convert_int(double v) {return int32_t(v);}

inline uint32_t convert_uint(bool v) {return uint32_t(v);}
inline uint32_t convert_uint(int8_t v) {return uint32_t(v);}
inline uint32_t convert_uint(uint8_t v) {return uint32_t(v);}
inline uint32_t convert_uint(int16_t v) {return uint32_t(v);}
inline uint32_t convert_uint(uint16_t v) {return uint32_t(v);}
inline uint32_t convert_uint(int32_t v) {return uint32_t(v);}
inline uint32_t convert_uint(uint32_t v) {return v;}
inline uint32_t convert_uint(int64_t v) {return uint32_t(v);}
inline uint32_t convert_uint(uint64_t v) {return uint32_t(v);}
inline uint32_t convert_uint(half v) {return uint32_t(v);}
inline uint32_t convert_uint(float v) {return uint32_t(v);}
inline uint32_t convert_uint(double v) {return uint32_t(v);}

inline int64_t convert_long(bool v) {return int64_t(v);}
inline int64_t convert_long(int8_t v) {return int64_t(v);}
inline int64_t convert_long(uint8_t v) {return int64_t(v);}
inline int64_t convert_long(int16_t v) {return int64_t(v);}
inline int64_t convert_long(uint16_t v) {return int64_t(v);}
inline int64_t convert_long(int32_t v) {return int64_t(v);}
inline int64_t convert_long(uint32_t v) {return int64_t(v);}
inline int64_t convert_long(int64_t v) {return v;}
inline int64_t convert_long(uint64_t v) {return int64_t(v);}
inline int64_t convert_long(half v) {return int64_t(v);}
inline int64_t convert_long(float v) {return int64_t(v);}
inline int64_t convert_long(double v) {return int64_t(v);}

inline uint64_t convert_ulong(bool v) {return uint64_t(v);}
inline uint64_t convert_ulong(int8_t v) {return uint64_t(v);}
inline uint64_t convert_ulong(uint8_t v) {return uint64_t(v);}
inline uint64_t convert_ulong(int16_t v) {return uint64_t(v);}
inline uint64_t convert_ulong(uint16_t v) {return uint64_t(v);}
inline uint64_t convert_ulong(int32_t v) {return uint64_t(v);}
inline uint64_t convert_ulong(uint32_t v) {return uint64_t(v);}
inline uint64_t convert_ulong(int64_t v) {return uint64_t(v);}
inline uint64_t convert_ulong(uint64_t v) {return v;}
inline uint64_t convert_ulong(half v) {return uint64_t(v);}
inline uint64_t convert_ulong(float v) {return uint64_t(v);}
inline uint64_t convert_ulong(double v) {return uint64_t(v);}

inline half convert_half(bool v) {return half(v);}
inline half convert_half(int8_t v) {return half(v);}
inline half convert_half(uint8_t v) {return half(v);}
inline half convert_half(int16_t v) {return half(v);}
inline half convert_half(uint16_t v) {return half(v);}
inline half convert_half(int32_t v) {return half(v);}
inline half convert_half(uint32_t v) {return half(v);}
inline half convert_half(int64_t v) {return half(v);}
inline half convert_half(uint64_t v) {return half(v);}
inline half convert_half(half v) {return v;}
inline half convert_half(float v) {return half(v);}
inline half convert_half(double v) {return half(v);}

inline float convert_float(bool v) {return float(v);}
inline float convert_float(int8_t v) {return float(v);}
inline float convert_float(uint8_t v) {return float(v);}
inline float convert_float(int16_t v) {return float(v);}
inline float convert_float(uint16_t v) {return float(v);}
inline float convert_float(int32_t v) {return float(v);}
inline float convert_float(uint32_t v) {return float(v);}
inline float convert_float(int64_t v) {return float(v);}
inline float convert_float(uint64_t v) {return float(v);}
inline float convert_float(half v) {return float(v);}
inline float convert_float(float v) {return v;}
inline float convert_float(double v) {return float(v);}

inline double convert_double(bool v) {return double(v);}
inline double convert_double(int8_t v) {return double(v);}
inline double convert_double(uint8_t v) {return double(v);}
inline double convert_double(int16_t v) {return double(v);}
inline double convert_double(uint16_t v) {return double(v);}
inline double convert_double(int32_t v) {return double(v);}
inline double convert_double(uint32_t v) {return double(v);}
inline double convert_double(int64_t v) {return double(v);}
inline double convert_double(uint64_t v) {return double(v);}
inline double convert_double(half v) {return double(v);}
inline double convert_double(float v) {return double(v);}
inline double convert_double(double v) {return v;}



inline byte2 convert_byte2(bool2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(byte2 v) {return v;}
inline byte2 convert_byte2(ubyte2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(short2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(ushort2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(int2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(uint2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(long2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(ulong2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(half2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(float2 v) {return vector2(int8_t(v.x), int8_t(v.y));}
inline byte2 convert_byte2(double2 v) {return vector2(int8_t(v.x), int8_t(v.y));}

inline ubyte2 convert_ubyte2(bool2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(byte2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(ubyte2 v) {return v;}
inline ubyte2 convert_ubyte2(short2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(ushort2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(int2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(uint2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(long2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(ulong2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(half2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(float2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}
inline ubyte2 convert_ubyte2(double2 v) {return vector2(uint8_t(v.x), uint8_t(v.y));}

inline short2 convert_short2(bool2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(byte2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(ubyte2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(short2 v) {return v;}
inline short2 convert_short2(ushort2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(int2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(uint2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(long2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(ulong2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(half2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(float2 v) {return vector2(int16_t(v.x), int16_t(v.y));}
inline short2 convert_short2(double2 v) {return vector2(int16_t(v.x), int16_t(v.y));}

inline ushort2 convert_ushort2(bool2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(byte2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(ubyte2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(short2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(ushort2 v) {return v;}
inline ushort2 convert_ushort2(int2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(uint2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(long2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(ulong2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(half2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(float2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}
inline ushort2 convert_ushort2(double2 v) {return vector2(uint16_t(v.x), uint16_t(v.y));}

inline int2 convert_int2(bool2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(byte2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(ubyte2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(short2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(ushort2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(int2 v) {return v;}
inline int2 convert_int2(uint2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(long2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(ulong2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(half2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(float2 v) {return vector2(int32_t(v.x), int32_t(v.y));}
inline int2 convert_int2(double2 v) {return vector2(int32_t(v.x), int32_t(v.y));}

inline uint2 convert_uint2(bool2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(byte2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(ubyte2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(short2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(ushort2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(int2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(uint2 v) {return v;}
inline uint2 convert_uint2(long2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(ulong2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(half2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(float2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}
inline uint2 convert_uint2(double2 v) {return vector2(uint32_t(v.x), uint32_t(v.y));}

inline long2 convert_long2(bool2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(byte2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(ubyte2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(short2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(ushort2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(int2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(uint2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(long2 v) {return v;}
inline long2 convert_long2(ulong2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(half2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(float2 v) {return vector2(int64_t(v.x), int64_t(v.y));}
inline long2 convert_long2(double2 v) {return vector2(int64_t(v.x), int64_t(v.y));}

inline ulong2 convert_ulong2(bool2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(byte2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(ubyte2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(short2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(ushort2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(int2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(uint2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(long2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(ulong2 v) {return v;}
inline ulong2 convert_ulong2(half2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(float2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}
inline ulong2 convert_ulong2(double2 v) {return vector2(uint64_t(v.x), uint64_t(v.y));}

inline half2 convert_half2(bool2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(byte2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(ubyte2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(short2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(ushort2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(int2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(uint2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(long2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(ulong2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(half2 v) {return v;}
inline half2 convert_half2(float2 v) {return vector2(half(v.x), half(v.y));}
inline half2 convert_half2(double2 v) {return vector2(half(v.x), half(v.y));}

inline float2 convert_float2(bool2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(byte2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(ubyte2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(short2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(ushort2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(int2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(uint2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(long2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(ulong2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(half2 v) {return vector2(float(v.x), float(v.y));}
inline float2 convert_float2(float2 v) {return v;}
inline float2 convert_float2(double2 v) {return vector2(float(v.x), float(v.y));}

inline double2 convert_double2(bool2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(byte2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(ubyte2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(short2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(ushort2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(int2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(uint2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(long2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(ulong2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(half2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(float2 v) {return vector2(double(v.x), double(v.y));}
inline double2 convert_double2(double2 v) {return v;}



inline byte3 convert_byte3(bool3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(byte3 v) {return v;}
inline byte3 convert_byte3(ubyte3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(short3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(ushort3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(int3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(uint3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(long3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(ulong3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(half3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(float3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}
inline byte3 convert_byte3(double3 v) {return vector3(int8_t(v.x), int8_t(v.y), int8_t(v.z));}

inline ubyte3 convert_ubyte3(bool3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(byte3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(ubyte3 v) {return v;}
inline ubyte3 convert_ubyte3(short3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(ushort3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(int3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(uint3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(long3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(ulong3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(half3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(float3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}
inline ubyte3 convert_ubyte3(double3 v) {return vector3(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z));}

inline short3 convert_short3(bool3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(byte3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(ubyte3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(short3 v) {return v;}
inline short3 convert_short3(ushort3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(int3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(uint3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(long3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(ulong3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(half3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(float3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}
inline short3 convert_short3(double3 v) {return vector3(int16_t(v.x), int16_t(v.y), int16_t(v.z));}

inline ushort3 convert_ushort3(bool3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(byte3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(ubyte3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(short3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(ushort3 v) {return v;}
inline ushort3 convert_ushort3(int3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(uint3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(long3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(ulong3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(half3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(float3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}
inline ushort3 convert_ushort3(double3 v) {return vector3(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z));}

inline int3 convert_int3(bool3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(byte3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(ubyte3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(short3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(ushort3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(int3 v) {return v;}
inline int3 convert_int3(uint3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(long3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(ulong3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(half3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(float3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}
inline int3 convert_int3(double3 v) {return vector3(int32_t(v.x), int32_t(v.y), int32_t(v.z));}

inline uint3 convert_uint3(bool3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(byte3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(ubyte3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(short3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(ushort3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(int3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(uint3 v) {return v;}
inline uint3 convert_uint3(long3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(ulong3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(half3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(float3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}
inline uint3 convert_uint3(double3 v) {return vector3(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z));}

inline long3 convert_long3(bool3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(byte3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(ubyte3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(short3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(ushort3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(int3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(uint3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(long3 v) {return v;}
inline long3 convert_long3(ulong3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(half3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(float3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}
inline long3 convert_long3(double3 v) {return vector3(int64_t(v.x), int64_t(v.y), int64_t(v.z));}

inline ulong3 convert_ulong3(bool3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(byte3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(ubyte3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(short3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(ushort3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(int3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(uint3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(long3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(ulong3 v) {return v;}
inline ulong3 convert_ulong3(half3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(float3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}
inline ulong3 convert_ulong3(double3 v) {return vector3(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z));}

inline half3 convert_half3(bool3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(byte3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(ubyte3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(short3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(ushort3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(int3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(uint3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(long3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(ulong3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(half3 v) {return v;}
inline half3 convert_half3(float3 v) {return vector3(half(v.x), half(v.y), half(v.z));}
inline half3 convert_half3(double3 v) {return vector3(half(v.x), half(v.y), half(v.z));}

inline float3 convert_float3(bool3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(byte3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(ubyte3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(short3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(ushort3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(int3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(uint3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(long3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(ulong3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(half3 v) {return vector3(float(v.x), float(v.y), float(v.z));}
inline float3 convert_float3(float3 v) {return v;}
inline float3 convert_float3(double3 v) {return vector3(float(v.x), float(v.y), float(v.z));}

inline double3 convert_double3(bool3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(byte3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(ubyte3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(short3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(ushort3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(int3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(uint3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(long3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(ulong3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(half3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(float3 v) {return vector3(double(v.x), double(v.y), double(v.z));}
inline double3 convert_double3(double3 v) {return v;}



inline byte4 convert_byte4(bool4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(byte4 v) {return v;}
inline byte4 convert_byte4(ubyte4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(short4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(ushort4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(int4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(uint4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(long4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(ulong4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(half4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(float4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}
inline byte4 convert_byte4(double4 v) {return vector4(int8_t(v.x), int8_t(v.y), int8_t(v.z), int8_t(v.w));}

inline ubyte4 convert_ubyte4(bool4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(byte4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(ubyte4 v) {return v;}
inline ubyte4 convert_ubyte4(short4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(ushort4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(int4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(uint4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(long4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(ulong4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(half4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(float4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}
inline ubyte4 convert_ubyte4(double4 v) {return vector4(uint8_t(v.x), uint8_t(v.y), uint8_t(v.z), uint8_t(v.w));}

inline short4 convert_short4(bool4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(byte4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(ubyte4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(short4 v) {return v;}
inline short4 convert_short4(ushort4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(int4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(uint4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(long4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(ulong4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(half4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(float4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}
inline short4 convert_short4(double4 v) {return vector4(int16_t(v.x), int16_t(v.y), int16_t(v.z), int16_t(v.w));}

inline ushort4 convert_ushort4(bool4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(byte4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(ubyte4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(short4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(ushort4 v) {return v;}
inline ushort4 convert_ushort4(int4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(uint4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(long4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(ulong4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(half4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(float4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}
inline ushort4 convert_ushort4(double4 v) {return vector4(uint16_t(v.x), uint16_t(v.y), uint16_t(v.z), uint16_t(v.w));}

inline int4 convert_int4(bool4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(byte4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(ubyte4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(short4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(ushort4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(int4 v) {return v;}
inline int4 convert_int4(uint4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(long4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(ulong4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(half4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(float4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}
inline int4 convert_int4(double4 v) {return vector4(int32_t(v.x), int32_t(v.y), int32_t(v.z), int32_t(v.w));}

inline uint4 convert_uint4(bool4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(byte4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(ubyte4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(short4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(ushort4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(int4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(uint4 v) {return v;}
inline uint4 convert_uint4(long4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(ulong4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(half4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(float4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}
inline uint4 convert_uint4(double4 v) {return vector4(uint32_t(v.x), uint32_t(v.y), uint32_t(v.z), uint32_t(v.w));}

inline long4 convert_long4(bool4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(byte4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(ubyte4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(short4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(ushort4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(int4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(uint4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(long4 v) {return v;}
inline long4 convert_long4(ulong4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(half4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(float4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}
inline long4 convert_long4(double4 v) {return vector4(int64_t(v.x), int64_t(v.y), int64_t(v.z), int64_t(v.w));}

inline ulong4 convert_ulong4(bool4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(byte4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(ubyte4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(short4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(ushort4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(int4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(uint4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(long4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(ulong4 v) {return v;}
inline ulong4 convert_ulong4(half4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(float4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}
inline ulong4 convert_ulong4(double4 v) {return vector4(uint64_t(v.x), uint64_t(v.y), uint64_t(v.z), uint64_t(v.w));}

inline half4 convert_half4(bool4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(byte4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(ubyte4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(short4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(ushort4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(int4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(uint4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(long4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(ulong4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(half4 v) {return v;}
inline half4 convert_half4(float4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}
inline half4 convert_half4(double4 v) {return vector4(half(v.x), half(v.y), half(v.z), half(v.w));}

inline float4 convert_float4(bool4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(byte4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(ubyte4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(short4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(ushort4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(int4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(uint4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(long4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(ulong4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(half4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}
inline float4 convert_float4(float4 v) {return v;}
inline float4 convert_float4(double4 v) {return vector4(float(v.x), float(v.y), float(v.z), float(v.w));}

inline double4 convert_double4(bool4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(byte4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(ubyte4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(short4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(ushort4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(int4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(uint4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(long4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(ulong4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(half4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(float4 v) {return vector4(double(v.x), double(v.y), double(v.z), double(v.w));}
inline double4 convert_double4(double4 v) {return v;}



inline int8_t convert_byte_rte(half v) {return rint<int8_t>(v);}
inline int8_t convert_byte_rte(float v) {return rint<int8_t>(v);}
inline int8_t convert_byte_rte(double v) {return rint<int8_t>(v);}

inline uint8_t convert_ubyte_rte(half v) {return rint<uint8_t>(v);}
inline uint8_t convert_ubyte_rte(float v) {return rint<uint8_t>(v);}
inline uint8_t convert_ubyte_rte(double v) {return rint<uint8_t>(v);}

inline int16_t convert_short_rte(half v) {return rint<int16_t>(v);}
inline int16_t convert_short_rte(float v) {return rint<int16_t>(v);}
inline int16_t convert_short_rte(double v) {return rint<int16_t>(v);}

inline uint16_t convert_ushort_rte(half v) {return rint<uint16_t>(v);}
inline uint16_t convert_ushort_rte(float v) {return rint<uint16_t>(v);}
inline uint16_t convert_ushort_rte(double v) {return rint<uint16_t>(v);}

inline int32_t convert_int_rte(half v) {return rint<int32_t>(v);}
inline int32_t convert_int_rte(float v) {return rint<int32_t>(v);}
inline int32_t convert_int_rte(double v) {return rint<int32_t>(v);}

inline uint32_t convert_uint_rte(half v) {return rint<uint32_t>(v);}
inline uint32_t convert_uint_rte(float v) {return rint<uint32_t>(v);}
inline uint32_t convert_uint_rte(double v) {return rint<uint32_t>(v);}

inline int64_t convert_long_rte(half v) {return rint<int64_t>(v);}
inline int64_t convert_long_rte(float v) {return rint<int64_t>(v);}
inline int64_t convert_long_rte(double v) {return rint<int64_t>(v);}

inline uint64_t convert_ulong_rte(half v) {return rint<uint64_t>(v);}
inline uint64_t convert_ulong_rte(float v) {return rint<uint64_t>(v);}
inline uint64_t convert_ulong_rte(double v) {return rint<uint64_t>(v);}






inline byte2 convert_byte2_rte(half2 v) {return vector2(rint<int8_t>(v.x), rint<int8_t>(v.y));}
inline byte2 convert_byte2_rte(float2 v) {return vector2(rint<int8_t>(v.x), rint<int8_t>(v.y));}
inline byte2 convert_byte2_rte(double2 v) {return vector2(rint<int8_t>(v.x), rint<int8_t>(v.y));}

inline ubyte2 convert_ubyte2_rte(half2 v) {return vector2(rint<uint8_t>(v.x), rint<uint8_t>(v.y));}
inline ubyte2 convert_ubyte2_rte(float2 v) {return vector2(rint<uint8_t>(v.x), rint<uint8_t>(v.y));}
inline ubyte2 convert_ubyte2_rte(double2 v) {return vector2(rint<uint8_t>(v.x), rint<uint8_t>(v.y));}

inline short2 convert_short2_rte(half2 v) {return vector2(rint<int16_t>(v.x), rint<int16_t>(v.y));}
inline short2 convert_short2_rte(float2 v) {return vector2(rint<int16_t>(v.x), rint<int16_t>(v.y));}
inline short2 convert_short2_rte(double2 v) {return vector2(rint<int16_t>(v.x), rint<int16_t>(v.y));}

inline ushort2 convert_ushort2_rte(half2 v) {return vector2(rint<uint16_t>(v.x), rint<uint16_t>(v.y));}
inline ushort2 convert_ushort2_rte(float2 v) {return vector2(rint<uint16_t>(v.x), rint<uint16_t>(v.y));}
inline ushort2 convert_ushort2_rte(double2 v) {return vector2(rint<uint16_t>(v.x), rint<uint16_t>(v.y));}

inline int2 convert_int2_rte(half2 v) {return vector2(rint<int32_t>(v.x), rint<int32_t>(v.y));}
inline int2 convert_int2_rte(float2 v) {return vector2(rint<int32_t>(v.x), rint<int32_t>(v.y));}
inline int2 convert_int2_rte(double2 v) {return vector2(rint<int32_t>(v.x), rint<int32_t>(v.y));}

inline uint2 convert_uint2_rte(half2 v) {return vector2(rint<uint32_t>(v.x), rint<uint32_t>(v.y));}
inline uint2 convert_uint2_rte(float2 v) {return vector2(rint<uint32_t>(v.x), rint<uint32_t>(v.y));}
inline uint2 convert_uint2_rte(double2 v) {return vector2(rint<uint32_t>(v.x), rint<uint32_t>(v.y));}

inline long2 convert_long2_rte(half2 v) {return vector2(rint<int64_t>(v.x), rint<int64_t>(v.y));}
inline long2 convert_long2_rte(float2 v) {return vector2(rint<int64_t>(v.x), rint<int64_t>(v.y));}
inline long2 convert_long2_rte(double2 v) {return vector2(rint<int64_t>(v.x), rint<int64_t>(v.y));}

inline ulong2 convert_ulong2_rte(half2 v) {return vector2(rint<uint64_t>(v.x), rint<uint64_t>(v.y));}
inline ulong2 convert_ulong2_rte(float2 v) {return vector2(rint<uint64_t>(v.x), rint<uint64_t>(v.y));}
inline ulong2 convert_ulong2_rte(double2 v) {return vector2(rint<uint64_t>(v.x), rint<uint64_t>(v.y));}






inline byte3 convert_byte3_rte(half3 v) {return vector3(rint<int8_t>(v.x), rint<int8_t>(v.y), rint<int8_t>(v.z));}
inline byte3 convert_byte3_rte(float3 v) {return vector3(rint<int8_t>(v.x), rint<int8_t>(v.y), rint<int8_t>(v.z));}
inline byte3 convert_byte3_rte(double3 v) {return vector3(rint<int8_t>(v.x), rint<int8_t>(v.y), rint<int8_t>(v.z));}

inline ubyte3 convert_ubyte3_rte(half3 v) {return vector3(rint<uint8_t>(v.x), rint<uint8_t>(v.y), rint<uint8_t>(v.z));}
inline ubyte3 convert_ubyte3_rte(float3 v) {return vector3(rint<uint8_t>(v.x), rint<uint8_t>(v.y), rint<uint8_t>(v.z));}
inline ubyte3 convert_ubyte3_rte(double3 v) {return vector3(rint<uint8_t>(v.x), rint<uint8_t>(v.y), rint<uint8_t>(v.z));}

inline short3 convert_short3_rte(half3 v) {return vector3(rint<int16_t>(v.x), rint<int16_t>(v.y), rint<int16_t>(v.z));}
inline short3 convert_short3_rte(float3 v) {return vector3(rint<int16_t>(v.x), rint<int16_t>(v.y), rint<int16_t>(v.z));}
inline short3 convert_short3_rte(double3 v) {return vector3(rint<int16_t>(v.x), rint<int16_t>(v.y), rint<int16_t>(v.z));}

inline ushort3 convert_ushort3_rte(half3 v) {return vector3(rint<uint16_t>(v.x), rint<uint16_t>(v.y), rint<uint16_t>(v.z));}
inline ushort3 convert_ushort3_rte(float3 v) {return vector3(rint<uint16_t>(v.x), rint<uint16_t>(v.y), rint<uint16_t>(v.z));}
inline ushort3 convert_ushort3_rte(double3 v) {return vector3(rint<uint16_t>(v.x), rint<uint16_t>(v.y), rint<uint16_t>(v.z));}

inline int3 convert_int3_rte(half3 v) {return vector3(rint<int32_t>(v.x), rint<int32_t>(v.y), rint<int32_t>(v.z));}
inline int3 convert_int3_rte(float3 v) {return vector3(rint<int32_t>(v.x), rint<int32_t>(v.y), rint<int32_t>(v.z));}
inline int3 convert_int3_rte(double3 v) {return vector3(rint<int32_t>(v.x), rint<int32_t>(v.y), rint<int32_t>(v.z));}

inline uint3 convert_uint3_rte(half3 v) {return vector3(rint<uint32_t>(v.x), rint<uint32_t>(v.y), rint<uint32_t>(v.z));}
inline uint3 convert_uint3_rte(float3 v) {return vector3(rint<uint32_t>(v.x), rint<uint32_t>(v.y), rint<uint32_t>(v.z));}
inline uint3 convert_uint3_rte(double3 v) {return vector3(rint<uint32_t>(v.x), rint<uint32_t>(v.y), rint<uint32_t>(v.z));}

inline long3 convert_long3_rte(half3 v) {return vector3(rint<int64_t>(v.x), rint<int64_t>(v.y), rint<int64_t>(v.z));}
inline long3 convert_long3_rte(float3 v) {return vector3(rint<int64_t>(v.x), rint<int64_t>(v.y), rint<int64_t>(v.z));}
inline long3 convert_long3_rte(double3 v) {return vector3(rint<int64_t>(v.x), rint<int64_t>(v.y), rint<int64_t>(v.z));}

inline ulong3 convert_ulong3_rte(half3 v) {return vector3(rint<uint64_t>(v.x), rint<uint64_t>(v.y), rint<uint64_t>(v.z));}
inline ulong3 convert_ulong3_rte(float3 v) {return vector3(rint<uint64_t>(v.x), rint<uint64_t>(v.y), rint<uint64_t>(v.z));}
inline ulong3 convert_ulong3_rte(double3 v) {return vector3(rint<uint64_t>(v.x), rint<uint64_t>(v.y), rint<uint64_t>(v.z));}






inline byte4 convert_byte4_rte(half4 v) {return vector4(rint<int8_t>(v.x), rint<int8_t>(v.y), rint<int8_t>(v.z), rint<int8_t>(v.w));}
inline byte4 convert_byte4_rte(float4 v) {return vector4(rint<int8_t>(v.x), rint<int8_t>(v.y), rint<int8_t>(v.z), rint<int8_t>(v.w));}
inline byte4 convert_byte4_rte(double4 v) {return vector4(rint<int8_t>(v.x), rint<int8_t>(v.y), rint<int8_t>(v.z), rint<int8_t>(v.w));}

inline ubyte4 convert_ubyte4_rte(half4 v) {return vector4(rint<uint8_t>(v.x), rint<uint8_t>(v.y), rint<uint8_t>(v.z), rint<uint8_t>(v.w));}
inline ubyte4 convert_ubyte4_rte(float4 v) {return vector4(rint<uint8_t>(v.x), rint<uint8_t>(v.y), rint<uint8_t>(v.z), rint<uint8_t>(v.w));}
inline ubyte4 convert_ubyte4_rte(double4 v) {return vector4(rint<uint8_t>(v.x), rint<uint8_t>(v.y), rint<uint8_t>(v.z), rint<uint8_t>(v.w));}

inline short4 convert_short4_rte(half4 v) {return vector4(rint<int16_t>(v.x), rint<int16_t>(v.y), rint<int16_t>(v.z), rint<int16_t>(v.w));}
inline short4 convert_short4_rte(float4 v) {return vector4(rint<int16_t>(v.x), rint<int16_t>(v.y), rint<int16_t>(v.z), rint<int16_t>(v.w));}
inline short4 convert_short4_rte(double4 v) {return vector4(rint<int16_t>(v.x), rint<int16_t>(v.y), rint<int16_t>(v.z), rint<int16_t>(v.w));}

inline ushort4 convert_ushort4_rte(half4 v) {return vector4(rint<uint16_t>(v.x), rint<uint16_t>(v.y), rint<uint16_t>(v.z), rint<uint16_t>(v.w));}
inline ushort4 convert_ushort4_rte(float4 v) {return vector4(rint<uint16_t>(v.x), rint<uint16_t>(v.y), rint<uint16_t>(v.z), rint<uint16_t>(v.w));}
inline ushort4 convert_ushort4_rte(double4 v) {return vector4(rint<uint16_t>(v.x), rint<uint16_t>(v.y), rint<uint16_t>(v.z), rint<uint16_t>(v.w));}

inline int4 convert_int4_rte(half4 v) {return vector4(rint<int32_t>(v.x), rint<int32_t>(v.y), rint<int32_t>(v.z), rint<int32_t>(v.w));}
inline int4 convert_int4_rte(float4 v) {return vector4(rint<int32_t>(v.x), rint<int32_t>(v.y), rint<int32_t>(v.z), rint<int32_t>(v.w));}
inline int4 convert_int4_rte(double4 v) {return vector4(rint<int32_t>(v.x), rint<int32_t>(v.y), rint<int32_t>(v.z), rint<int32_t>(v.w));}

inline uint4 convert_uint4_rte(half4 v) {return vector4(rint<uint32_t>(v.x), rint<uint32_t>(v.y), rint<uint32_t>(v.z), rint<uint32_t>(v.w));}
inline uint4 convert_uint4_rte(float4 v) {return vector4(rint<uint32_t>(v.x), rint<uint32_t>(v.y), rint<uint32_t>(v.z), rint<uint32_t>(v.w));}
inline uint4 convert_uint4_rte(double4 v) {return vector4(rint<uint32_t>(v.x), rint<uint32_t>(v.y), rint<uint32_t>(v.z), rint<uint32_t>(v.w));}

inline long4 convert_long4_rte(half4 v) {return vector4(rint<int64_t>(v.x), rint<int64_t>(v.y), rint<int64_t>(v.z), rint<int64_t>(v.w));}
inline long4 convert_long4_rte(float4 v) {return vector4(rint<int64_t>(v.x), rint<int64_t>(v.y), rint<int64_t>(v.z), rint<int64_t>(v.w));}
inline long4 convert_long4_rte(double4 v) {return vector4(rint<int64_t>(v.x), rint<int64_t>(v.y), rint<int64_t>(v.z), rint<int64_t>(v.w));}

inline ulong4 convert_ulong4_rte(half4 v) {return vector4(rint<uint64_t>(v.x), rint<uint64_t>(v.y), rint<uint64_t>(v.z), rint<uint64_t>(v.w));}
inline ulong4 convert_ulong4_rte(float4 v) {return vector4(rint<uint64_t>(v.x), rint<uint64_t>(v.y), rint<uint64_t>(v.z), rint<uint64_t>(v.w));}
inline ulong4 convert_ulong4_rte(double4 v) {return vector4(rint<uint64_t>(v.x), rint<uint64_t>(v.y), rint<uint64_t>(v.z), rint<uint64_t>(v.w));}






inline int8_t convert_byte_sat(int8_t v) {return v;}
inline int8_t convert_byte_sat(uint8_t v) {return min_rtz(v, std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(int16_t v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(uint16_t v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(int32_t v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(uint32_t v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(int64_t v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(uint64_t v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(half v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(float v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat(double v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline uint8_t convert_ubyte_sat(int8_t v) {return max_rtz(v, uint8_t(0));}
inline uint8_t convert_ubyte_sat(uint8_t v) {return v;}
inline uint8_t convert_ubyte_sat(int16_t v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(uint16_t v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(int32_t v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(uint32_t v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(int64_t v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(uint64_t v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(half v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(float v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat(double v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline int16_t convert_short_sat(int8_t v) {return convert_short(v);}
inline int16_t convert_short_sat(uint8_t v) {return convert_short(v);}
inline int16_t convert_short_sat(int16_t v) {return v;}
inline int16_t convert_short_sat(uint16_t v) {return min_rtz(v, std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat(int32_t v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat(uint32_t v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat(int64_t v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat(uint64_t v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat(half v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat(float v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat(double v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline uint16_t convert_ushort_sat(int8_t v) {return convert_ushort(v);}
inline uint16_t convert_ushort_sat(uint8_t v) {return convert_ushort(v);}
inline uint16_t convert_ushort_sat(int16_t v) {return max_rtz(v, uint16_t(0));}
inline uint16_t convert_ushort_sat(uint16_t v) {return v;}
inline uint16_t convert_ushort_sat(int32_t v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat(uint32_t v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat(int64_t v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat(uint64_t v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat(half v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat(float v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat(double v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int32_t convert_int_sat(int8_t v) {return convert_int(v);}
inline int32_t convert_int_sat(uint8_t v) {return convert_int(v);}
inline int32_t convert_int_sat(int16_t v) {return convert_int(v);}
inline int32_t convert_int_sat(uint16_t v) {return convert_int(v);}
inline int32_t convert_int_sat(int32_t v) {return v;}
inline int32_t convert_int_sat(uint32_t v) {return min_rtz(v, std::numeric_limits<int32_t>::max());}
inline int32_t convert_int_sat(int64_t v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int32_t convert_int_sat(uint64_t v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int32_t convert_int_sat(half v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int32_t convert_int_sat(float v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int32_t convert_int_sat(double v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint32_t convert_uint_sat(int8_t v) {return convert_uint(v);}
inline uint32_t convert_uint_sat(uint8_t v) {return convert_uint(v);}
inline uint32_t convert_uint_sat(int16_t v) {return convert_uint(v);}
inline uint32_t convert_uint_sat(uint16_t v) {return convert_uint(v);}
inline uint32_t convert_uint_sat(int32_t v) {return max_rtz(v, uint32_t(0));}
inline uint32_t convert_uint_sat(uint32_t v) {return v;}
inline uint32_t convert_uint_sat(int64_t v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint32_t convert_uint_sat(uint64_t v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint32_t convert_uint_sat(half v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint32_t convert_uint_sat(float v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint32_t convert_uint_sat(double v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline int64_t convert_long_sat(int8_t v) {return convert_long(v);}
inline int64_t convert_long_sat(uint8_t v) {return convert_long(v);}
inline int64_t convert_long_sat(int16_t v) {return convert_long(v);}
inline int64_t convert_long_sat(uint16_t v) {return convert_long(v);}
inline int64_t convert_long_sat(int32_t v) {return convert_long(v);}
inline int64_t convert_long_sat(uint32_t v) {return convert_long(v);}
inline int64_t convert_long_sat(int64_t v) {return v;}
inline int64_t convert_long_sat(uint64_t v) {return min_rtz(v, std::numeric_limits<int64_t>::max());}
inline int64_t convert_long_sat(half v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline int64_t convert_long_sat(float v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline int64_t convert_long_sat(double v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline uint64_t convert_ulong_sat(int8_t v) {return convert_ulong(v);}
inline uint64_t convert_ulong_sat(uint8_t v) {return convert_ulong(v);}
inline uint64_t convert_ulong_sat(int16_t v) {return convert_ulong(v);}
inline uint64_t convert_ulong_sat(uint16_t v) {return convert_ulong(v);}
inline uint64_t convert_ulong_sat(int32_t v) {return convert_ulong(v);}
inline uint64_t convert_ulong_sat(uint32_t v) {return convert_ulong(v);}
inline uint64_t convert_ulong_sat(int64_t v) {return max_rtz(v, uint64_t(0));}
inline uint64_t convert_ulong_sat(uint64_t v) {return v;}
inline uint64_t convert_ulong_sat(half v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline uint64_t convert_ulong_sat(float v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline uint64_t convert_ulong_sat(double v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline byte2 convert_byte2_sat(byte2 v) {return v;}
inline byte2 convert_byte2_sat(ubyte2 v) {return min_rtz(v, std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(short2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(ushort2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(int2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(uint2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(long2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(ulong2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline ubyte2 convert_ubyte2_sat(byte2 v) {return max_rtz(v, uint8_t(0));}
inline ubyte2 convert_ubyte2_sat(ubyte2 v) {return v;}
inline ubyte2 convert_ubyte2_sat(short2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(ushort2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(int2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(uint2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(long2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(ulong2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline short2 convert_short2_sat(byte2 v) {return convert_short2(v);}
inline short2 convert_short2_sat(ubyte2 v) {return convert_short2(v);}
inline short2 convert_short2_sat(short2 v) {return v;}
inline short2 convert_short2_sat(ushort2 v) {return min_rtz(v, std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat(int2 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat(uint2 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat(long2 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat(ulong2 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline ushort2 convert_ushort2_sat(byte2 v) {return convert_ushort2(v);}
inline ushort2 convert_ushort2_sat(ubyte2 v) {return convert_ushort2(v);}
inline ushort2 convert_ushort2_sat(short2 v) {return max_rtz(v, uint16_t(0));}
inline ushort2 convert_ushort2_sat(ushort2 v) {return v;}
inline ushort2 convert_ushort2_sat(int2 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat(uint2 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat(long2 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat(ulong2 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int2 convert_int2_sat(byte2 v) {return convert_int2(v);}
inline int2 convert_int2_sat(ubyte2 v) {return convert_int2(v);}
inline int2 convert_int2_sat(short2 v) {return convert_int2(v);}
inline int2 convert_int2_sat(ushort2 v) {return convert_int2(v);}
inline int2 convert_int2_sat(int2 v) {return v;}
inline int2 convert_int2_sat(uint2 v) {return min_rtz(v, std::numeric_limits<int32_t>::max());}
inline int2 convert_int2_sat(long2 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int2 convert_int2_sat(ulong2 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int2 convert_int2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int2 convert_int2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int2 convert_int2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint2 convert_uint2_sat(byte2 v) {return convert_uint2(v);}
inline uint2 convert_uint2_sat(ubyte2 v) {return convert_uint2(v);}
inline uint2 convert_uint2_sat(short2 v) {return convert_uint2(v);}
inline uint2 convert_uint2_sat(ushort2 v) {return convert_uint2(v);}
inline uint2 convert_uint2_sat(int2 v) {return max_rtz(v, uint32_t(0));}
inline uint2 convert_uint2_sat(uint2 v) {return v;}
inline uint2 convert_uint2_sat(long2 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint2 convert_uint2_sat(ulong2 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint2 convert_uint2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint2 convert_uint2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint2 convert_uint2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline long2 convert_long2_sat(byte2 v) {return convert_long2(v);}
inline long2 convert_long2_sat(ubyte2 v) {return convert_long2(v);}
inline long2 convert_long2_sat(short2 v) {return convert_long2(v);}
inline long2 convert_long2_sat(ushort2 v) {return convert_long2(v);}
inline long2 convert_long2_sat(int2 v) {return convert_long2(v);}
inline long2 convert_long2_sat(uint2 v) {return convert_long2(v);}
inline long2 convert_long2_sat(long2 v) {return v;}
inline long2 convert_long2_sat(ulong2 v) {return min_rtz(v, std::numeric_limits<int64_t>::max());}
inline long2 convert_long2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long2 convert_long2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long2 convert_long2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline ulong2 convert_ulong2_sat(byte2 v) {return convert_ulong2(v);}
inline ulong2 convert_ulong2_sat(ubyte2 v) {return convert_ulong2(v);}
inline ulong2 convert_ulong2_sat(short2 v) {return convert_ulong2(v);}
inline ulong2 convert_ulong2_sat(ushort2 v) {return convert_ulong2(v);}
inline ulong2 convert_ulong2_sat(int2 v) {return convert_ulong2(v);}
inline ulong2 convert_ulong2_sat(uint2 v) {return convert_ulong2(v);}
inline ulong2 convert_ulong2_sat(long2 v) {return max_rtz(v, uint64_t(0));}
inline ulong2 convert_ulong2_sat(ulong2 v) {return v;}
inline ulong2 convert_ulong2_sat(half2 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong2 convert_ulong2_sat(float2 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong2 convert_ulong2_sat(double2 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline byte3 convert_byte3_sat(byte3 v) {return v;}
inline byte3 convert_byte3_sat(ubyte3 v) {return min_rtz(v, std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(short3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(ushort3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(int3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(uint3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(long3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(ulong3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline ubyte3 convert_ubyte3_sat(byte3 v) {return max_rtz(v, uint8_t(0));}
inline ubyte3 convert_ubyte3_sat(ubyte3 v) {return v;}
inline ubyte3 convert_ubyte3_sat(short3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(ushort3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(int3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(uint3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(long3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(ulong3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline short3 convert_short3_sat(byte3 v) {return convert_short3(v);}
inline short3 convert_short3_sat(ubyte3 v) {return convert_short3(v);}
inline short3 convert_short3_sat(short3 v) {return v;}
inline short3 convert_short3_sat(ushort3 v) {return min_rtz(v, std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat(int3 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat(uint3 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat(long3 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat(ulong3 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline ushort3 convert_ushort3_sat(byte3 v) {return convert_ushort3(v);}
inline ushort3 convert_ushort3_sat(ubyte3 v) {return convert_ushort3(v);}
inline ushort3 convert_ushort3_sat(short3 v) {return max_rtz(v, uint16_t(0));}
inline ushort3 convert_ushort3_sat(ushort3 v) {return v;}
inline ushort3 convert_ushort3_sat(int3 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat(uint3 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat(long3 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat(ulong3 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int3 convert_int3_sat(byte3 v) {return convert_int3(v);}
inline int3 convert_int3_sat(ubyte3 v) {return convert_int3(v);}
inline int3 convert_int3_sat(short3 v) {return convert_int3(v);}
inline int3 convert_int3_sat(ushort3 v) {return convert_int3(v);}
inline int3 convert_int3_sat(int3 v) {return v;}
inline int3 convert_int3_sat(uint3 v) {return min_rtz(v, std::numeric_limits<int32_t>::max());}
inline int3 convert_int3_sat(long3 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int3 convert_int3_sat(ulong3 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int3 convert_int3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int3 convert_int3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int3 convert_int3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint3 convert_uint3_sat(byte3 v) {return convert_uint3(v);}
inline uint3 convert_uint3_sat(ubyte3 v) {return convert_uint3(v);}
inline uint3 convert_uint3_sat(short3 v) {return convert_uint3(v);}
inline uint3 convert_uint3_sat(ushort3 v) {return convert_uint3(v);}
inline uint3 convert_uint3_sat(int3 v) {return max_rtz(v, uint32_t(0));}
inline uint3 convert_uint3_sat(uint3 v) {return v;}
inline uint3 convert_uint3_sat(long3 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint3 convert_uint3_sat(ulong3 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint3 convert_uint3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint3 convert_uint3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint3 convert_uint3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline long3 convert_long3_sat(byte3 v) {return convert_long3(v);}
inline long3 convert_long3_sat(ubyte3 v) {return convert_long3(v);}
inline long3 convert_long3_sat(short3 v) {return convert_long3(v);}
inline long3 convert_long3_sat(ushort3 v) {return convert_long3(v);}
inline long3 convert_long3_sat(int3 v) {return convert_long3(v);}
inline long3 convert_long3_sat(uint3 v) {return convert_long3(v);}
inline long3 convert_long3_sat(long3 v) {return v;}
inline long3 convert_long3_sat(ulong3 v) {return min_rtz(v, std::numeric_limits<int64_t>::max());}
inline long3 convert_long3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long3 convert_long3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long3 convert_long3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline ulong3 convert_ulong3_sat(byte3 v) {return convert_ulong3(v);}
inline ulong3 convert_ulong3_sat(ubyte3 v) {return convert_ulong3(v);}
inline ulong3 convert_ulong3_sat(short3 v) {return convert_ulong3(v);}
inline ulong3 convert_ulong3_sat(ushort3 v) {return convert_ulong3(v);}
inline ulong3 convert_ulong3_sat(int3 v) {return convert_ulong3(v);}
inline ulong3 convert_ulong3_sat(uint3 v) {return convert_ulong3(v);}
inline ulong3 convert_ulong3_sat(long3 v) {return max_rtz(v, uint64_t(0));}
inline ulong3 convert_ulong3_sat(ulong3 v) {return v;}
inline ulong3 convert_ulong3_sat(half3 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong3 convert_ulong3_sat(float3 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong3 convert_ulong3_sat(double3 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline byte4 convert_byte4_sat(byte4 v) {return v;}
inline byte4 convert_byte4_sat(ubyte4 v) {return min_rtz(v, std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(short4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(ushort4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(int4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(uint4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(long4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(ulong4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline ubyte4 convert_ubyte4_sat(byte4 v) {return max_rtz(v, uint8_t(0));}
inline ubyte4 convert_ubyte4_sat(ubyte4 v) {return v;}
inline ubyte4 convert_ubyte4_sat(short4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(ushort4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(int4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(uint4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(long4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(ulong4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline short4 convert_short4_sat(byte4 v) {return convert_short4(v);}
inline short4 convert_short4_sat(ubyte4 v) {return convert_short4(v);}
inline short4 convert_short4_sat(short4 v) {return v;}
inline short4 convert_short4_sat(ushort4 v) {return min_rtz(v, std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat(int4 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat(uint4 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat(long4 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat(ulong4 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline ushort4 convert_ushort4_sat(byte4 v) {return convert_ushort4(v);}
inline ushort4 convert_ushort4_sat(ubyte4 v) {return convert_ushort4(v);}
inline ushort4 convert_ushort4_sat(short4 v) {return max_rtz(v, uint16_t(0));}
inline ushort4 convert_ushort4_sat(ushort4 v) {return v;}
inline ushort4 convert_ushort4_sat(int4 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat(uint4 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat(long4 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat(ulong4 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int4 convert_int4_sat(byte4 v) {return convert_int4(v);}
inline int4 convert_int4_sat(ubyte4 v) {return convert_int4(v);}
inline int4 convert_int4_sat(short4 v) {return convert_int4(v);}
inline int4 convert_int4_sat(ushort4 v) {return convert_int4(v);}
inline int4 convert_int4_sat(int4 v) {return v;}
inline int4 convert_int4_sat(uint4 v) {return min_rtz(v, std::numeric_limits<int32_t>::max());}
inline int4 convert_int4_sat(long4 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int4 convert_int4_sat(ulong4 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int4 convert_int4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int4 convert_int4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int4 convert_int4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint4 convert_uint4_sat(byte4 v) {return convert_uint4(v);}
inline uint4 convert_uint4_sat(ubyte4 v) {return convert_uint4(v);}
inline uint4 convert_uint4_sat(short4 v) {return convert_uint4(v);}
inline uint4 convert_uint4_sat(ushort4 v) {return convert_uint4(v);}
inline uint4 convert_uint4_sat(int4 v) {return max_rtz(v, uint32_t(0));}
inline uint4 convert_uint4_sat(uint4 v) {return v;}
inline uint4 convert_uint4_sat(long4 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint4 convert_uint4_sat(ulong4 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint4 convert_uint4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint4 convert_uint4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint4 convert_uint4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline long4 convert_long4_sat(byte4 v) {return convert_long4(v);}
inline long4 convert_long4_sat(ubyte4 v) {return convert_long4(v);}
inline long4 convert_long4_sat(short4 v) {return convert_long4(v);}
inline long4 convert_long4_sat(ushort4 v) {return convert_long4(v);}
inline long4 convert_long4_sat(int4 v) {return convert_long4(v);}
inline long4 convert_long4_sat(uint4 v) {return convert_long4(v);}
inline long4 convert_long4_sat(long4 v) {return v;}
inline long4 convert_long4_sat(ulong4 v) {return min_rtz(v, std::numeric_limits<int64_t>::max());}
inline long4 convert_long4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long4 convert_long4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long4 convert_long4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline ulong4 convert_ulong4_sat(byte4 v) {return convert_ulong4(v);}
inline ulong4 convert_ulong4_sat(ubyte4 v) {return convert_ulong4(v);}
inline ulong4 convert_ulong4_sat(short4 v) {return convert_ulong4(v);}
inline ulong4 convert_ulong4_sat(ushort4 v) {return convert_ulong4(v);}
inline ulong4 convert_ulong4_sat(int4 v) {return convert_ulong4(v);}
inline ulong4 convert_ulong4_sat(uint4 v) {return convert_ulong4(v);}
inline ulong4 convert_ulong4_sat(long4 v) {return max_rtz(v, uint64_t(0));}
inline ulong4 convert_ulong4_sat(ulong4 v) {return v;}
inline ulong4 convert_ulong4_sat(half4 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong4 convert_ulong4_sat(float4 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong4 convert_ulong4_sat(double4 v) {return clamp_rtz(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline int8_t convert_byte_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline int8_t convert_byte_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline uint8_t convert_ubyte_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline uint8_t convert_ubyte_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline int16_t convert_short_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline int16_t convert_short_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline uint16_t convert_ushort_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline uint16_t convert_ushort_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int32_t convert_int_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int32_t convert_int_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int32_t convert_int_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint32_t convert_uint_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint32_t convert_uint_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint32_t convert_uint_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline int64_t convert_long_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline int64_t convert_long_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline int64_t convert_long_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline uint64_t convert_ulong_sat_rte(half v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline uint64_t convert_ulong_sat_rte(float v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline uint64_t convert_ulong_sat_rte(double v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline byte2 convert_byte2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte2 convert_byte2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline ubyte2 convert_ubyte2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte2 convert_ubyte2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline short2 convert_short2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short2 convert_short2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline ushort2 convert_ushort2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort2 convert_ushort2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int2 convert_int2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int2 convert_int2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int2 convert_int2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint2 convert_uint2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint2 convert_uint2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint2 convert_uint2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline long2 convert_long2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long2 convert_long2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long2 convert_long2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline ulong2 convert_ulong2_sat_rte(half2 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong2 convert_ulong2_sat_rte(float2 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong2 convert_ulong2_sat_rte(double2 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline byte3 convert_byte3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte3 convert_byte3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline ubyte3 convert_ubyte3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte3 convert_ubyte3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline short3 convert_short3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short3 convert_short3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline ushort3 convert_ushort3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort3 convert_ushort3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int3 convert_int3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int3 convert_int3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int3 convert_int3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint3 convert_uint3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint3 convert_uint3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint3 convert_uint3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline long3 convert_long3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long3 convert_long3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long3 convert_long3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline ulong3 convert_ulong3_sat_rte(half3 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong3 convert_ulong3_sat_rte(float3 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong3 convert_ulong3_sat_rte(double3 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline byte4 convert_byte4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}
inline byte4 convert_byte4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());}

inline ubyte4 convert_ubyte4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}
inline ubyte4 convert_ubyte4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());}

inline short4 convert_short4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}
inline short4 convert_short4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());}

inline ushort4 convert_ushort4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}
inline ushort4 convert_ushort4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());}

inline int4 convert_int4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int4 convert_int4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}
inline int4 convert_int4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());}

inline uint4 convert_uint4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint4 convert_uint4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}
inline uint4 convert_uint4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());}

inline long4 convert_long4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long4 convert_long4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}
inline long4 convert_long4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());}

inline ulong4 convert_ulong4_sat_rte(half4 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong4 convert_ulong4_sat_rte(float4 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}
inline ulong4 convert_ulong4_sat_rte(double4 v) {return clamp_rte(v, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());}






inline int8_t as_byte(uint8_t v) {return *(int8_t*)&v;}

inline uint8_t as_ubyte(int8_t v) {return *(uint8_t*)&v;}

inline int16_t as_short(uint16_t v) {return *(int16_t*)&v;}
inline int16_t as_short(half v) {return *(int16_t*)&v;}

inline uint16_t as_ushort(int16_t v) {return *(uint16_t*)&v;}
inline uint16_t as_ushort(half v) {return *(uint16_t*)&v;}

inline int32_t as_int(uint32_t v) {return *(int32_t*)&v;}
inline int32_t as_int(float v) {return *(int32_t*)&v;}

inline uint32_t as_uint(int32_t v) {return *(uint32_t*)&v;}
inline uint32_t as_uint(float v) {return *(uint32_t*)&v;}

inline int64_t as_long(uint64_t v) {return *(int64_t*)&v;}
inline int64_t as_long(double v) {return *(int64_t*)&v;}

inline uint64_t as_ulong(int64_t v) {return *(uint64_t*)&v;}
inline uint64_t as_ulong(double v) {return *(uint64_t*)&v;}

inline half as_half(int16_t v) {return *(half*)&v;}
inline half as_half(uint16_t v) {return *(half*)&v;}

inline float as_float(int32_t v) {return *(float*)&v;}
inline float as_float(uint32_t v) {return *(float*)&v;}

inline double as_double(int64_t v) {return *(double*)&v;}
inline double as_double(uint64_t v) {return *(double*)&v;}


inline byte2 as_byte2(ubyte2 v) {return *(byte2*)&v;}

inline ubyte2 as_ubyte2(byte2 v) {return *(ubyte2*)&v;}

inline short2 as_short2(ushort2 v) {return *(short2*)&v;}
inline short2 as_short2(half2 v) {return *(short2*)&v;}

inline ushort2 as_ushort2(short2 v) {return *(ushort2*)&v;}
inline ushort2 as_ushort2(half2 v) {return *(ushort2*)&v;}

inline int2 as_int2(uint2 v) {return *(int2*)&v;}
inline int2 as_int2(float2 v) {return *(int2*)&v;}

inline uint2 as_uint2(int2 v) {return *(uint2*)&v;}
inline uint2 as_uint2(float2 v) {return *(uint2*)&v;}

inline long2 as_long2(ulong2 v) {return *(long2*)&v;}
inline long2 as_long2(double2 v) {return *(long2*)&v;}

inline ulong2 as_ulong2(long2 v) {return *(ulong2*)&v;}
inline ulong2 as_ulong2(double2 v) {return *(ulong2*)&v;}

inline half2 as_half2(short2 v) {return *(half2*)&v;}
inline half2 as_half2(ushort2 v) {return *(half2*)&v;}

inline float2 as_float2(int2 v) {return *(float2*)&v;}
inline float2 as_float2(uint2 v) {return *(float2*)&v;}

inline double2 as_double2(long2 v) {return *(double2*)&v;}
inline double2 as_double2(ulong2 v) {return *(double2*)&v;}


inline byte3 as_byte3(ubyte3 v) {return *(byte3*)&v;}

inline ubyte3 as_ubyte3(byte3 v) {return *(ubyte3*)&v;}

inline short3 as_short3(ushort3 v) {return *(short3*)&v;}
inline short3 as_short3(half3 v) {return *(short3*)&v;}

inline ushort3 as_ushort3(short3 v) {return *(ushort3*)&v;}
inline ushort3 as_ushort3(half3 v) {return *(ushort3*)&v;}

inline int3 as_int3(uint3 v) {return *(int3*)&v;}
inline int3 as_int3(float3 v) {return *(int3*)&v;}

inline uint3 as_uint3(int3 v) {return *(uint3*)&v;}
inline uint3 as_uint3(float3 v) {return *(uint3*)&v;}

inline long3 as_long3(ulong3 v) {return *(long3*)&v;}
inline long3 as_long3(double3 v) {return *(long3*)&v;}

inline ulong3 as_ulong3(long3 v) {return *(ulong3*)&v;}
inline ulong3 as_ulong3(double3 v) {return *(ulong3*)&v;}

inline half3 as_half3(short3 v) {return *(half3*)&v;}
inline half3 as_half3(ushort3 v) {return *(half3*)&v;}

inline float3 as_float3(int3 v) {return *(float3*)&v;}
inline float3 as_float3(uint3 v) {return *(float3*)&v;}

inline double3 as_double3(long3 v) {return *(double3*)&v;}
inline double3 as_double3(ulong3 v) {return *(double3*)&v;}


inline byte4 as_byte4(ubyte4 v) {return *(byte4*)&v;}

inline ubyte4 as_ubyte4(byte4 v) {return *(ubyte4*)&v;}

inline short4 as_short4(ushort4 v) {return *(short4*)&v;}
inline short4 as_short4(half4 v) {return *(short4*)&v;}

inline ushort4 as_ushort4(short4 v) {return *(ushort4*)&v;}
inline ushort4 as_ushort4(half4 v) {return *(ushort4*)&v;}

inline int4 as_int4(uint4 v) {return *(int4*)&v;}
inline int4 as_int4(float4 v) {return *(int4*)&v;}

inline uint4 as_uint4(int4 v) {return *(uint4*)&v;}
inline uint4 as_uint4(float4 v) {return *(uint4*)&v;}

inline long4 as_long4(ulong4 v) {return *(long4*)&v;}
inline long4 as_long4(double4 v) {return *(long4*)&v;}

inline ulong4 as_ulong4(long4 v) {return *(ulong4*)&v;}
inline ulong4 as_ulong4(double4 v) {return *(ulong4*)&v;}

inline half4 as_half4(short4 v) {return *(half4*)&v;}
inline half4 as_half4(ushort4 v) {return *(half4*)&v;}

inline float4 as_float4(int4 v) {return *(float4*)&v;}
inline float4 as_float4(uint4 v) {return *(float4*)&v;}

inline double4 as_double4(long4 v) {return *(double4*)&v;}
inline double4 as_double4(ulong4 v) {return *(double4*)&v;}


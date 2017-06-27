typedef Matrix2<float2> float2x2;
typedef Matrix2<double2> double2x2;

typedef Matrix3<float2> float2x3;
typedef Matrix3<double2> double2x3;

typedef Matrix4<float2> float2x4;
typedef Matrix4<double2> double2x4;

typedef Matrix2<float3> float3x2;
typedef Matrix2<double3> double3x2;

typedef Matrix3<float3> float3x3;
typedef Matrix3<double3> double3x3;

typedef Matrix4<float3> float3x4;
typedef Matrix4<double3> double3x4;

typedef Matrix2<float4> float4x2;
typedef Matrix2<double4> double4x2;

typedef Matrix3<float4> float4x3;
typedef Matrix3<double4> double4x3;

typedef Matrix4<float4> float4x4;
typedef Matrix4<double4> double4x4;

inline float2x2 convert_float2x2(const float2x2& m) {return m;}
inline float2x2 convert_float2x2(const double2x2& m) {return matrix2(convert_float2(m.x), convert_float2(m.y));}

inline double2x2 convert_double2x2(const float2x2& m) {return matrix2(convert_double2(m.x), convert_double2(m.y));}
inline double2x2 convert_double2x2(const double2x2& m) {return m;}


inline float2x3 convert_float2x3(const float2x3& m) {return m;}
inline float2x3 convert_float2x3(const double2x3& m) {return matrix3(convert_float2(m.x), convert_float2(m.y), convert_float2(m.z));}

inline double2x3 convert_double2x3(const float2x3& m) {return matrix3(convert_double2(m.x), convert_double2(m.y), convert_double2(m.z));}
inline double2x3 convert_double2x3(const double2x3& m) {return m;}


inline float2x4 convert_float2x4(const float2x4& m) {return m;}
inline float2x4 convert_float2x4(const double2x4& m) {return matrix4(convert_float2(m.x), convert_float2(m.y), convert_float2(m.z), convert_float2(m.w));}

inline double2x4 convert_double2x4(const float2x4& m) {return matrix4(convert_double2(m.x), convert_double2(m.y), convert_double2(m.z), convert_double2(m.w));}
inline double2x4 convert_double2x4(const double2x4& m) {return m;}


inline float3x2 convert_float3x2(const float3x2& m) {return m;}
inline float3x2 convert_float3x2(const double3x2& m) {return matrix2(convert_float3(m.x), convert_float3(m.y));}

inline double3x2 convert_double3x2(const float3x2& m) {return matrix2(convert_double3(m.x), convert_double3(m.y));}
inline double3x2 convert_double3x2(const double3x2& m) {return m;}


inline float3x3 convert_float3x3(const float3x3& m) {return m;}
inline float3x3 convert_float3x3(const double3x3& m) {return matrix3(convert_float3(m.x), convert_float3(m.y), convert_float3(m.z));}

inline double3x3 convert_double3x3(const float3x3& m) {return matrix3(convert_double3(m.x), convert_double3(m.y), convert_double3(m.z));}
inline double3x3 convert_double3x3(const double3x3& m) {return m;}


inline float3x4 convert_float3x4(const float3x4& m) {return m;}
inline float3x4 convert_float3x4(const double3x4& m) {return matrix4(convert_float3(m.x), convert_float3(m.y), convert_float3(m.z), convert_float3(m.w));}

inline double3x4 convert_double3x4(const float3x4& m) {return matrix4(convert_double3(m.x), convert_double3(m.y), convert_double3(m.z), convert_double3(m.w));}
inline double3x4 convert_double3x4(const double3x4& m) {return m;}


inline float4x2 convert_float4x2(const float4x2& m) {return m;}
inline float4x2 convert_float4x2(const double4x2& m) {return matrix2(convert_float4(m.x), convert_float4(m.y));}

inline double4x2 convert_double4x2(const float4x2& m) {return matrix2(convert_double4(m.x), convert_double4(m.y));}
inline double4x2 convert_double4x2(const double4x2& m) {return m;}


inline float4x3 convert_float4x3(const float4x3& m) {return m;}
inline float4x3 convert_float4x3(const double4x3& m) {return matrix3(convert_float4(m.x), convert_float4(m.y), convert_float4(m.z));}

inline double4x3 convert_double4x3(const float4x3& m) {return matrix3(convert_double4(m.x), convert_double4(m.y), convert_double4(m.z));}
inline double4x3 convert_double4x3(const double4x3& m) {return m;}


inline float4x4 convert_float4x4(const float4x4& m) {return m;}
inline float4x4 convert_float4x4(const double4x4& m) {return matrix4(convert_float4(m.x), convert_float4(m.y), convert_float4(m.z), convert_float4(m.w));}

inline double4x4 convert_double4x4(const float4x4& m) {return matrix4(convert_double4(m.x), convert_double4(m.y), convert_double4(m.z), convert_double4(m.w));}
inline double4x4 convert_double4x4(const double4x4& m) {return m;}



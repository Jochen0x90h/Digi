#define make_bool2(x, y) (bool2)((x), (y))
#define make_byte2(x, y) (byte2)((x), (y))
#define make_ubyte2(x, y) (ubyte2)((x), (y))
#define make_short2(x, y) (short2)((x), (y))
#define make_ushort2(x, y) (ushort2)((x), (y))
#define make_int2(x, y) (int2)((x), (y))
#define make_uint2(x, y) (uint2)((x), (y))
#define make_float2(x, y) (float2)((x), (y))

#define make_bool3(x, y, z) (bool3)((x), (y), (z))
#define make_byte3(x, y, z) (byte3)((x), (y), (z))
#define make_ubyte3(x, y, z) (ubyte3)((x), (y), (z))
#define make_short3(x, y, z) (short3)((x), (y), (z))
#define make_ushort3(x, y, z) (ushort3)((x), (y), (z))
#define make_int3(x, y, z) (int3)((x), (y), (z))
#define make_uint3(x, y, z) (uint3)((x), (y), (z))
#define make_float3(x, y, z) (float3)((x), (y), (z))

#define make_bool4(x, y, z, w) (bool4)((x), (y), (z), (w))
#define make_byte4(x, y, z, w) (byte4)((x), (y), (z), (w))
#define make_ubyte4(x, y, z, w) (ubyte4)((x), (y), (z), (w))
#define make_short4(x, y, z, w) (short4)((x), (y), (z), (w))
#define make_ushort4(x, y, z, w) (ushort4)((x), (y), (z), (w))
#define make_int4(x, y, z, w) (int4)((x), (y), (z), (w))
#define make_uint4(x, y, z, w) (uint4)((x), (y), (z), (w))
#define make_float4(x, y, z, w) (float4)((x), (y), (z), (w))


__attribute__((const)) byte convert_byte(bool v);
__attribute__((const)) byte convert_byte(byte v);
__attribute__((const)) byte convert_byte(ubyte v);
__attribute__((const)) byte convert_byte(short v);
__attribute__((const)) byte convert_byte(ushort v);
__attribute__((const)) byte convert_byte(int v);
__attribute__((const)) byte convert_byte(uint v);
__attribute__((const)) byte convert_byte(float v);

__attribute__((const)) ubyte convert_ubyte(bool v);
__attribute__((const)) ubyte convert_ubyte(byte v);
__attribute__((const)) ubyte convert_ubyte(ubyte v);
__attribute__((const)) ubyte convert_ubyte(short v);
__attribute__((const)) ubyte convert_ubyte(ushort v);
__attribute__((const)) ubyte convert_ubyte(int v);
__attribute__((const)) ubyte convert_ubyte(uint v);
__attribute__((const)) ubyte convert_ubyte(float v);

__attribute__((const)) short convert_short(bool v);
__attribute__((const)) short convert_short(byte v);
__attribute__((const)) short convert_short(ubyte v);
__attribute__((const)) short convert_short(short v);
__attribute__((const)) short convert_short(ushort v);
__attribute__((const)) short convert_short(int v);
__attribute__((const)) short convert_short(uint v);
__attribute__((const)) short convert_short(float v);

__attribute__((const)) ushort convert_ushort(bool v);
__attribute__((const)) ushort convert_ushort(byte v);
__attribute__((const)) ushort convert_ushort(ubyte v);
__attribute__((const)) ushort convert_ushort(short v);
__attribute__((const)) ushort convert_ushort(ushort v);
__attribute__((const)) ushort convert_ushort(int v);
__attribute__((const)) ushort convert_ushort(uint v);
__attribute__((const)) ushort convert_ushort(float v);

__attribute__((const)) int convert_int(bool v);
__attribute__((const)) int convert_int(byte v);
__attribute__((const)) int convert_int(ubyte v);
__attribute__((const)) int convert_int(short v);
__attribute__((const)) int convert_int(ushort v);
__attribute__((const)) int convert_int(int v);
__attribute__((const)) int convert_int(uint v);
__attribute__((const)) int convert_int(float v);

__attribute__((const)) uint convert_uint(bool v);
__attribute__((const)) uint convert_uint(byte v);
__attribute__((const)) uint convert_uint(ubyte v);
__attribute__((const)) uint convert_uint(short v);
__attribute__((const)) uint convert_uint(ushort v);
__attribute__((const)) uint convert_uint(int v);
__attribute__((const)) uint convert_uint(uint v);
__attribute__((const)) uint convert_uint(float v);

__attribute__((const)) float convert_float(bool v);
__attribute__((const)) float convert_float(byte v);
__attribute__((const)) float convert_float(ubyte v);
__attribute__((const)) float convert_float(short v);
__attribute__((const)) float convert_float(ushort v);
__attribute__((const)) float convert_float(int v);
__attribute__((const)) float convert_float(uint v);
__attribute__((const)) float convert_float(float v);



__attribute__((const)) byte2 convert_byte2(bool2 v);
__attribute__((const)) byte2 convert_byte2(byte2 v);
__attribute__((const)) byte2 convert_byte2(ubyte2 v);
__attribute__((const)) byte2 convert_byte2(short2 v);
__attribute__((const)) byte2 convert_byte2(ushort2 v);
__attribute__((const)) byte2 convert_byte2(int2 v);
__attribute__((const)) byte2 convert_byte2(uint2 v);
__attribute__((const)) byte2 convert_byte2(float2 v);

__attribute__((const)) ubyte2 convert_ubyte2(bool2 v);
__attribute__((const)) ubyte2 convert_ubyte2(byte2 v);
__attribute__((const)) ubyte2 convert_ubyte2(ubyte2 v);
__attribute__((const)) ubyte2 convert_ubyte2(short2 v);
__attribute__((const)) ubyte2 convert_ubyte2(ushort2 v);
__attribute__((const)) ubyte2 convert_ubyte2(int2 v);
__attribute__((const)) ubyte2 convert_ubyte2(uint2 v);
__attribute__((const)) ubyte2 convert_ubyte2(float2 v);

__attribute__((const)) short2 convert_short2(bool2 v);
__attribute__((const)) short2 convert_short2(byte2 v);
__attribute__((const)) short2 convert_short2(ubyte2 v);
__attribute__((const)) short2 convert_short2(short2 v);
__attribute__((const)) short2 convert_short2(ushort2 v);
__attribute__((const)) short2 convert_short2(int2 v);
__attribute__((const)) short2 convert_short2(uint2 v);
__attribute__((const)) short2 convert_short2(float2 v);

__attribute__((const)) ushort2 convert_ushort2(bool2 v);
__attribute__((const)) ushort2 convert_ushort2(byte2 v);
__attribute__((const)) ushort2 convert_ushort2(ubyte2 v);
__attribute__((const)) ushort2 convert_ushort2(short2 v);
__attribute__((const)) ushort2 convert_ushort2(ushort2 v);
__attribute__((const)) ushort2 convert_ushort2(int2 v);
__attribute__((const)) ushort2 convert_ushort2(uint2 v);
__attribute__((const)) ushort2 convert_ushort2(float2 v);

__attribute__((const)) int2 convert_int2(bool2 v);
__attribute__((const)) int2 convert_int2(byte2 v);
__attribute__((const)) int2 convert_int2(ubyte2 v);
__attribute__((const)) int2 convert_int2(short2 v);
__attribute__((const)) int2 convert_int2(ushort2 v);
__attribute__((const)) int2 convert_int2(int2 v);
__attribute__((const)) int2 convert_int2(uint2 v);
__attribute__((const)) int2 convert_int2(float2 v);

__attribute__((const)) uint2 convert_uint2(bool2 v);
__attribute__((const)) uint2 convert_uint2(byte2 v);
__attribute__((const)) uint2 convert_uint2(ubyte2 v);
__attribute__((const)) uint2 convert_uint2(short2 v);
__attribute__((const)) uint2 convert_uint2(ushort2 v);
__attribute__((const)) uint2 convert_uint2(int2 v);
__attribute__((const)) uint2 convert_uint2(uint2 v);
__attribute__((const)) uint2 convert_uint2(float2 v);

__attribute__((const)) float2 convert_float2(bool2 v);
__attribute__((const)) float2 convert_float2(byte2 v);
__attribute__((const)) float2 convert_float2(ubyte2 v);
__attribute__((const)) float2 convert_float2(short2 v);
__attribute__((const)) float2 convert_float2(ushort2 v);
__attribute__((const)) float2 convert_float2(int2 v);
__attribute__((const)) float2 convert_float2(uint2 v);
__attribute__((const)) float2 convert_float2(float2 v);



__attribute__((const)) byte3 convert_byte3(bool3 v);
__attribute__((const)) byte3 convert_byte3(byte3 v);
__attribute__((const)) byte3 convert_byte3(ubyte3 v);
__attribute__((const)) byte3 convert_byte3(short3 v);
__attribute__((const)) byte3 convert_byte3(ushort3 v);
__attribute__((const)) byte3 convert_byte3(int3 v);
__attribute__((const)) byte3 convert_byte3(uint3 v);
__attribute__((const)) byte3 convert_byte3(float3 v);

__attribute__((const)) ubyte3 convert_ubyte3(bool3 v);
__attribute__((const)) ubyte3 convert_ubyte3(byte3 v);
__attribute__((const)) ubyte3 convert_ubyte3(ubyte3 v);
__attribute__((const)) ubyte3 convert_ubyte3(short3 v);
__attribute__((const)) ubyte3 convert_ubyte3(ushort3 v);
__attribute__((const)) ubyte3 convert_ubyte3(int3 v);
__attribute__((const)) ubyte3 convert_ubyte3(uint3 v);
__attribute__((const)) ubyte3 convert_ubyte3(float3 v);

__attribute__((const)) short3 convert_short3(bool3 v);
__attribute__((const)) short3 convert_short3(byte3 v);
__attribute__((const)) short3 convert_short3(ubyte3 v);
__attribute__((const)) short3 convert_short3(short3 v);
__attribute__((const)) short3 convert_short3(ushort3 v);
__attribute__((const)) short3 convert_short3(int3 v);
__attribute__((const)) short3 convert_short3(uint3 v);
__attribute__((const)) short3 convert_short3(float3 v);

__attribute__((const)) ushort3 convert_ushort3(bool3 v);
__attribute__((const)) ushort3 convert_ushort3(byte3 v);
__attribute__((const)) ushort3 convert_ushort3(ubyte3 v);
__attribute__((const)) ushort3 convert_ushort3(short3 v);
__attribute__((const)) ushort3 convert_ushort3(ushort3 v);
__attribute__((const)) ushort3 convert_ushort3(int3 v);
__attribute__((const)) ushort3 convert_ushort3(uint3 v);
__attribute__((const)) ushort3 convert_ushort3(float3 v);

__attribute__((const)) int3 convert_int3(bool3 v);
__attribute__((const)) int3 convert_int3(byte3 v);
__attribute__((const)) int3 convert_int3(ubyte3 v);
__attribute__((const)) int3 convert_int3(short3 v);
__attribute__((const)) int3 convert_int3(ushort3 v);
__attribute__((const)) int3 convert_int3(int3 v);
__attribute__((const)) int3 convert_int3(uint3 v);
__attribute__((const)) int3 convert_int3(float3 v);

__attribute__((const)) uint3 convert_uint3(bool3 v);
__attribute__((const)) uint3 convert_uint3(byte3 v);
__attribute__((const)) uint3 convert_uint3(ubyte3 v);
__attribute__((const)) uint3 convert_uint3(short3 v);
__attribute__((const)) uint3 convert_uint3(ushort3 v);
__attribute__((const)) uint3 convert_uint3(int3 v);
__attribute__((const)) uint3 convert_uint3(uint3 v);
__attribute__((const)) uint3 convert_uint3(float3 v);

__attribute__((const)) float3 convert_float3(bool3 v);
__attribute__((const)) float3 convert_float3(byte3 v);
__attribute__((const)) float3 convert_float3(ubyte3 v);
__attribute__((const)) float3 convert_float3(short3 v);
__attribute__((const)) float3 convert_float3(ushort3 v);
__attribute__((const)) float3 convert_float3(int3 v);
__attribute__((const)) float3 convert_float3(uint3 v);
__attribute__((const)) float3 convert_float3(float3 v);



__attribute__((const)) byte4 convert_byte4(bool4 v);
__attribute__((const)) byte4 convert_byte4(byte4 v);
__attribute__((const)) byte4 convert_byte4(ubyte4 v);
__attribute__((const)) byte4 convert_byte4(short4 v);
__attribute__((const)) byte4 convert_byte4(ushort4 v);
__attribute__((const)) byte4 convert_byte4(int4 v);
__attribute__((const)) byte4 convert_byte4(uint4 v);
__attribute__((const)) byte4 convert_byte4(float4 v);

__attribute__((const)) ubyte4 convert_ubyte4(bool4 v);
__attribute__((const)) ubyte4 convert_ubyte4(byte4 v);
__attribute__((const)) ubyte4 convert_ubyte4(ubyte4 v);
__attribute__((const)) ubyte4 convert_ubyte4(short4 v);
__attribute__((const)) ubyte4 convert_ubyte4(ushort4 v);
__attribute__((const)) ubyte4 convert_ubyte4(int4 v);
__attribute__((const)) ubyte4 convert_ubyte4(uint4 v);
__attribute__((const)) ubyte4 convert_ubyte4(float4 v);

__attribute__((const)) short4 convert_short4(bool4 v);
__attribute__((const)) short4 convert_short4(byte4 v);
__attribute__((const)) short4 convert_short4(ubyte4 v);
__attribute__((const)) short4 convert_short4(short4 v);
__attribute__((const)) short4 convert_short4(ushort4 v);
__attribute__((const)) short4 convert_short4(int4 v);
__attribute__((const)) short4 convert_short4(uint4 v);
__attribute__((const)) short4 convert_short4(float4 v);

__attribute__((const)) ushort4 convert_ushort4(bool4 v);
__attribute__((const)) ushort4 convert_ushort4(byte4 v);
__attribute__((const)) ushort4 convert_ushort4(ubyte4 v);
__attribute__((const)) ushort4 convert_ushort4(short4 v);
__attribute__((const)) ushort4 convert_ushort4(ushort4 v);
__attribute__((const)) ushort4 convert_ushort4(int4 v);
__attribute__((const)) ushort4 convert_ushort4(uint4 v);
__attribute__((const)) ushort4 convert_ushort4(float4 v);

__attribute__((const)) int4 convert_int4(bool4 v);
__attribute__((const)) int4 convert_int4(byte4 v);
__attribute__((const)) int4 convert_int4(ubyte4 v);
__attribute__((const)) int4 convert_int4(short4 v);
__attribute__((const)) int4 convert_int4(ushort4 v);
__attribute__((const)) int4 convert_int4(int4 v);
__attribute__((const)) int4 convert_int4(uint4 v);
__attribute__((const)) int4 convert_int4(float4 v);

__attribute__((const)) uint4 convert_uint4(bool4 v);
__attribute__((const)) uint4 convert_uint4(byte4 v);
__attribute__((const)) uint4 convert_uint4(ubyte4 v);
__attribute__((const)) uint4 convert_uint4(short4 v);
__attribute__((const)) uint4 convert_uint4(ushort4 v);
__attribute__((const)) uint4 convert_uint4(int4 v);
__attribute__((const)) uint4 convert_uint4(uint4 v);
__attribute__((const)) uint4 convert_uint4(float4 v);

__attribute__((const)) float4 convert_float4(bool4 v);
__attribute__((const)) float4 convert_float4(byte4 v);
__attribute__((const)) float4 convert_float4(ubyte4 v);
__attribute__((const)) float4 convert_float4(short4 v);
__attribute__((const)) float4 convert_float4(ushort4 v);
__attribute__((const)) float4 convert_float4(int4 v);
__attribute__((const)) float4 convert_float4(uint4 v);
__attribute__((const)) float4 convert_float4(float4 v);



__attribute__((const)) byte convert_byte_rte(float v);

__attribute__((const)) ubyte convert_ubyte_rte(float v);

__attribute__((const)) short convert_short_rte(float v);

__attribute__((const)) ushort convert_ushort_rte(float v);

__attribute__((const)) int convert_int_rte(float v);

__attribute__((const)) uint convert_uint_rte(float v);




__attribute__((const)) byte2 convert_byte2_rte(float2 v);

__attribute__((const)) ubyte2 convert_ubyte2_rte(float2 v);

__attribute__((const)) short2 convert_short2_rte(float2 v);

__attribute__((const)) ushort2 convert_ushort2_rte(float2 v);

__attribute__((const)) int2 convert_int2_rte(float2 v);

__attribute__((const)) uint2 convert_uint2_rte(float2 v);




__attribute__((const)) byte3 convert_byte3_rte(float3 v);

__attribute__((const)) ubyte3 convert_ubyte3_rte(float3 v);

__attribute__((const)) short3 convert_short3_rte(float3 v);

__attribute__((const)) ushort3 convert_ushort3_rte(float3 v);

__attribute__((const)) int3 convert_int3_rte(float3 v);

__attribute__((const)) uint3 convert_uint3_rte(float3 v);




__attribute__((const)) byte4 convert_byte4_rte(float4 v);

__attribute__((const)) ubyte4 convert_ubyte4_rte(float4 v);

__attribute__((const)) short4 convert_short4_rte(float4 v);

__attribute__((const)) ushort4 convert_ushort4_rte(float4 v);

__attribute__((const)) int4 convert_int4_rte(float4 v);

__attribute__((const)) uint4 convert_uint4_rte(float4 v);




__attribute__((const)) byte convert_byte_sat(byte v);
__attribute__((const)) byte convert_byte_sat(ubyte v);
__attribute__((const)) byte convert_byte_sat(short v);
__attribute__((const)) byte convert_byte_sat(ushort v);
__attribute__((const)) byte convert_byte_sat(int v);
__attribute__((const)) byte convert_byte_sat(uint v);
__attribute__((const)) byte convert_byte_sat(float v);

__attribute__((const)) ubyte convert_ubyte_sat(byte v);
__attribute__((const)) ubyte convert_ubyte_sat(ubyte v);
__attribute__((const)) ubyte convert_ubyte_sat(short v);
__attribute__((const)) ubyte convert_ubyte_sat(ushort v);
__attribute__((const)) ubyte convert_ubyte_sat(int v);
__attribute__((const)) ubyte convert_ubyte_sat(uint v);
__attribute__((const)) ubyte convert_ubyte_sat(float v);

__attribute__((const)) short convert_short_sat(byte v);
__attribute__((const)) short convert_short_sat(ubyte v);
__attribute__((const)) short convert_short_sat(short v);
__attribute__((const)) short convert_short_sat(ushort v);
__attribute__((const)) short convert_short_sat(int v);
__attribute__((const)) short convert_short_sat(uint v);
__attribute__((const)) short convert_short_sat(float v);

__attribute__((const)) ushort convert_ushort_sat(byte v);
__attribute__((const)) ushort convert_ushort_sat(ubyte v);
__attribute__((const)) ushort convert_ushort_sat(short v);
__attribute__((const)) ushort convert_ushort_sat(ushort v);
__attribute__((const)) ushort convert_ushort_sat(int v);
__attribute__((const)) ushort convert_ushort_sat(uint v);
__attribute__((const)) ushort convert_ushort_sat(float v);

__attribute__((const)) int convert_int_sat(byte v);
__attribute__((const)) int convert_int_sat(ubyte v);
__attribute__((const)) int convert_int_sat(short v);
__attribute__((const)) int convert_int_sat(ushort v);
__attribute__((const)) int convert_int_sat(int v);
__attribute__((const)) int convert_int_sat(uint v);
__attribute__((const)) int convert_int_sat(float v);

__attribute__((const)) uint convert_uint_sat(byte v);
__attribute__((const)) uint convert_uint_sat(ubyte v);
__attribute__((const)) uint convert_uint_sat(short v);
__attribute__((const)) uint convert_uint_sat(ushort v);
__attribute__((const)) uint convert_uint_sat(int v);
__attribute__((const)) uint convert_uint_sat(uint v);
__attribute__((const)) uint convert_uint_sat(float v);

__attribute__((const)) float convert_float_sat(byte v);
__attribute__((const)) float convert_float_sat(ubyte v);
__attribute__((const)) float convert_float_sat(short v);
__attribute__((const)) float convert_float_sat(ushort v);
__attribute__((const)) float convert_float_sat(int v);
__attribute__((const)) float convert_float_sat(uint v);
__attribute__((const)) float convert_float_sat(float v);



__attribute__((const)) byte2 convert_byte2_sat(byte2 v);
__attribute__((const)) byte2 convert_byte2_sat(ubyte2 v);
__attribute__((const)) byte2 convert_byte2_sat(short2 v);
__attribute__((const)) byte2 convert_byte2_sat(ushort2 v);
__attribute__((const)) byte2 convert_byte2_sat(int2 v);
__attribute__((const)) byte2 convert_byte2_sat(uint2 v);
__attribute__((const)) byte2 convert_byte2_sat(float2 v);

__attribute__((const)) ubyte2 convert_ubyte2_sat(byte2 v);
__attribute__((const)) ubyte2 convert_ubyte2_sat(ubyte2 v);
__attribute__((const)) ubyte2 convert_ubyte2_sat(short2 v);
__attribute__((const)) ubyte2 convert_ubyte2_sat(ushort2 v);
__attribute__((const)) ubyte2 convert_ubyte2_sat(int2 v);
__attribute__((const)) ubyte2 convert_ubyte2_sat(uint2 v);
__attribute__((const)) ubyte2 convert_ubyte2_sat(float2 v);

__attribute__((const)) short2 convert_short2_sat(byte2 v);
__attribute__((const)) short2 convert_short2_sat(ubyte2 v);
__attribute__((const)) short2 convert_short2_sat(short2 v);
__attribute__((const)) short2 convert_short2_sat(ushort2 v);
__attribute__((const)) short2 convert_short2_sat(int2 v);
__attribute__((const)) short2 convert_short2_sat(uint2 v);
__attribute__((const)) short2 convert_short2_sat(float2 v);

__attribute__((const)) ushort2 convert_ushort2_sat(byte2 v);
__attribute__((const)) ushort2 convert_ushort2_sat(ubyte2 v);
__attribute__((const)) ushort2 convert_ushort2_sat(short2 v);
__attribute__((const)) ushort2 convert_ushort2_sat(ushort2 v);
__attribute__((const)) ushort2 convert_ushort2_sat(int2 v);
__attribute__((const)) ushort2 convert_ushort2_sat(uint2 v);
__attribute__((const)) ushort2 convert_ushort2_sat(float2 v);

__attribute__((const)) int2 convert_int2_sat(byte2 v);
__attribute__((const)) int2 convert_int2_sat(ubyte2 v);
__attribute__((const)) int2 convert_int2_sat(short2 v);
__attribute__((const)) int2 convert_int2_sat(ushort2 v);
__attribute__((const)) int2 convert_int2_sat(int2 v);
__attribute__((const)) int2 convert_int2_sat(uint2 v);
__attribute__((const)) int2 convert_int2_sat(float2 v);

__attribute__((const)) uint2 convert_uint2_sat(byte2 v);
__attribute__((const)) uint2 convert_uint2_sat(ubyte2 v);
__attribute__((const)) uint2 convert_uint2_sat(short2 v);
__attribute__((const)) uint2 convert_uint2_sat(ushort2 v);
__attribute__((const)) uint2 convert_uint2_sat(int2 v);
__attribute__((const)) uint2 convert_uint2_sat(uint2 v);
__attribute__((const)) uint2 convert_uint2_sat(float2 v);

__attribute__((const)) float2 convert_float2_sat(byte2 v);
__attribute__((const)) float2 convert_float2_sat(ubyte2 v);
__attribute__((const)) float2 convert_float2_sat(short2 v);
__attribute__((const)) float2 convert_float2_sat(ushort2 v);
__attribute__((const)) float2 convert_float2_sat(int2 v);
__attribute__((const)) float2 convert_float2_sat(uint2 v);
__attribute__((const)) float2 convert_float2_sat(float2 v);



__attribute__((const)) byte3 convert_byte3_sat(byte3 v);
__attribute__((const)) byte3 convert_byte3_sat(ubyte3 v);
__attribute__((const)) byte3 convert_byte3_sat(short3 v);
__attribute__((const)) byte3 convert_byte3_sat(ushort3 v);
__attribute__((const)) byte3 convert_byte3_sat(int3 v);
__attribute__((const)) byte3 convert_byte3_sat(uint3 v);
__attribute__((const)) byte3 convert_byte3_sat(float3 v);

__attribute__((const)) ubyte3 convert_ubyte3_sat(byte3 v);
__attribute__((const)) ubyte3 convert_ubyte3_sat(ubyte3 v);
__attribute__((const)) ubyte3 convert_ubyte3_sat(short3 v);
__attribute__((const)) ubyte3 convert_ubyte3_sat(ushort3 v);
__attribute__((const)) ubyte3 convert_ubyte3_sat(int3 v);
__attribute__((const)) ubyte3 convert_ubyte3_sat(uint3 v);
__attribute__((const)) ubyte3 convert_ubyte3_sat(float3 v);

__attribute__((const)) short3 convert_short3_sat(byte3 v);
__attribute__((const)) short3 convert_short3_sat(ubyte3 v);
__attribute__((const)) short3 convert_short3_sat(short3 v);
__attribute__((const)) short3 convert_short3_sat(ushort3 v);
__attribute__((const)) short3 convert_short3_sat(int3 v);
__attribute__((const)) short3 convert_short3_sat(uint3 v);
__attribute__((const)) short3 convert_short3_sat(float3 v);

__attribute__((const)) ushort3 convert_ushort3_sat(byte3 v);
__attribute__((const)) ushort3 convert_ushort3_sat(ubyte3 v);
__attribute__((const)) ushort3 convert_ushort3_sat(short3 v);
__attribute__((const)) ushort3 convert_ushort3_sat(ushort3 v);
__attribute__((const)) ushort3 convert_ushort3_sat(int3 v);
__attribute__((const)) ushort3 convert_ushort3_sat(uint3 v);
__attribute__((const)) ushort3 convert_ushort3_sat(float3 v);

__attribute__((const)) int3 convert_int3_sat(byte3 v);
__attribute__((const)) int3 convert_int3_sat(ubyte3 v);
__attribute__((const)) int3 convert_int3_sat(short3 v);
__attribute__((const)) int3 convert_int3_sat(ushort3 v);
__attribute__((const)) int3 convert_int3_sat(int3 v);
__attribute__((const)) int3 convert_int3_sat(uint3 v);
__attribute__((const)) int3 convert_int3_sat(float3 v);

__attribute__((const)) uint3 convert_uint3_sat(byte3 v);
__attribute__((const)) uint3 convert_uint3_sat(ubyte3 v);
__attribute__((const)) uint3 convert_uint3_sat(short3 v);
__attribute__((const)) uint3 convert_uint3_sat(ushort3 v);
__attribute__((const)) uint3 convert_uint3_sat(int3 v);
__attribute__((const)) uint3 convert_uint3_sat(uint3 v);
__attribute__((const)) uint3 convert_uint3_sat(float3 v);

__attribute__((const)) float3 convert_float3_sat(byte3 v);
__attribute__((const)) float3 convert_float3_sat(ubyte3 v);
__attribute__((const)) float3 convert_float3_sat(short3 v);
__attribute__((const)) float3 convert_float3_sat(ushort3 v);
__attribute__((const)) float3 convert_float3_sat(int3 v);
__attribute__((const)) float3 convert_float3_sat(uint3 v);
__attribute__((const)) float3 convert_float3_sat(float3 v);



__attribute__((const)) byte4 convert_byte4_sat(byte4 v);
__attribute__((const)) byte4 convert_byte4_sat(ubyte4 v);
__attribute__((const)) byte4 convert_byte4_sat(short4 v);
__attribute__((const)) byte4 convert_byte4_sat(ushort4 v);
__attribute__((const)) byte4 convert_byte4_sat(int4 v);
__attribute__((const)) byte4 convert_byte4_sat(uint4 v);
__attribute__((const)) byte4 convert_byte4_sat(float4 v);

__attribute__((const)) ubyte4 convert_ubyte4_sat(byte4 v);
__attribute__((const)) ubyte4 convert_ubyte4_sat(ubyte4 v);
__attribute__((const)) ubyte4 convert_ubyte4_sat(short4 v);
__attribute__((const)) ubyte4 convert_ubyte4_sat(ushort4 v);
__attribute__((const)) ubyte4 convert_ubyte4_sat(int4 v);
__attribute__((const)) ubyte4 convert_ubyte4_sat(uint4 v);
__attribute__((const)) ubyte4 convert_ubyte4_sat(float4 v);

__attribute__((const)) short4 convert_short4_sat(byte4 v);
__attribute__((const)) short4 convert_short4_sat(ubyte4 v);
__attribute__((const)) short4 convert_short4_sat(short4 v);
__attribute__((const)) short4 convert_short4_sat(ushort4 v);
__attribute__((const)) short4 convert_short4_sat(int4 v);
__attribute__((const)) short4 convert_short4_sat(uint4 v);
__attribute__((const)) short4 convert_short4_sat(float4 v);

__attribute__((const)) ushort4 convert_ushort4_sat(byte4 v);
__attribute__((const)) ushort4 convert_ushort4_sat(ubyte4 v);
__attribute__((const)) ushort4 convert_ushort4_sat(short4 v);
__attribute__((const)) ushort4 convert_ushort4_sat(ushort4 v);
__attribute__((const)) ushort4 convert_ushort4_sat(int4 v);
__attribute__((const)) ushort4 convert_ushort4_sat(uint4 v);
__attribute__((const)) ushort4 convert_ushort4_sat(float4 v);

__attribute__((const)) int4 convert_int4_sat(byte4 v);
__attribute__((const)) int4 convert_int4_sat(ubyte4 v);
__attribute__((const)) int4 convert_int4_sat(short4 v);
__attribute__((const)) int4 convert_int4_sat(ushort4 v);
__attribute__((const)) int4 convert_int4_sat(int4 v);
__attribute__((const)) int4 convert_int4_sat(uint4 v);
__attribute__((const)) int4 convert_int4_sat(float4 v);

__attribute__((const)) uint4 convert_uint4_sat(byte4 v);
__attribute__((const)) uint4 convert_uint4_sat(ubyte4 v);
__attribute__((const)) uint4 convert_uint4_sat(short4 v);
__attribute__((const)) uint4 convert_uint4_sat(ushort4 v);
__attribute__((const)) uint4 convert_uint4_sat(int4 v);
__attribute__((const)) uint4 convert_uint4_sat(uint4 v);
__attribute__((const)) uint4 convert_uint4_sat(float4 v);

__attribute__((const)) float4 convert_float4_sat(byte4 v);
__attribute__((const)) float4 convert_float4_sat(ubyte4 v);
__attribute__((const)) float4 convert_float4_sat(short4 v);
__attribute__((const)) float4 convert_float4_sat(ushort4 v);
__attribute__((const)) float4 convert_float4_sat(int4 v);
__attribute__((const)) float4 convert_float4_sat(uint4 v);
__attribute__((const)) float4 convert_float4_sat(float4 v);



__attribute__((const)) byte convert_byte_sat_rte(float v);

__attribute__((const)) ubyte convert_ubyte_sat_rte(float v);

__attribute__((const)) short convert_short_sat_rte(float v);

__attribute__((const)) ushort convert_ushort_sat_rte(float v);

__attribute__((const)) int convert_int_sat_rte(float v);

__attribute__((const)) uint convert_uint_sat_rte(float v);




__attribute__((const)) byte2 convert_byte2_sat_rte(float2 v);

__attribute__((const)) ubyte2 convert_ubyte2_sat_rte(float2 v);

__attribute__((const)) short2 convert_short2_sat_rte(float2 v);

__attribute__((const)) ushort2 convert_ushort2_sat_rte(float2 v);

__attribute__((const)) int2 convert_int2_sat_rte(float2 v);

__attribute__((const)) uint2 convert_uint2_sat_rte(float2 v);




__attribute__((const)) byte3 convert_byte3_sat_rte(float3 v);

__attribute__((const)) ubyte3 convert_ubyte3_sat_rte(float3 v);

__attribute__((const)) short3 convert_short3_sat_rte(float3 v);

__attribute__((const)) ushort3 convert_ushort3_sat_rte(float3 v);

__attribute__((const)) int3 convert_int3_sat_rte(float3 v);

__attribute__((const)) uint3 convert_uint3_sat_rte(float3 v);




__attribute__((const)) byte4 convert_byte4_sat_rte(float4 v);

__attribute__((const)) ubyte4 convert_ubyte4_sat_rte(float4 v);

__attribute__((const)) short4 convert_short4_sat_rte(float4 v);

__attribute__((const)) ushort4 convert_ushort4_sat_rte(float4 v);

__attribute__((const)) int4 convert_int4_sat_rte(float4 v);

__attribute__((const)) uint4 convert_uint4_sat_rte(float4 v);



__attribute__((const)) float as_float(int v);
__attribute__((const)) float as_float(uint v);
__attribute__((const)) int as_int(float v);
__attribute__((const)) uint as_uint(float v);

__attribute__((const)) float2 as_float2(int2 v);
__attribute__((const)) float2 as_float2(uint2 v);
__attribute__((const)) int2 as_int2(float2 v);
__attribute__((const)) uint2 as_uint2(float2 v);

__attribute__((const)) float3 as_float3(int3 v);
__attribute__((const)) float3 as_float3(uint3 v);
__attribute__((const)) int3 as_int3(float3 v);
__attribute__((const)) uint3 as_uint3(float3 v);

__attribute__((const)) float4 as_float4(int4 v);
__attribute__((const)) float4 as_float4(uint4 v);
__attribute__((const)) int4 as_int4(float4 v);
__attribute__((const)) uint4 as_uint4(float4 v);


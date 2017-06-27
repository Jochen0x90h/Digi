extern "C" void _convert(ubyte* srcData, size_t srcStride, void* global, ubyte* dstData, size_t dstStride, size_t numElements)
{
	while (numElements > 0)
	{
		float2 a = *(packed_float2*)srcData;
		srcData += srcStride;
		float4 b = vector4(a, 0.0f, 1.0f);
		ushort2 c = convert_ushort2_sat_rte(b.xy * 65535.0f);
		ushort d = c.x;
		dstData[0] = d;
		dstData[1] = d >> 8;
		ushort e = c.y;
		dstData[2] = e;
		dstData[3] = e >> 8;
		dstData += dstStride;
		--numElements;
	}
}
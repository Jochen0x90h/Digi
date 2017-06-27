extern "C" void _convert(ubyte* srcData, size_t srcStride, void* global, ubyte* dstData, size_t dstStride, size_t numElements)
{
	while (numElements > 0)
	{
		ubyte4 a = *(packed_ubyte4*)srcData;
		srcData += srcStride;
		*(packed_ubyte4*)dstData = a;
		dstData += dstStride;
		--numElements;
	}
}
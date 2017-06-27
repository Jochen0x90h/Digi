extern "C" { 

__attribute__((const)) float eSTF(const float* xValues, const float* keys, int numKeys, float x);
__attribute__((const)) float eBTF(const float* xValues, const float* keys, int numKeys, float x);
__attribute__((const)) float eWBTF(const float* xValues, const float* keys, int numKeys, float x);
__attribute__((const)) float eCTF(const float* keys, float x);

__attribute__((const)) float eSTS(const ushort* xValues, const ushort* keys, int numKeys, float x);
__attribute__((const)) float eBTS(const ushort* xValues, const ushort* keys, int numKeys, float x);
__attribute__((const)) float eWBTS(const ushort* xValues, const ushort* keys, int numKeys, float x);
__attribute__((const)) float eCTS(const ushort* keys, float x);

}

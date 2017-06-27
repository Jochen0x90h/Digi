// eSTF
BIND_C_FUNCTION(module, "eSTF", (float (*)(const float *, const float *, int, float))evalStepTrack);

// eBTF
BIND_C_FUNCTION(module, "eBTF", (float (*)(const float *, const float *, int, float))evalBezierTrack);

// eWBTF
BIND_C_FUNCTION(module, "eWBTF", (float (*)(const float *, const float *, int, float))evalWeightedBezierTrack);

// eCTF
BIND_C_FUNCTION(module, "eCTF", (float (*)(const float *, float))evalCatmullRomTrack);

// eSTS
BIND_C_FUNCTION(module, "eSTS", (float (*)(const ushort *, const ushort *, int, float))evalStepTrack);

// eBTS
BIND_C_FUNCTION(module, "eBTS", (float (*)(const ushort *, const ushort *, int, float))evalBezierTrack);

// eWBTS
BIND_C_FUNCTION(module, "eWBTS", (float (*)(const ushort *, const ushort *, int, float))evalWeightedBezierTrack);

// eCTS
BIND_C_FUNCTION(module, "eCTS", (float (*)(const ushort *, float))evalCatmullRomTrack);


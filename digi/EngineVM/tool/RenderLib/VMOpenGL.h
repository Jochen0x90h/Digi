#include "stdlib.h"
#include "gl.h"
#include "GLWrapper.h"

#include "math.h"
inline float fabs(float x) {return (float)fabs((double)x);}
inline float floor(float x) {return (float)floor((double)x);}
inline float ceil(float x) {return (float)ceil((double)x);}
inline float round(float x) {return (float)round((double)x);}
inline float trunc(float x) {return (float)trunc((double)x);}
inline float fmod(float x, float y) {return (float)fmod((double)x, (double)y);}
inline float sqrt(float x) {return sqrtf(x);}
inline float exp(float x) {return (float)exp((double)x);}
inline float pow(float x, float n) {return (float)pow((double)x, (double)n);}
inline float log(float x) {return (float)log((double)x);}
inline float log10(float x) {return (float)log10((double)x);}
inline float sin(float x) {return sinf(x);}
inline float cos(float x) {return cosf(x);}
inline float tan(float x) {return (float)tan((double)x);}
inline float asin(float x) {return (float)asin((double)x);}
inline float acos(float x) {return (float)acos((double)x);}
inline float atan(float x) {return (float)atan((double)x);}
inline float atan2(float y, float x) {return (float)atan2((double)y, (double)x);}

#include "VectorMath.h"
#include "Track.h"
#include "Random.h"
#include "Noise.h"

#include "Intersection.h"
#include "RenderTypes.h"
#include "RenderJob.h"

typedef const char* string;

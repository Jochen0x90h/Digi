#ifndef digi_Engine_Text_h
#define digi_Engine_Text_h

#include <digi/Utility/Standard.h>
#include <digi/Math/All.h>


namespace digi {

/// @addtogroup Engine
/// @{
	
typedef const char* string;

struct TextSymbols
{
	float uv2posX;
	float uv2posY;
	
	// number of symbols
	int numSymbols;
	
	// index into strings for each symbol
	uint16_t* indices;
	
	// null terminated string for each symbol
	uint8_t* strings;

	// rectangles for symbols on font texture
	float* rects;
};

struct TextVertex
{
	float3 position;
	
	//!
	float3 normal;
	float3 tangent;
	float3 bitangent;
	
	float2 symbol;
};

struct TextData
{
	int numSymbols;
	TextVertex* vertices;
	float3 center;
	float3 size;
};

void text2symbols(float2 scale, float3 space, float4 tab, int2 align, string text,
	TextSymbols& symbols, int maxNumSymbols, TextData& data);

/// @}

} // namespace digi

#endif

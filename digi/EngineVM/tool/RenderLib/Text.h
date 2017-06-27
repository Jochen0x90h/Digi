//! todo: this is not included yet. how to handle text2symbols?

typedef const char* string;

struct TextSymbols
{
	float uv2posX;
	float uv2posY;
	
	// number of symbols
	int numSymbols;
	
	// index into strings for each symbol
	ushort* indices;
	
	// null terminated string for each symbol
	ubyte* strings;

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

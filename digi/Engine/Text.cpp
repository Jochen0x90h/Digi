#include "Text.h"


namespace digi {

namespace
{
	int find(TextSymbols& symbols, int low, int high, int len, uint8_t ch)
	{
		while (true)
		{
			int mid = (low + high) >> 1;
			ushort* it = symbols.indices + mid;
			if (low >= high)
				return mid;
	
			if (symbols.strings[*it + len] >= ch)
				high = mid;
			else
				low = mid + 1;
		}
		
	}
}

void text2symbols(float2 scale, float3 space, float4 column, int2 align, string text,
	TextSymbols& symbols, int maxNumSymbols, TextData& data)
{
	float spaceWidth = space.x * scale.x;
	float letterSpacing = space.y * scale.x;
	float lineHeight = space.z * scale.y;
	float columns[4];
	columns[0] = column.x * scale.x;
	columns[1] = column.y * scale.x + columns[0];
	columns[2] = column.z * scale.x + columns[1];
	columns[3] = column.w * scale.x + columns[2];
	int alignH = align.x;
	int alignV = align.y;
	float uv2posX = scale.x * symbols.uv2posX;
	float uv2posY = scale.y * symbols.uv2posY;
	
	// 1st pass: determine size
	const char* it = text;
	int numSymbols = 0;
	float x = 0;
	float y = 0;
	int columnIndex = 0;
	float maxX = 0;
	while (*it != 0 && numSymbols < maxNumSymbols)
	{
		if (uint8_t(*it) <= 32)
		{
			// special characters
			if (*it == ' ')
			{
				// space
				x += spaceWidth;
			}
			else if (*it == '\t')
			{
				// tab
				x = columns[columnIndex];
				columnIndex = (columnIndex + 1) & 3;
			}
			else if (*it == '\n')
			{
				// new line
				x = 0;
				y -= lineHeight;
				columnIndex = 0;
			}
			++it;
		}
		else
		{
			float sx;
			float sy;
			float sw;
			float sh;
			float w;
			float h;
			if (symbols.numSymbols == 0)
			{
				// no symbols: generate dummy quad
				sx = 0.0f;
				sy = 0.0f;
				sw = 1.0f;
				sh = 1.0f;
				w = scale.x;
				h = scale.y;
				++it;
			}
			else
			{
				// find symbol (may consist of multiple chars e.g. ligature 'ft' or utf8-encoded char)
				int low = 0;
				int high = symbols.numSymbols;
				int len = 0;
				int found = -1;
				while (*it != 0)
				{
					low = find(symbols, low, high, len, *it);
					high = find(symbols, low, high, len, *it + 1);

					if (low == high)
						break;
						
					found = low;
					++it;
					++len;
				}
				
				if (found == -1 || symbols.strings[symbols.indices[found] + len] != 0)
				{
					// symbol not found: use first symbol
					found = 0;
					if (len == 0)
						++it;
				}

				// get texture rectangle and size
				float* s = &symbols.rects[found * 4];
				sx = s[0];
				sy = s[1];
				sw = s[2];
				sh = s[3];
				w = sw * uv2posX;
				h = sh * uv2posY;
			}
			
			// advance x position
			x += w + letterSpacing;
						
			++numSymbols;
		}
		maxX = max(maxX, x);
	}
	float minY = y - scale.y;

	// align bounding box
	float offsetX = alignH == 0 ? 0.0f : -(alignH == 1 ? 0.5f * maxX : maxX);
	float offsetY = alignV == 0 ? 0.0f : -(alignV == 1 ? 0.5f * minY : minY);

	// 2nd pass: generate geometry
	it = text;
	numSymbols = 0;
	x = offsetX;
	y = offsetY;
	columnIndex = 0;
	TextVertex* vertex = data.vertices;
	TextVertex* startVertex = vertex;
	while (*it != 0 && numSymbols < maxNumSymbols)
	{
		if (uint8_t(*it) <= 32)
		{
			// special characters
			if (*it == ' ')
			{
				// space
				x += spaceWidth;
			}
			else if (*it == '\t')
			{
				// tab
				x = columns[columnIndex];
				columnIndex = (columnIndex + 1) & 3;
			}
			if (*it == '\n')
			{
				// new line

				// align line
				if (alignH != 0)
				{
					float d = maxX - (x - offsetX);
					float offset = alignH == 1 ? 0.5f * d : d;
					for (; startVertex < vertex; ++startVertex)
					{
						startVertex->position.x += offset;
					}
				}
				
				x = offsetX;
				y -= lineHeight;
				columnIndex = 0;
			}
			++it;
		}
		else
		{
			float sx;
			float sy;
			float sw;
			float sh;
			float w;
			float h;
			if (symbols.numSymbols == 0)
			{
				// no symbols: generate dummy quad
				sx = 0.0f;
				sy = 0.0f;
				sw = 1.0f;
				sh = 1.0f;
				w = scale.x;
				h = scale.y;
				++it;
			}
			else
			{
				// find symbol (may consist of multiple chars e.g. ligature 'ft' or utf8-encoded char)
				int low = 0;
				int high = symbols.numSymbols;
				int len = 0;
				int found = -1;
				while (*it != 0)
				{
					low = find(symbols, low, high, len, *it);
					high = find(symbols, low, high, len, *it + 1);

					if (low == high)
						break;
						
					found = low;
					++it;
					++len;
				}
				
				if (found == -1 || symbols.strings[symbols.indices[found] + len] != 0)
				{
					// symbol not found: use first symbol
					found = 0;
					if (len == 0)
						++it;
				}

				// get texture rectangle and size
				float* s = &symbols.rects[found * 4];
				sx = s[0];
				sy = s[1];
				sw = s[2];
				sh = s[3];
				w = sw * uv2posX;
				h = sh * uv2posY;
			}
			
			// generate quad for symbol
			vertex->position = vector3(x, y - h, 0.0f);
			vertex->normal = vector3(0.0f, 0.0f, 1.0f);
			vertex->tangent = vector3(1.0f, 0.0f, 0.0f);
			vertex->bitangent = vector3(0.0f, 1.0f, 0.0f);
			vertex->symbol = vector2(sx, 1.0f - (sy + sh));
			++vertex;
			vertex->position = vector3(x + w, y - h, 0.0f);
			vertex->normal = vector3(0.0f, 0.0f, 1.0f);
			vertex->tangent = vector3(1.0f, 0.0f, 0.0f);
			vertex->bitangent = vector3(0.0f, 1.0f, 0.0f);
			vertex->symbol = vector2(sx + sw, 1.0f - (sy + sh));
			++vertex;
			vertex->position = vector3(x + w, y, 0.0f);
			vertex->normal = vector3(0.0f, 0.0f, 1.0f);
			vertex->tangent = vector3(1.0f, 0.0f, 0.0f);
			vertex->bitangent = vector3(0.0f, 1.0f, 0.0f);
			vertex->symbol = vector2(sx + sw, 1.0f - sy);
			++vertex;
			vertex->position = vector3(x, y, 0.0f);
			vertex->normal = vector3(0.0f, 0.0f, 1.0f);
			vertex->tangent = vector3(1.0f, 0.0f, 0.0f);
			vertex->bitangent = vector3(0.0f, 1.0f, 0.0f);
			vertex->symbol = vector2(sx, 1.0f - sy);
			++vertex;

			// advance x position
			x += w + letterSpacing;
						
			++numSymbols;
		}
		maxX = max(maxX, x);
	}
	// align last line
	if (alignH != 0)
	{
		float d = maxX - (x - offsetX);
		float offset = alignH == 1 ? 0.5f * d : d;
		for (; startVertex < vertex; ++startVertex)
		{
			startVertex->position.x += offset;
		}
	}
	
/*
	// do alignment
	float minY = y - scale.y;
	float offsetX = align.x == 0 ? 0.0f : -(align.x == 1 ? 0.5f * maxX : maxX);
	float offsetY = align.y == 0 ? 0.0f : -(align.y == 1 ? 0.5f * minY : minY);
	int numPositions = numSymbols * 4;
	vertex = data.vertices;
	for (int i = 0; i < numPositions; ++i)
	{
		vertex->position.x += offsetX;
		vertex->position.y += offsetY;
		++vertex;
	}
*/
	// set number of symbols
	data.numSymbols = numSymbols;

	// set bounding box
	float3 p1 = vector3(offsetX, minY + offsetY, 0.0f);
	float3 p2 = vector3(maxX + offsetX, offsetY, 0.0f);
	data.center = (p1 + p2) * 0.5f;
	data.size = p2 - data.center;
/*
	vertices[0].position = vector2(0.0f, 0.0f);
	vertices[0].symbol = vector2(0.0f, 0.0f);
	vertices[1].position = vector2(0.0f, 1.0f);
	vertices[1].symbol = vector2(0.0f, 1.0f);
	vertices[2].position = vector2(1.0f, 1.0f);
	vertices[2].symbol = vector2(1.0f, 1.0f);
	vertices[3].position = vector2(1.0f, 0.0f);
	vertices[3].symbol = vector2(1.0f, 0.0f);

	data.numSymbols = 1;
*/
}

} // namespace digi

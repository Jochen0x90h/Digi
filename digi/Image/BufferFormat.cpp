#include "BufferFormat.h"


namespace digi {


const BufferFormat::LayoutInfo BufferFormat::layoutInfos[] =
{
	// standard layouts
	{STANDARD, 1, 1, 1, { 8,  0,  0,  0}}, // X8
	{STANDARD, 1, 2, 2, { 8,  8,  0,  0}}, // XY8
	{STANDARD, 1, 3, 3, { 8,  8,  8,  0}}, // XYZ8
	{STANDARD, 1, 4, 4, { 8,  8,  8,  8}}, // XYZW8
	{STANDARD, 2, 1, 1, {16,  0,  0,  0}}, // X16
	{STANDARD, 2, 2, 2, {16, 16,  0,  0}}, // XY16
	{STANDARD, 2, 3, 3, {16, 16, 16,  0}}, // XYZ16
	{STANDARD, 2, 4, 4, {16, 16, 16, 16}}, // XYZW16
	{STANDARD, 4, 1, 1, {32,  0,  0,  0}}, // X32
	{STANDARD, 4, 2, 2, {32, 32,  0,  0}}, // XY32
	{STANDARD, 4, 3, 3, {32, 32, 32,  0}}, // XYZ32
	{STANDARD, 4, 4, 4, {32, 32, 32, 32}}, // XYZW32

	// combined layouts
	{COMBINED, 2, 1, 4, { 4,  4,  4,  4}}, // X4Y4Z4W4
	{COMBINED, 2, 1, 4, { 5,  5,  5,  1}}, // X5Y5Z5W1
	{COMBINED, 2, 1, 3, { 5,  6,  5,  0}}, // X5Y6Z5
	{COMBINED, 4, 1, 4, { 8,  8,  8,  8}}, // X8Y8Z8W8
	{COMBINED, 4, 1, 4, { 9,  9,  9,  5}}, // X9Y9Z9W5
	{COMBINED, 4, 1, 4, {10, 10, 10,  2}}, // X10Y10Z10W2
	{COMBINED, 4, 1, 4, {11, 11, 10,  0}}, // X11Y11Z10
	
	// opaque formats
	{DATA_BLOCK, 1,  8, 0, {0, 0, 0, 0}}, // BLOCK8
	{DATA_BLOCK, 1, 16, 0, {0, 0, 0, 0}}, // BLOCK16
};

} // namespace digi

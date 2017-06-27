#ifndef digi_Data_WriteFunctions_h
#define digi_Data_WriteFunctions_h

#include <digi/Base/Platform.h>
#include <digi/Utility/as.h>


namespace digi {

/// @addtogroup Data
/// @{

// functions for writing of basic types to a buffer

template <typename Type>
void writeLE(Type value, uint8_t* buffer)
{
	for (size_t i = 0; i < sizeof(Type); ++i)
	{
		*buffer = uint8_t(value);
		value >>= 8;
		++buffer;
	}
}

template <typename Type>
struct WriteLE
{
	enum {SIZE = sizeof(Type)};
	void operator ()(Type value, uint8_t* buffer)
	{
		writeLE(value, buffer);
	}
};

template <>
struct WriteLE<bool>
{
	enum {SIZE = 1};
	void operator ()(bool value, uint8_t* buffer)
	{
		*buffer = value ? 1 : 0;
	}
};

template <>
struct WriteLE<float>
{
	enum {SIZE = 4};
	void operator ()(float value, uint8_t* buffer)
	{
		writeLE(as<uint32_t>(value), buffer);
	}
};

template <>
struct WriteLE<double>
{
	enum {SIZE = 8};
	void operator ()(double value, uint8_t* buffer)
	{
		writeLE(as<uint64_t>(value), buffer);
	}
};


template <typename Type>
void writeBE(Type value, uint8_t* buffer)
{
	buffer += sizeof(Type) - 1;
	for (size_t i = 0; i < sizeof(Type); ++i)
	{
		*buffer = uint8_t(value);
		value >>= 8;
		--buffer;
	}
}

template <typename Type>
struct WriteBE
{
	enum {SIZE = sizeof(Type)};
	void operator ()(Type value, uint8_t* buffer)
	{
		writeBE(value, buffer);
	}
};

template <>
struct WriteBE<bool>
{
	enum {SIZE = 1};
	void operator ()(bool value, uint8_t* buffer)
	{
		*buffer = value ? 1 : 0;
	}
};

template <>
struct WriteBE<float>
{
	enum {SIZE = 4};
	void operator ()(float value, uint8_t* buffer)
	{
		writeBE(as<uint32_t>(value), buffer);
	}
};

template <>
struct WriteBE<double>
{
	enum {SIZE = 8};
	void operator ()(double value, uint8_t* buffer)
	{
		writeBE(as<uint64_t>(value), buffer);
	}
};

/// @}

} // namespace digi

#endif

#ifndef digi_Data_ReadFunctions_h
#define digi_Data_ReadFunctions_h

#include <boost/detail/endian.hpp>

#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Data
/// @{

// functions for reading of basic types from buffer

template <typename Type>
Type readLE(uint8_t* buffer)
{
	Type value = Type();
	buffer += sizeof(Type) - 1;
	for (size_t i = 0; i < sizeof(Type); ++i)
	{
		value <<= 8;
		value |= *buffer;
		--buffer;
	}
	return value;
}

template <typename Type>
struct ReadLE
{
	enum {SIZE = sizeof(Type)};
	Type operator ()(uint8_t* buffer)
	{
		return readLE<Type>(buffer);
	}
};

template <>
struct ReadLE<bool>
{
	enum {SIZE = 1};
	bool operator ()(uint8_t* buffer)
	{
		return *buffer != 0;
	}
};

template <>
struct ReadLE<float>
{
	enum {SIZE = 4};
	float operator ()(uint8_t* buffer)
	{
		uint32_t v = readLE<uint32_t>(buffer);
		return *(float*)&v;
	}
};

template <>
struct ReadLE<double>
{
	enum {SIZE = 8};
	double operator ()(uint8_t* buffer)
	{
		uint64_t v = readLE<uint64_t>(buffer);
		return *(double*)&v;
	}
};


template <typename Type>
Type readBE(uint8_t* buffer)
{
	Type value = Type();
	for (size_t i = 0; i < sizeof(Type); ++i)
	{
		value <<= 8;
		value |= *buffer;
		++buffer;
	}
	return value;
}

template <typename Type>
struct ReadBE
{
	enum {SIZE = sizeof(Type)};
	Type operator ()(uint8_t* buffer)
	{
		return readBE<Type>(buffer);
	}
};

template <>
struct ReadBE<bool>
{
	enum {SIZE = 1};
	bool operator ()(uint8_t* buffer)
	{
		return *buffer != 0;
	}
};

template <>
struct ReadBE<float>
{
	enum {SIZE = 4};
	float operator ()(uint8_t* buffer)
	{
		uint32_t v = readBE<uint32_t>(buffer);
		return *(float*)&v;
	}
};

template <>
struct ReadBE<double>
{
	enum {SIZE = 8};
	double operator ()(uint8_t* buffer)
	{
		uint64_t v = readBE<uint64_t>(buffer);
		return *(double*)&v;
	}
};

template <>
struct ReadBE<long double>
{
	enum {SIZE = 10};
	long double operator ()(uint8_t* buffer)
	{
		long double e;
		uint8_t* d = (uint8_t*)&e;
		#ifdef BOOST_LITTLE_ENDIAN
			for (int i = 0; i < 10; ++i)
				d[i] = buffer[9 - i];
		#else
			for (int i = 0; i < 10; ++i)
				d[i] = buffer[i];
		#endif
		return e;
	}
};

/// @}

} // namespace digi

#endif

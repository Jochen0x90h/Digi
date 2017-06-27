#ifndef digi_Scene_ObjectReader_h
#define digi_Scene_ObjectReader_h

#include <digi/Utility/MapUtility.h>
#include <digi/Utility/Standard.h>
#include <digi/Utility/foreach.h>
#include <digi/Data/LittleEndianReader.h>
#include <digi/Image/Buffer.h>
#include <digi/Image/Image.h>
#include <digi/Math/All.h>
#include <digi/CodeGenerator/Type.h>

#include "ValueWrapper.h"


namespace digi {

/// @addtogroup Scene
/// @{

class ObjectReader;

template <typename Type>
void load(ObjectReader& r, Pointer<Type>& object)
{
	object = Type::load(r);
}

void load(ObjectReader& r, Pointer<Buffer>& buffer);
void load(ObjectReader& r, Pointer<Image>& image);


class ObjectReader : public LittleEndianReader
{
public:

	ObjectReader(Pointer<IODevice> dev)
		: LittleEndianReader(dev), version(0) {}

	ObjectReader(const fs::path& path)
		: LittleEndianReader(path), version(0) {}

	template <typename Type>
	void readVarSizeUnsigned(Type& value)
	{
		uint8_t b;
		value = 0;
		int shift = 0;
		do
		{
			b = this->read<uint8_t>();
			value |= Type(b & 0x7f) << shift;
			shift += 7;
		} while (b & 0x80);
	}

	template <typename Type>
	void readVarSizeSigned(Type& value)
	{
		this->readVarSizeUnsigned(value);
		value = (value & 1) == 0 ? value >> 1 : ~(value >> 1);
	}


	// basic types
	ObjectReader& operator &(bool& value)
	{
		uint8_t tmp;
		this->readVarSizeUnsigned(tmp);
		value = tmp != 0;
		return *this;	
	}

	ObjectReader& operator &(signed char& value)
	{
		this->readVarSizeSigned(value);
		return *this;	
	}

	ObjectReader& operator &(unsigned char& value)
	{
		this->readVarSizeUnsigned(value);
		return *this;	
	}

	ObjectReader& operator &(short& value)
	{
		this->readVarSizeSigned(value);
		return *this;	
	}

	ObjectReader& operator &(unsigned short& value)
	{
		this->readVarSizeUnsigned(value);
		return *this;	
	}

	ObjectReader& operator &(int& value)
	{
		this->readVarSizeSigned(value);
		return *this;	
	}

	ObjectReader& operator &(unsigned int& value)
	{
		this->readVarSizeUnsigned(value);
		return *this;	
	}

	ObjectReader& operator &(long& value)
	{
		this->readVarSizeSigned(value);
		return *this;	
	}

	ObjectReader& operator &(unsigned long& value)
	{
		this->readVarSizeUnsigned(value);
		return *this;	
	}

	ObjectReader& operator &(long long& value)
	{
		this->readVarSizeSigned(value);
		return *this;	
	}

	ObjectReader& operator &(unsigned long long& value)
	{
		this->readVarSizeUnsigned(value);
		return *this;	
	}

	ObjectReader& operator &(float& value)
	{
		this->read(value);
		return *this;	
	}

	ObjectReader& operator &(double& value)
	{
		this->read(value);
		return *this;	
	}


	// vector types
	template <typename Type>
	ObjectReader& operator &(Vector4<Type>& value)
	{
		return *this & value.x & value.y & value.z & value.w;
	}


	// object types
	template <typename Type>
	ObjectReader& operator &(Pointer<Type>& object)
	{
		uint index;
		
		// read index
		*this & index;
					
		if (index == 0)
		{
			// object is null
			object = null;
		}
		else
		{
			--index;
			if (index == 0)
			{
				// object is new
				
				// assign index and occupy object
				index = uint(this->objects.size() + 1);
				Pointer<Object>& o = this->objects[index];
				
				// then load object
				load(*this, object);
				o = object;
			}
			else
			{
				// object is already known
				object = staticCast<Type>(this->objects[index]);
			}
		}
		return *this;
	}

	uint version;
	
protected:
	
	std::map<uint, Pointer<Object> > objects;
};


// value wrapper e.g. for enums
template <typename DataType, typename Type>
ObjectReader& operator &(ObjectReader& r, ValueWrapper<DataType, Type> value)
{
	DataType tmp;
	r & tmp;
	value.value = Type(tmp);
	return r;
}


// convenience read function that returns the value
template <typename Type>
Type readVarSize(ObjectReader& r)
{
	Type temp;
	r & temp;
	return temp;
}


// std::string
static inline ObjectReader& operator &(ObjectReader& r, std::string& str)
{
	size_t size = readVarSize<size_t>(r);
	str.resize(size);
	r.readData((char*)str.data(), size);
	return r;
}

// std::vector
template <typename Type>
ObjectReader& operator &(ObjectReader& r, std::vector<Type>& ar)
{
	//! optimize for basic types (e.g. uint)

	size_t size = readVarSize<size_t>(r);
	ar.resize(size);
	foreach (Type& element, ar)
		r & element;
	return r;
}

// std::set
template <typename Type>
ObjectReader& operator &(ObjectReader& r, std::set<Type>& set)
{
	size_t size = readVarSize<size_t>(r);
	for (size_t i = 0; i < size; ++i)
	{
		Type element;
		r & element;
		set.insert(element);
	}
	return r;
}

// std::map
template <typename Key, typename Value>
ObjectReader& operator &(ObjectReader& r, std::map<Key, Value>& map)
{
	size_t size = readVarSize<size_t>(r);
	for (size_t i = 0; i < size; ++i)
	{
		Key key;
		r & key;
		r & map[key];
	}
	return r;
}
		
// digi::Type
static inline ObjectReader& operator &(ObjectReader& r, Pointer<Type>& type)
{
	std::string str;
	r & str;
	type = Type::create(str);
	return r;
}


struct VersionReader
{
	VersionReader(ObjectReader& r)
		: savedVersion(r.version), ref(r.version)
	{
		uint version;
		r & version;
		
		// set version
		r.version = version;
	}
	
	~VersionReader()
	{
		// restore version
		this->ref = this->savedVersion;
	}
	
	uint savedVersion;
	uint& ref;
};

/// @}

} // namespace digi

#endif

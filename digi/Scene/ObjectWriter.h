#ifndef digi_Scene_ObjectWriter_h
#define digi_Scene_ObjectWriter_h

#include <digi/Utility/MapUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/Standard.h>
#include <digi/Data/LittleEndianWriter.h>
#include <digi/Image/Buffer.h>
#include <digi/Image/Image.h>
#include <digi/CodeGenerator/Type.h>
#include <digi/Utility/foreach.h>

#include "ValueWrapper.h"


namespace digi {

/// @addtogroup Scene
/// @{

class ObjectWriter;

template <typename Type>
void save(ObjectWriter& w, Pointer<Type> object)
{
	object->save(w);
}

void save(ObjectWriter& w, Pointer<Buffer> buffer);
void save(ObjectWriter& w, Pointer<Image> image);


class ObjectWriter : public LittleEndianWriter
{
public:

	ObjectWriter(Pointer<IODevice> dev)
		: LittleEndianWriter(dev), version(0) {}

	ObjectWriter(const fs::path& path)
		: LittleEndianWriter(path), version(0) {}

	template <typename Type>
	void writeVarSizeUnsigned(Type value)
	{
		uint8_t buffer[10];
		int i = 0;
		do
		{
			buffer[i++] = uint8_t(value | 0x80);
			value >>= 7;
		} while (value > 0);
		buffer[i - 1] &= 0x7f;
		this->writeData(buffer, i);
	}

	template <typename Type>
	void writeVarSizeSigned(Type value)
	{
		this->writeVarSizeUnsigned(value >= 0 ? value << 1 : (~value << 1) | 1);
	}


	// basic types
	ObjectWriter& operator &(bool value)
	{
		this->write(value);
		return *this;	
	}

	ObjectWriter& operator &(signed char value)
	{
		this->writeVarSizeSigned(value);
		return *this;	
	}

	ObjectWriter& operator &(unsigned char value)
	{
		this->writeVarSizeUnsigned(value);
		return *this;	
	}

	ObjectWriter& operator &(short value)
	{
		this->writeVarSizeSigned(value);
		return *this;	
	}

	ObjectWriter& operator &(unsigned short value)
	{
		this->writeVarSizeUnsigned(value);
		return *this;	
	}

	ObjectWriter& operator &(int value)
	{
		this->writeVarSizeSigned(value);
		return *this;	
	}

	ObjectWriter& operator &(unsigned int value)
	{
		this->writeVarSizeUnsigned(value);
		return *this;	
	}

	ObjectWriter& operator &(long value)
	{
		this->writeVarSizeSigned(value);
		return *this;	
	}

	ObjectWriter& operator &(unsigned long value)
	{
		this->writeVarSizeUnsigned(value);
		return *this;	
	}

	ObjectWriter& operator &(long long value)
	{
		this->writeVarSizeSigned(value);
		return *this;	
	}

	ObjectWriter& operator &(unsigned long long value)
	{
		this->writeVarSizeUnsigned(value);
		return *this;	
	}

	ObjectWriter& operator &(float value)
	{
		this->write(value);
		return *this;	
	}

	ObjectWriter& operator &(double value)
	{
		this->write(value);
		return *this;	
	}


	// vector types
	template <typename Type>
	ObjectWriter& operator &(const Vector4<Type>& value)
	{
		return *this & value.x & value.y & value.z & value.w;
	}


	// object types
	template <typename Type>
	ObjectWriter& operator &(Pointer<Type> object)
	{
		if (object == null)
		{
			// write zero
			*this & 0;
		}
		else
		{	
			uint& index = this->objects[object];
			
			// write object index + 1
			*this & (index + 1);
			
			// check if object is new
			if (index == 0)
			{
				// assign new index
				index = uint(this->objects.size());
				
				// save object
				save(*this, object);
			}
		}
		return *this;
	}

	uint version;
	
protected:
	
	std::map<Pointer<Object>, uint> objects;
};


// value wrapper e.g. for enums
template <typename DataType, typename Type>
ObjectWriter& operator &(ObjectWriter& w, ValueWrapper<DataType, Type> value)
{
	w & DataType(value.value);
	return w;
}


// std::string
static inline ObjectWriter&  operator &(ObjectWriter& w, const std::string& str)
{
	size_t size = str.size();
	w & size;
	w.writeData(str.data(), size);
	return w;
}

// std::vector
template <typename Type>
ObjectWriter& operator &(ObjectWriter& w, std::vector<Type>& ar)
{
	//! optimize for basic types (e.g. uint)
	
	size_t size = ar.size();
	w & size;
	foreach (Type& element, ar)
		w & element;
	return w;
}

// std::vector<uint8_t>
static inline ObjectWriter& operator &(ObjectWriter& w, std::vector<uint8_t>& ar)
{
	size_t size = ar.size();
	w & size;
	w.writeData(ar.data(), size);
	return w;
}

// std::set
template <typename Type>
ObjectWriter& operator &(ObjectWriter& w, std::set<Type>& set)
{
	size_t size = set.size();
	w & size;
	foreach (const Type& element, set)
		w & element;
	return w;
}

// std::map
template <typename Key, typename Value>
ObjectWriter& operator &(ObjectWriter& w, std::map<Key, Value>& map)
{
	size_t size = map.size();
	w & size;
	for (typename std::map<Key, Value>::iterator it = map.begin(); it != map.end(); ++it)
	{
		w & it->first;
		w & it->second;
	}
	return w;
}

// digi::StringRef
static inline ObjectWriter&  operator &(ObjectWriter& w, StringRef str)
{
	size_t size = str.size();
	w & size;
	w.writeData(str.data(), size);
	return w;
}

// digi::Type
static inline ObjectWriter& operator &(ObjectWriter& w, Pointer<Type> type)
{
	if (type != null)
		w & type->toString();
	else
		w & 0;
	return w;
}


struct VersionWriter
{
	VersionWriter(ObjectWriter& w, uint version)
		: savedVersion(w.version), ref(w.version)
	{
		w & version;
		
		// set version
		w.version = version;
	}
	
	~VersionWriter()
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

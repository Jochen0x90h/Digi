#ifndef digi_CodeGenerator_NameMangler_h
#define digi_CodeGenerator_NameMangler_h

#include <sstream>

#include <digi/Utility/StringRef.h>

#include "TypeInfo.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{


// mangles names, e.g. convert_float3(int3) -> _Z14convert_float3Dv3_i
// http://www.codesourcery.com/public/cxx-abi/abi.html#mangling
class NameMangler
{
public:
	
	NameMangler()	{}
	
	void addFunction(StringRef name);
	
	void addArgument(VectorInfo::Type type);
	void addArgument(VectorInfo type);
	
	std::string get() {return s.str();}

	template <typename Type1>
	static std::string mangle(StringRef name, Type1 type1)
	{
		NameMangler m;
		m.addFunction(name);
		m.addArgument(type1);
		return m.get();
	}
	
	template <typename Type1, typename Type2>
	static std::string mangle(StringRef name, Type1 type1, Type2 type2)
	{
		NameMangler m;
		m.addFunction(name);
		m.addArgument(type1);
		m.addArgument(type2);
		return m.get();	
	}

protected:
	
	std::stringstream s;
};

class NameDemangler
{
public:

	static std::string demangle(StringRef mangled);
};


/// @}

} // namespace digi

#endif

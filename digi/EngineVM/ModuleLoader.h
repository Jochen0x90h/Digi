#ifndef digi_EngineVM_ModuleLoader_h
#define digi_EngineVM_ModuleLoader_h

// note: llvm needs __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS to be defined
#undef interface
#include <clang/Lex/ModuleLoader.h>


namespace digi {

/// @addtogroup EngineVM
/// @{

class ModuleLoader : public clang::ModuleLoader
{
public:
	virtual ~ModuleLoader();
	
	virtual clang::ModuleKey loadModule(clang::SourceLocation ImportLoc, clang::IdentifierInfo &ModuleName,
		clang::SourceLocation ModuleNameLoc);
};

/// @}

} // namespace digi

#endif

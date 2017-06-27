#include "ModuleLoader.h"


namespace digi {

// ModuleLoader

ModuleLoader::~ModuleLoader()
{
}
	
clang::ModuleKey ModuleLoader::loadModule(clang::SourceLocation ImportLoc, clang::IdentifierInfo &ModuleName,
	clang::SourceLocation ModuleNameLoc)
{
	return NULL;
}

} // namespace digi

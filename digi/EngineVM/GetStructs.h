#ifndef digi_EngineVM_GetStructs_h
#define digi_EngineVM_GetStructs_h

#include "CodeGenWrapper.h"


namespace digi {

/// @addtogroup EngineVM
/// @{

class GetStructs : public CodeGenWrapper
{
public:

	GetStructs(clang::CodeGenerator* codeGenerator);

	virtual ~GetStructs();

	// collects global variables
	virtual void handleDecl(clang::Decl* decl);

	
	clang::RecordDecl* globalDecl;
	clang::RecordDecl* instanceDecl;
};

/// @}

} // namespace digi

#endif

#ifndef digi_SceneConvert_GlobalCollector_h
#define digi_SceneConvert_GlobalCollector_h

#include <map>

#include <digi/EngineVM/CodeGenWrapper.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

/*
	collects all global variables in a map (name -> VarDecl) while compiling (derives from clang::CodeGenerator).
	this is used to find out which variables (and their members) the llvm IR accesses.
*/
class GlobalCollector : public CodeGenWrapper
{
public:

	GlobalCollector(clang::CodeGenerator* codeGenerator);

	virtual ~GlobalCollector();


	// collects global variables
	virtual void handleDecl(clang::Decl* decl);

	
	// map for global variable name -> VarDecl
	std::map<std::string, clang::VarDecl*> decls;
};


/// @}

} // namespace digi

#endif

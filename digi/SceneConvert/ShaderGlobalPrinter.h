#ifndef digi_SceneConvert_ShaderGlobalPrinter_h
#define digi_SceneConvert_ShaderGlobalPrinter_h

#include <string>

#include <llvm/Value.h>

#include <digi/Utility/Object.h>
#include <digi/Math/All.h>

#include "GlobalPrinter.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

/// writes globals into builtin variables (e.g. "gl_Position")
class ShaderGlobalPrinter : public GlobalPrinter
{
public:

	ShaderGlobalPrinter(NameGenerator& nameGenerator)
		: nameGenerator(nameGenerator)
	{}
	
	virtual ~ShaderGlobalPrinter();
	
	// print a global for store. may only be called once
	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	
	// print a global for load. returns true if the global is a vector
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	
	NameGenerator nameGenerator;
	
	// output variable (vector or array of vectors)
	ShaderVariable output;
};

/// @}

} // namespace digi

#endif

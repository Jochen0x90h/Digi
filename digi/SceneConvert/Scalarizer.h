#ifndef digi_SceneConvert_Scalarizer_h
#define digi_SceneConvert_Scalarizer_h

#include <map>

#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

// scalarizes a module including structs. vectors are replaced by arrays
class Scalarizer
{
public:

	struct ValueInfo
	{
		llvm::Value* values[4];
	};

	// create scalarizer with function to scalarize into given module
	Scalarizer(llvm::Function& function, llvm::Module& sModule);


	llvm::BasicBlock* getBasicBlock(llvm::BasicBlock* bb);
	
	ValueInfo getValue(llvm::Value* value);

	ValueInfo getPointer(llvm::IRBuilder<>& builder, llvm::Value* value, int numElements);

	llvm::Value* getGlobal(llvm::Value* value);

	llvm::Type* scalarizeType(llvm::Type* type);
	

	// scalarized module
	llvm::Module& sModule;
	
	// scalarized function
	llvm::Function* sFunction;

	// maps basic block to its copy
	std::map<llvm::BasicBlock*, llvm::BasicBlock*> basicBlocks;
	
	// maps values to its scalar copies
	std::map<const llvm::Value*, ValueInfo> valueInfos;
};

/// @}

} // namespace digi

#endif

#ifndef digi_SceneConvert_PrintWalker_h
#define digi_SceneConvert_PrintWalker_h

#include <map>

#include <llvm/Module.h>

#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/NameGenerator.h>

#include "ValueHelper.h"
#include "GlobalPrinter.h"
#include "PrintHelper.h"
#include "InstructionWalker.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{


class PrintWalker : public InstructionWalker
{
public:

	PrintWalker(llvm::PostDominatorTree* postDominatorTree, Pointer<IODevice> dev, Language language,
		std::map<std::string, GlobalPrinterAccess>& globalPrinters)
		: InstructionWalker(postDominatorTree), w(dev, language), globalPrinters(globalPrinters) {}

	void close() {this->w.close();}

	// do a branch		
	virtual void doBranch(llvm::BasicBlock* basicBlock, llvm::Value* condition, llvm::BasicBlock* trueBranch,
		llvm::BasicBlock* falseBranch, llvm::BasicBlock* join, llvm::BasicBlock* stopBlock);

	// assigns values to phi variables before join
	virtual void doJoin(llvm::BasicBlock* predecessor, llvm::BasicBlock* basicBlock);

	// do an instruction
	virtual void doInstruction(llvm::BasicBlock* predecessor, llvm::Instruction* instruction);

			
	// print an instruction
	void printInstruction(CodeStream& w, llvm::Instruction* instruction, int priority);
			
	// print a value
	void printValue(CodeStream& w, llvm::Value* value, int priority);

	// print a vector
	void printVector(CodeStream& w, llvm::Value* value, TrackValue* values, int numElements, int priority);

	// print name of a value that is already known (created with createName()). returns true if value is a vector
	void printName(CodeStream& w, llvm::Value* value);

	// print variable for value
	void printVariable(CodeStream& w, llvm::Value* value);

	// print global for load, returns true if vector
	void printGlobalForLoad(CodeStream& w, llvm::Type* type, llvm::Value* ptr, int priority);
	
	// print global for store
	void printGlobalForStore(llvm::Value* value, llvm::Value* ptr);


	// generator for temporary variables
	NameGenerator nameGenerator;


	// pair of value name and vector property
	struct ValueName
	{
		std::string name;
		
		ValueName()
			{}
		ValueName(const std::string& name)
			: name(name) {}
	};

	// maps a value to its name
	std::map<llvm::Value*, ValueName> names;

	CodeWriter w;
	std::map<std::string, GlobalPrinterAccess>& globalPrinters;
};

/// @}

} // namespace digi

#endif

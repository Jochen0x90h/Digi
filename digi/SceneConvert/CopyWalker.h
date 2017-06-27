#ifndef digi_SceneConvert_CopyWalker_h
#define digi_SceneConvert_CopyWalker_h

#include <llvm/Module.h>
#include <llvm/Instructions.h>

#include <digi/Utility/ArrayRef.h>
#include <digi/Utility/StringRef.h>
#include <digi/CodeGenerator/NameGenerator.h>

#include "InstructionWalker.h"
#include "ShaderType.h"

namespace digi {

/// @addtogroup SceneConvert
/// @{

class CopyWalker : public InstructionWalker
{
public:

	typedef std::map<llvm::Value*, llvm::Value*>::iterator ValueIterator;
	typedef std::map<llvm::Value*, llvm::Value*>::value_type ValuePair;

	struct Scope
	{
		// debug
		int index;
		
		llvm::Module* module;
		
		llvm::Function* currentFunction;
		
		llvm::BasicBlock* currentBasicBlock;

		// mapping from original basic blocks to copied basic blocks
		std::map<llvm::BasicBlock*, llvm::BasicBlock*> basicBlocks;

		// mapping from original values to copied values
		std::map<llvm::Value*, llvm::Value*> values;
						
		Scope()
			: module(), currentFunction(), currentBasicBlock() {}
			
		void insert(llvm::Value* value, llvm::Value* scopeValue)
		{
			this->values.insert(ValuePair(value, scopeValue));
		}
	};

	typedef std::pair<Scope*, Scope*> ScopePair;
		
	
	struct Transfer
	{
		struct ValueInfo
		{
			llvm::LoadInst* loadInst;
			llvm::Value* vectorized;

			ValueInfo()
				: loadInst(), vectorized() {}
		};
	
		struct GlobalAccess
		{
			llvm::Value* global;
			std::vector<int> access;
	
			GlobalAccess(llvm::Value* global)
				: global(global) {}
			
			bool operator <(const GlobalAccess& a) const
			{
				return this->global < a.global || (this->global == a.global && this->access < a.access);
			}
		};
	
		struct ArrayInfo
		{
			struct Load
			{
				llvm::LoadInst* inst;
				llvm::Value* index;
			
				Load()
					: inst(), index() {}
				Load(llvm::LoadInst* inst, llvm::Value* index)
					: inst(inst), index(index) {}
			};
	
			// the array type
			llvm::Type* type;

			// all load accesses to this array
			std::vector<Load> loads;
		
			ArrayInfo()
				: type() {}
		};

		typedef std::map<llvm::Value*, ValueInfo>::value_type ValuePair;
		typedef std::map<llvm::Value*, ValueInfo>::iterator ValueIterator;

		// original value -> load instruction in destination scope
		std::map<llvm::Value*, ValueInfo> valueInfos;
		
		// used to preserve the order
		std::vector<ValueIterator> valueIterators;

		typedef std::map<GlobalAccess, ArrayInfo>::value_type ArrayPair;
		typedef std::map<GlobalAccess, ArrayInfo>::iterator ArrayIterator;

		// original global and access pattern -> copied array
		std::map<GlobalAccess, ArrayInfo> arrayInfos;

		// used to preserve the order
		std::vector<ArrayIterator> arrayIterators;
	};


	// for classifying values
	struct ValueInfo
	{
		int scope;
		int flags;

		ValueInfo()
			: scope(-1), flags(0) {}
			
		ValueInfo(int scope, int flags)
			: scope(scope), flags(flags) {}
	};

	
	CopyWalker(llvm::PostDominatorTree* postDominatorTree, llvm::Function& function,
		ArrayRef<llvm::OwningPtr<llvm::Module> > modules);

	virtual ~CopyWalker();

	// do a branch		
	virtual void doBranch(llvm::BasicBlock* basicBlock, llvm::Value* condition, llvm::BasicBlock* trueBranch,
		llvm::BasicBlock* falseBranch, llvm::BasicBlock* join, llvm::BasicBlock* stopBlock);

	// copy an instruction
	virtual void doInstruction(llvm::BasicBlock* predecessor, llvm::Instruction* instruction);

	// copy a basic block (or move to end of function if already present)
	llvm::BasicBlock* copyBasicBlock(Scope* scope, llvm::BasicBlock* basicBlock);

	// get a copied a value
	llvm::Value* getValue(Scope* scope, llvm::Value* value);

	// get or copy a global variable
	llvm::Value* getGlobal(Scope* scope, llvm::Value* value);

	// get scope of value
	Scope* getScope(llvm::Value* value);

	
	// set type of classification root (global input variable)
	void setScope(llvm::Function& function, const std::string& globalName, int scope, int flags = 0);

	// classify instructions into scopes
	std::pair<llvm::Value*, int> classifyInstructions(llvm::Function& function,
		const std::string& globalName, int outputScope, int flags);
	
	// classify an instruction, returns scope
	int classifyInstruction(llvm::Instruction* instruction, int flags);
	
	// classify a basic block, returns scope
	int classifyBasicBlock(llvm::BasicBlock* bb, int flags, int forceScope = -1);

	// classify a value, returns scope
	int classifyValue(llvm::Value* value, int flags)
	{
		if (llvm::Instruction* instruction = llvm::dyn_cast<llvm::Instruction>(value))
		{
			// instruction
			return this->classifyInstruction(instruction, flags);
		}	
		if (llvm::isa<llvm::GlobalValue>(value))
		{
			// global variable
			ValueInfo& valueInfo = this->valueInfos[value];
			valueInfo.flags |= flags;
			return valueInfo.scope;
		}
		
		// constant (not in any scope)
		return -1;
	}
	
	// classify function, returns scope
	virtual int classifyFunction(llvm::CallInst* callInst, const std::string& name, int flags);

	// set scope to value info
	virtual void setValueScope(ValueInfo& valueInfo, int scope);


	// insert store and fix load instructions and get transfer shader types
	void fixAndGetTransfer(int srcScopeIndex, int dstScopeIndex, std::vector<ShaderType>& shaderTypes,
		StringRef name, bool getObjectTypes);
	
	
	llvm::BasicBlock* entryBlock;

	// the scopes that the code is split up into
	std::vector<Scope> scopes;

	// maps a value or basic block to classification
	std::map<const llvm::Value*, ValueInfo> valueInfos;

	// tansfer between scopes
	std::map<ScopePair, Transfer> transfers;
};

/// @}

} // namespace digi

#endif

#include <llvm/Support/IRBuilder.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/Math/all.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "ValueHelper.h"
#include "ShaderWalker.h"


namespace digi {

ShaderWalker::~ShaderWalker()
{
}

void ShaderWalker::sampleFlipY(llvm::Function& function, llvm::StringRef name)
{
	// get global function
	llvm::GlobalValue* global = function.getParent()->getNamedValue(name);
	if (global != NULL)
	{
		// iterate over uses of the global value
		llvm::Value::use_iterator it = global->use_begin(), end = global->use_end();
		for (; it != end; ++it)
		{
			llvm::Value* value = *it;
			if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(value))
			{
				// is a call instruction
				
				// check if instruction belongs to this function
				if (callInst->getParent()->getParent() == &function)
				{
					// get uv coords and its type
					llvm::Value* coord = callInst->getArgOperand(1);
					llvm::Type* type = coord->getType();
					
					// coord * (1, -1) + (0, 1)
					llvm::IRBuilder<> builder(callInst);
					llvm::Value* mul = builder.CreateFMul(coord, createFloatVector(type, vector4(1.0f, -1.0f, 1.0f, 1.0f)));
					llvm::Value* add = builder.CreateFAdd(mul, createFloatVector(type, vector4(0.0f, 1.0f, 0.0f, 0.0f)));
					
					callInst->setArgOperand(1, add);
				}				
			}
		}
	}
}

int ShaderWalker::classifyFunction(llvm::CallInst* callInst, const std::string& name, int flags)
{
	int scope = -1;
	
	if (!this->avoidPixelShader)
	{
		if (name == "vs")
		{
			// vs() forces execution in vertex shader
			flags &= ~USED_BY_PIXEL;
			flags |= USED_BY_VERTEX;
		}
	}
	else
	{
		if (startsWith(name, "sample"))
		{
			// texture sampling always in pixel shader
			scope = PIXEL;
		}
	}

	// classify arguments
	int numOperands = callInst->getNumArgOperands();
	for (int i = 0; i < numOperands; ++i)
	{
		llvm::Value* value = callInst->getArgOperand(i);
		scope = max(scope, this->classifyValue(value, flags));
	}

	return scope;
}

void ShaderWalker::setValueScope(ValueInfo& valueInfo, int scope)
{
	// if a value is used only by pixel and is in vertex scope then move to pixel scope
	if (!this->avoidPixelShader && valueInfo.flags == USED_BY_PIXEL && scope == VERTEX)
	{
		valueInfo.scope = PIXEL;
	}
	else
	{
		// use max as a basic block may be classified to PIXEL by discard()
		valueInfo.scope = max(valueInfo.scope, scope);
	}	
}

} // namespace digi

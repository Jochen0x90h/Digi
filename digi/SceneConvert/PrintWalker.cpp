#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/Constants.h>
#include <llvm/Support/IRBuilder.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "PrintWalker.h"


namespace digi {

namespace
{
	// info for binary operators

	struct BinaryOperator
	{
		const char* text;
		int priority;
		
		// priority for right hand side to handle these cases:
		// a - (b - c)
		// a / (b * c)
		int rightPriority;	
	};

	const BinaryOperator binaryOperators[] = 
	{
		{"|",  PRIO_OR,      PRIO_OR},
		{"^",  PRIO_XOR,     PRIO_XOR},
		{"&",  PRIO_AND,     PRIO_AND},
		{"==", PRIO_EQUALS,  PRIO_EQUALS},
		{"!=", PRIO_EQUALS,  PRIO_EQUALS},
		{"<",  PRIO_LESS,    PRIO_LESS},
		{">",  PRIO_LESS,    PRIO_LESS},
		{"<=", PRIO_LESS,    PRIO_LESS},
		{">=", PRIO_LESS,    PRIO_LESS},
		{"<<", PRIO_SHIFT,   PRIO_SHIFT},
		{">>", PRIO_SHIFT,   PRIO_SHIFT},
		{"+",  PRIO_ADD,     PRIO_ADD},
		{"-",  PRIO_ADD,     PRIO_ADD + 1},
		{"*",  PRIO_MUL,     PRIO_MUL},
		{"/",  PRIO_MUL,     PRIO_MUL + 1},
		{"%",  PRIO_MUL,     PRIO_MUL + 1}
	};

	enum BinaryOp
	{
		BINARY_OR,
		BINARY_XOR,
		BINARY_AND,
		BINARY_EQUAL,
		BINARY_NOT_EQUAL,
		BINARY_LESS,
		BINARY_GREATER,
		BINARY_LESS_EQUAL,
		BINARY_GREATER_EQUAL,
		BINARY_SHIFT_LEFT,
		BINARY_SHIFT_RIGHT,
		BINARY_ADD,
		BINARY_SUB,
		BINARY_MUL,
		BINARY_DIV,
		BINARY_MOD,
		
		BINARY_INVALID
	};


	struct FunctionInfo
	{
		// name in llvm IR
		const char* name;
		
		// name for C++/HLSL output
		const char* cppName;

		// name for GLSL/ESSL output
		const char* glslName;
		
		// flags to indicate which arguments may be scalar instead of vector (e.g. min(vector, scalar))
		int flags;
		
		// name for javascript output
		const char* jsName;
	};

	struct FunctionInfoEquals
	{
		bool operator ()(const FunctionInfo& a, const std::string& b)
		{
			return a.name == b;
		}
	};

	const FunctionInfo functionInfos[] =
	{	// must be sorted!
	//   LLVM       C++/HLSL    GLSL       flags    JavaScript
		{"abs",     "abs",      "abs",         0,   "Math.abs"    },
		{"acos",    "acos",     "acos",        0,   "Math.acos"   },
		{"asin",    "asin",     "asin",        0,   "Math.asin"   },
		{"atan",    "atan",     "atan",        0,   "Math.atan"   },
		{"atan2",   "atan2",    "atan",        0,   "Math.atan2"  }, //TODO: HLSL: atan2(x, y)
		{"ceil",    "ceil",     "ceil",        0,   "Math.ceil"   },
		{"cos",     "cos",      "cos",         0,   "Math.cos"    },
		{"eBTF",    "",         "",            0,   "engine.eBT"  }, // evalBezierTrack
		{"eBTS",    "",         "",            0,   "engine.eBT"  },
		{"eCTF",    "",         "",            0,   "engine.eCT"  }, // evalCatmulRomTrack
		{"eCTS",    "",         "",            0,   "engine.eCT"  },
		{"eSTF",    "",         "",            0,   "engine.eST"  }, // evalStepTrack
		{"eSTS",    "",         "",            0,   "engine.eST"  },
		{"eWBTF",   "",         "",            0,   "engine.eWBT" }, // evalWeightedBezierTrack
		{"eWBTS",   "",         "",            0,   "engine.eWBT" },
		{"exp",     "exp",      "exp",         0,   "Math.exp"    },
		{"fabs",    "abs",      "abs",         0,   "Math.abs"    },
		{"floor",   "floor",    "floor",       0,   "Math.floor"  },
		{"frac",    "frac",     "fract",       0,   "engine.frac" },
		{"log",     "log",      "log",         0,   "Math.log"    },
		{"log10",   "log10",    "log10",       0,   "Math.log10"  }, //TODO: log10 not supported in glsl, replace by log(x)/log(10)
		{"log2",    "log2",     "log2",        0,   "Math.log2"   },
		{"max",     "max",      "max",         2,   "Math.max"    }, // max(vector, scalar) allowed
		{"min",     "min",      "min",         2,   "Math.min"    }, // min(vector, scalar) allowed
		{"mod",     "mod",      "mod",         2,   NULL          }, // mod(vector, scalar) allowed, % in javascript
		{"pow",     "pow",      "pow",         0,   "Math.pow"    },
		{"round",   "round",    "round",       0,   "Math.round"  },
		{"rsqrt",   "rsqrt",    "inversesqrt", 0,   NULL          }, // does not occur in input code
		{"sign",    "sign",     "sign",        0,   "engine.sign" },
		{"sin",     "sin",      "sin",         0,   "Math.sin"    },
		{"sqrt",    "sqrt",     "sqrt",        0,   "Math.sqrt"   },
		{"step",    "step",     "step",        1,   NULL          }, // step(scalar, vector) allowed, does not occur in input code
		{"tan",     "tan",      "tan",         0,   "Math.tan"    },
		{"trunc",   "trunc",    "trunc",       0,   "engine.trunc"},
		{"vs",      "",         "",            0,   NULL          }, // forces execution in vertex shader
	};

	struct ShaderInfo
	{
		const char* name;
		const char* oldGLSLName;
		const char* glslName;
		const char* hlslName;
	};

	struct ShaderInfoEquals
	{
		bool operator ()(const ShaderInfo& a, const std::string& b)
		{
			return a.name == b;
		}
	};

	// note: when changing names of sample() functions also update ShaderPass::runOnFunction
	const ShaderInfo shaderInfos[] =
	{
	//   LLVM                            old GLSL (<= 1.2)  GLSL (>= 1.3)    HLSL (dx9)
		{"_Z6samplePU3AS2iDv2_f",        "texture2D",       "texture",       "tex2D"  },
		{"_Z6samplePU3AS3iDv3_f",        "texture3D",       "texture",       "tex3D"  },
		{"_Z6samplePU3AS4iDv3_f",        "textureCube",     "texture",       "texCUBE"},

		{"_Z6samplePU3AS2iDv2_ff",       "texture2DLod",    "textureLod",    "tex2Dlod"  },
		{"_Z6samplePU3AS3iDv3_ff",       "texture3DLod",    "textureLod",    "tex3Dlod"  },
		{"_Z6samplePU3AS4iDv3_ff",       "textureCube",     "textureLod",    "texCUBElod"},

		{"_Z6samplePU3AS2iDv2_fS1_S1_",  "texture2DGrad",   "textureGrad",   "tex2Dgrad"  },
		{"_Z6samplePU3AS3iDv3_fS1_S1_",  "texture3DGrad",   "textureGrad",   "tex3Dgrad"  },
		{"_Z6samplePU3AS4iDv3_fS1_S1_",  "textureCubeGrad", "textureGrad",   "texCUBEgrad"},

		{"_Z4dFdxf",                     "dFdx",            "dFdx",          "ddx"},
		{"_Z4dFdyf",                     "dFdy",            "dFdy",          "ddy"},

		{"_Z4dFdxDv2_f",                 "dFdx",            "dFdx",          "ddx"},
		{"_Z4dFdyDv2_f",                 "dFdy",            "dFdy",          "ddy"},

		{"_Z4dFdxDv3_f",                 "dFdx",            "dFdx",          "ddx"},
		{"_Z4dFdyDv3_f",                 "dFdy",            "dFdy",          "ddy"},

		{"_Z4dFdxDv4_f",                 "dFdx",            "dFdx",          "ddx"},
		{"_Z4dFdyDv4_f",                 "dFdy",            "dFdy",          "ddy"},
	};

	bool hasPhi(llvm::BasicBlock* bb)
	{
		llvm::BasicBlock::iterator it = bb->begin();
		llvm::BasicBlock::iterator end = bb->end();
		if (it != end)
		{
			llvm::Instruction* instruction = it;
			
			// check for phi node
			if (instruction->getOpcode() == llvm::Instruction::PHI)
				return true;
		}		
		return false;
	}

	llvm::Value* getIndices(llvm::Value* ptr, std::vector<llvm::Value*>& indices)
	{
		// check for getelementptr instruction and get field indices
		if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(ptr))
		{
			if (constantExpr->isGEPWithNoNotionalOverIndexing())
			{
				// the global variable is the first argument of getElementPointer
				ptr = constantExpr->getOperand(0);
				int numOperands = constantExpr->getNumOperands();
				for (int i = 2; i < numOperands; ++i)
					indices += constantExpr->getOperand(i);
			}
		}
		else if (llvm::GetElementPtrInst* getElementPtrInst = llvm::dyn_cast<llvm::GetElementPtrInst>(ptr))
		{
				// the global variable is the first argument of getElementPointer
				ptr = getElementPtrInst->getPointerOperand();
				llvm::GetElementPtrInst::op_iterator it = getElementPtrInst->idx_begin() + 1;
				llvm::GetElementPtrInst::op_iterator end = getElementPtrInst->idx_end();
				for (; it != end; ++it)
				{
					indices.push_back(*it);
				}
		}
		return ptr;
	}

	Pointer<GlobalPrinter> getGlobalPrinter(std::map<std::string, GlobalPrinterAccess>& globalPrinters,
		const std::string& name, int access)
	{
		std::map<std::string, GlobalPrinterAccess>::iterator it = globalPrinters.find(name);
		
		// return if found
		if (it != globalPrinters.end())
		{
			it->second.access |= access;
			return it->second.globalPrinter;
		}

		// try default with empty name
		it = globalPrinters.find(std::string());
		if (it != globalPrinters.end())
		{
			it->second.access |= access;
			return it->second.globalPrinter;
		}
		
		return null;
	}

} // anonymous namespace


void PrintWalker::doBranch(llvm::BasicBlock* basicBlock, llvm::Value* condition, llvm::BasicBlock* trueBranch,
	llvm::BasicBlock* falseBranch, llvm::BasicBlock* join, llvm::BasicBlock* stopBlock)
{							
	CodeWriter& w = this->w;
	
	// iterate over phi nodes of join basic block to create variables.
	// create variables only if join is not the stopBlock,
	// otherwise the phi variables would get created multiple times
	if (join != stopBlock)
	{
		llvm::BasicBlock::iterator it = join->begin();
		llvm::BasicBlock::iterator end = join->end();
		for (; it != end; ++it)
		{
			llvm::Instruction* instruction = it;
			
			// check for end of phi nodes
			if (instruction->getOpcode() != llvm::Instruction::PHI)
				break;

			// instruction is a phi node
				
			// print variable for phi node (also assigns variable name which is used in doJoin())
			this->printVariable(w, instruction);
			w << ";\n";
		}
	}

	// check if true branch is empty
	bool joinHasPhi = hasPhi(join);
	if (trueBranch == join && !joinHasPhi)
	{
		// no true branch

		// create if clause
		w << "if (!";
		this->printValue(w, condition, PRIO_UNARY);
		w << ")\n";

		// follow false branch until join block is hit (join is new stop block)
		w.beginScope();
		this->doBasicBlock(basicBlock, falseBranch, join);
		w.endScope();
	}
	else
	{
		// create if clause
		w << "if (";
		this->printValue(w, condition, PRIO_NONE);
		w << ")\n";

		// follow true branch until join block is hit (join is new stop block)
		w.beginScope();
		this->doBasicBlock(basicBlock, trueBranch, join);
		w.endScope();
	
		// check if there is a false branch
		if (falseBranch != join || joinHasPhi)
		{
			w << "else\n";

			// follow false branch until join block is hit (join is new stop block)
			w.beginScope();
			this->doBasicBlock(basicBlock, falseBranch, join);
			w.endScope();
		}
	}
}

void PrintWalker::doJoin(llvm::BasicBlock* predecessor, llvm::BasicBlock* basicBlock)
{
	// assigning variables of phi nodes is not necessary if the static predecessor is NULL (unknown at compile time):
	// float x;
	// if (a >= 0)
	// {
	//   if (a < 4)
	//     x = 0;
	//   else
	//     x = 1;
	//   x = x; <- not necessary. predecessor is NULL here
	// }
	// else
	// {
	//   x = 2;
	// }
	if (predecessor == NULL)
		return;
	
	CodeWriter& w = this->w;

	// assign variables that were created at the split point, e.g.
	// float x;
	// if (c1)
	//   x = 1.0f; <- we are here
	// else
	//   x = 2.0f; <- or here

	// iterate over phi nodes of join basic block
	llvm::BasicBlock::iterator it = basicBlock->begin();
	llvm::BasicBlock::iterator end = basicBlock->end();
	for (; it != end; ++it)
	{
		llvm::Instruction* instruction = it;
				
		// check for phi node
		if (instruction->getOpcode() != llvm::Instruction::PHI)
			break;
	
		// print name of phi node created in doBranch()
		this->printName(w, instruction);
		
		// assign value (via static predecessor of phi node)
		w << " = ";
		this->printInstruction(w, instruction, 0);
		w << ";\n";
	}	
}

void PrintWalker::doInstruction(llvm::BasicBlock* predecessor, llvm::Instruction* instruction)
{
	CodeWriter& w = this->w;

	// get number of uses (returns 2 for 2 or more uses)
	int numUses = getNumUses(instruction);

	// create a variable and print instruction if two or more uses
	if (numUses >= 2)
	{
		// a phi node without static predecessor already got a variable name before if clause
		if (instruction->getOpcode() != llvm::Instruction::PHI || predecessor != NULL)
		{
			this->printVariable(w, instruction);
			w << " = ";
			this->printInstruction(w, instruction, PRIO_ASSIGN);
			w << ";\n";
		}
	}
	else if (numUses == 0)
	{
		// handle instructions with no result value
		switch (instruction->getOpcode())
		{
		case llvm::Instruction::Store:
			// store
			{
				llvm::Value* value = instruction->getOperand(0);
				llvm::Value* ptr = instruction->getOperand(1);
				this->printGlobalForStore(value, ptr);
			}
			break;
		case llvm::Instruction::Call:
			// call (only discard possible)
			w << "discard;\n";
			break;
		}
	}
}

void PrintWalker::printInstruction(CodeStream& w, llvm::Instruction* instruction, int priority)
{
	BinaryOp binaryOp = BINARY_INVALID;
	switch (instruction->getOpcode())
	{
	case llvm::Instruction::Load:
		{
			llvm::Type* type = instruction->getType();
			this->printGlobalForLoad(w, type, instruction->getOperand(0), priority);
		}
		break;
	case llvm::Instruction::PHI:
		{
			llvm::PHINode* phiNode = llvm::cast<llvm::PHINode>(instruction);
			
			// get basic block that contains the phi node
			llvm::BasicBlock* basicBlock = phiNode->getParent();
			
			// check if there is a static predecessor (known at compile time)
			if (llvm::BasicBlock* predecessor = this->getPredecessor(basicBlock))
			{
				// find the incoming value
				int numIncomingValues = phiNode->getNumIncomingValues();
				for (int i = 0; i < numIncomingValues; ++i)
				{
					// check if predecessor is known at compile time
					if (phiNode->getIncomingBlock(i) == predecessor)
					{
						llvm::Value* value = phiNode->getIncomingValue(i);

						/*
							float x
							if (foo)
							{
								...
								x = value; <- we are here
							}							
						*/

						// check if we need a splat to assign a scalar to a vector
						Language language = w.getLanguage();
						bool needSplat = language.isGLSL() && getPrintType(instruction) == PRINT_VECTOR && getPrintType(value) != PRINT_VECTOR;
						if (needSplat)
						{
							printConstructor(w, instruction->getType(), INIT_SCALAR);
							w << '(';
						}						
						
						this->printValue(w, value, priority);

						if (needSplat)
							w << ')';
						
						return;
					}
				}
				
				// error: incoming value not found
				return;
			}

			// only print name since variable was created before split of CFG (e.g. if)
			this->printName(w, phiNode);
		}
		break;
	
	case llvm::Instruction::Or:
		binaryOp = BINARY_OR;
		break;
	case llvm::Instruction::Xor:
		binaryOp = BINARY_XOR;
		break;
	case llvm::Instruction::And:
		binaryOp = BINARY_AND;
		break;

	case llvm::Instruction::ICmp:
	case llvm::Instruction::FCmp:
		{
			llvm::CmpInst* cmpInst = llvm::cast<llvm::CmpInst>(instruction);
			switch (cmpInst->getPredicate())
			{
			case llvm::CmpInst::ICMP_EQ:
			case llvm::CmpInst::FCMP_OEQ:
			case llvm::CmpInst::FCMP_UEQ:
				binaryOp = BINARY_EQUAL;
				break;
			case llvm::CmpInst::ICMP_NE:
			case llvm::CmpInst::FCMP_ONE:
			case llvm::CmpInst::FCMP_UNE:
				binaryOp = BINARY_NOT_EQUAL;
				break;
			case llvm::CmpInst::ICMP_ULT:
			case llvm::CmpInst::ICMP_SLT:
			case llvm::CmpInst::FCMP_OLT:
			case llvm::CmpInst::FCMP_ULT:
				binaryOp = BINARY_LESS;
				break;
			case llvm::CmpInst::ICMP_UGT:
			case llvm::CmpInst::ICMP_SGT:
			case llvm::CmpInst::FCMP_OGT:
			case llvm::CmpInst::FCMP_UGT:
				binaryOp = BINARY_GREATER;
				break;
			case llvm::CmpInst::ICMP_ULE:
			case llvm::CmpInst::ICMP_SLE:
			case llvm::CmpInst::FCMP_OLE:
			case llvm::CmpInst::FCMP_ULE:
				binaryOp = BINARY_LESS_EQUAL;
				break;
			case llvm::CmpInst::ICMP_UGE:
			case llvm::CmpInst::ICMP_SGE:
			case llvm::CmpInst::FCMP_OGE:
			case llvm::CmpInst::FCMP_UGE:
				binaryOp = BINARY_GREATER_EQUAL;
				break;
			default:
				;
			}
		}
		break;
	case llvm::Instruction::Shl:
		binaryOp = BINARY_SHIFT_LEFT;
		break;
	case llvm::Instruction::LShr:
	case llvm::Instruction::AShr:
		binaryOp = BINARY_SHIFT_RIGHT;
		break;
	case llvm::Instruction::Add:
	case llvm::Instruction::FAdd:
		binaryOp = BINARY_ADD;
		break;
	case llvm::Instruction::Sub:
	case llvm::Instruction::FSub:
		// detect 0 - x = -x
		if (getConstantDouble(instruction->getOperand(0)) == 0.0)
		{
			w << '-';
			this->printValue(w, instruction->getOperand(1), PRIO_UNARY);
			return;
		}

		binaryOp = BINARY_SUB;
		break;
	case llvm::Instruction::Mul:
	case llvm::Instruction::FMul:
		binaryOp = BINARY_MUL;
		break;
	case llvm::Instruction::UDiv:
	case llvm::Instruction::SDiv:
	case llvm::Instruction::FDiv:		
		// detect x / sqrt(x) = x * inversesqrt(x)
		{			
			Language language = w.getLanguage();
			const char* invSquareRootName = NULL;
			if (language.isGLSL())
				invSquareRootName = "inversesqrt";
			else if (language.isHLSL())
				invSquareRootName = "rsqrt";
			
			if (invSquareRootName != NULL)
			{
				// track right operand			
				int numElements = getNumElements(instruction);
				TrackValue values[4];
				trackValues(values, numElements, instruction->getOperand(1));
				llvm::Value* value = values[0].value;
				
				// check if it is a call instruction
				if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(value))
				{				
					// check if all values are the same and index 0 (e.g. sqrt(a).xxx)
					int i;
					for (i = 0; i < numElements; ++i)
					{
						if (values[i].value != value || values[i].index != 0)
							break;
					}
				
					// currently only if sqrt is scalar
					if (i == numElements && getNumElements(callInst) == 1 && getNumUses(callInst) == 1)
					{
						llvm::Function* function = callInst->getCalledFunction();
						if (function != NULL)
						{
							if (function->getName() == "sqrt")
							{
								// found call to sqrt
								llvm::Value* left = instruction->getOperand(0);
								llvm::Value* right = callInst->getArgOperand(0);

								// cast to vector if binary op is vector, but the operands are scalar
								bool castInstruction = getPrintType(instruction) == PRINT_VECTOR && getPrintType(left) != PRINT_VECTOR
									&& getPrintType(right) != PRINT_VECTOR;

								// left can be omitted if equal to 1.0
								bool useLeft = getConstantDouble(left) != 1.0;

								if (castInstruction)
									printConstructor(w, instruction->getType(), INIT_SCALAR);

								if (castInstruction || (useLeft && priority > PRIO_MUL))
									w << '(';
								
								// print left if not constant 1.0
								if (useLeft)
								{
									this->printValue(w, left, PRIO_MUL);
									w << " * ";
								}
									
								// print inverse square root of right
								w << invSquareRootName << '(';
								this->printValue(w, right, PRIO_NONE);
								w << ')';

								if (castInstruction || (useLeft && priority > PRIO_MUL))
									w << ')';

								// next instruction
								return;
							}
						}
					}
				}
			}
		}
		
		binaryOp = BINARY_DIV;
		break;
	case llvm::Instruction::URem:
	case llvm::Instruction::SRem:
		binaryOp = BINARY_MOD;
		break;

	case llvm::Instruction::ZExt:
	case llvm::Instruction::SExt:
	case llvm::Instruction::Trunc:
	case llvm::Instruction::FPExt:
	case llvm::Instruction::FPTrunc:
		// pass through
		this->printValue(w, instruction->getOperand(0), priority);
		break;
			
	case llvm::Instruction::SIToFP:
	case llvm::Instruction::UIToFP:
		// integer to float conversion
		if (w.getLanguage().isJS())
		{
			// javascript: pass through
			this->printValue(w, instruction->getOperand(0), priority);
		}
		else
		{
			printConstructor(w, instruction->getType(), INIT_VECTOR);
			w << '(';
			this->printValue(w, instruction->getOperand(0), PRIO_NONE);
			w << ')';
		}
		break;

	case llvm::Instruction::FPToSI:
	case llvm::Instruction::FPToUI:
		// float to integer conversion
		if (w.getLanguage().isJS())
		{
			// javascript
			llvm::Value* op0 = instruction->getOperand(0);
			
			// do nothing if result of floor(), ceil() or round()
			if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(op0))
			{
				llvm::Function* function = callInst->getCalledFunction();
				if (function != NULL)
				{
					// get demangled function name
					std::string name = NameDemangler::demangle(function->getNameStr());
					if (name == "floor" || name == "ceil" || name == "round")
					{
						// pass through
						this->printValue(w, op0, priority);
						break;
					}
				}
			}
			
			// otherwise or with zero to simulate the truncating effect of float to integer conversion
			if (priority > PRIO_OR)
				w << '(';
			this->printValue(w, op0, PRIO_OR);
			w << " | 0";
			if (priority > PRIO_OR)
				w << ')';
		}
		else
		{
			printConstructor(w, instruction->getType(), INIT_VECTOR);
			w << '(';
			this->printValue(w, instruction->getOperand(0), PRIO_NONE);
			w << ')';
		}
		break;
		
	case llvm::Instruction::Select:
		{
			llvm::Value* condition = instruction->getOperand(0);
			llvm::Value* op0 = instruction->getOperand(1);
			llvm::Value* op1 = instruction->getOperand(2);

			bool selectIsVector = getPrintType(instruction) == PRINT_VECTOR;
			bool conditionIsVector = getPrintType(condition) == PRINT_VECTOR;
			bool op0IsVector = getPrintType(op0) == PRINT_VECTOR;
			bool op1IsVector = getPrintType(op1) == PRINT_VECTOR;

			// cast to vector if select is vector, but all operands are scalar
			bool castInstruction = selectIsVector && !conditionIsVector && !op0IsVector && !op1IsVector;
			if (castInstruction)
				printConstructor(w, instruction->getType(), INIT_SCALAR);
				
			if (castInstruction || priority > PRIO_CONDITIONAL)
				w << '(';
			
			this->printValue(w, condition, PRIO_CONDITIONAL);
			
			w << " ? ";
			{
				// cast to vector if this operand is scalar and the other is vector
				bool cast = !op0IsVector && op1IsVector;
				int p = PRIO_CONDITIONAL;
				if (cast)
				{
					printConstructor(w, op0->getType(), INIT_SCALAR);
					w << '(';
					p = PRIO_NONE;
				}

				this->printValue(w, op0, p);
				
				if (cast)
					w << ')';
			}
			w << " : ";
			{
				// cast to vector if this operand is scalar and the other is vector
				bool cast = op0IsVector && !op1IsVector;
				int p = PRIO_CONDITIONAL;
				if (cast)
				{
					printConstructor(w, op1->getType(), INIT_SCALAR);
					w << '(';
					p = PRIO_NONE;
				}
			
				this->printValue(w, op1, p);
			
				if (cast)
					w << ')';
			}
			
			if (castInstruction || priority > PRIO_CONDITIONAL)
				w << ')';			
		}
		break;
		
	case llvm::Instruction::InsertElement:
		{
			// insert operand1 into operand0 at index operand2

			// check if the whole vector gets assembled
			TrackValue values[4];
			int numElements = getNumElements(instruction);
			trackValuesInsertElement(values, numElements, instruction);

			return this->printVector(w, instruction, values, numElements, priority);
		}
		break;				
	case llvm::Instruction::ExtractElement:
		{
			TrackValue values[1];
			trackValuesExtractElement(values, 1, instruction);
			
			return this->printVector(w, instruction, values, 1, priority);
		}
		break;
	case llvm::Instruction::ShuffleVector:
		{
			TrackValue values[4];
			int numElements = getNumElements(instruction);
			trackValuesShuffleVector(values, numElements, instruction);				

			return this->printVector(w, instruction, values, numElements, priority);
		}
		break;
			
	case llvm::Instruction::Call:
		{
			llvm::CallInst* callInst = llvm::cast<llvm::CallInst>(instruction);
	
			llvm::Function* function = callInst->getCalledFunction();
			if (function != NULL)
			{
				// get demangled function name
				std::string mangledName = function->getNameStr();
				std::string name = NameDemangler::demangle(mangledName);
				
				Language language = w.getLanguage();

				if (language.isJS() && name == "fmod")
				{
					// fmod is operator % in javascript
					int numOperands = callInst->getNumArgOperands();
					if (numOperands == 2)
					{
						this->printValue(w, callInst->getArgOperand(0), PRIO_MUL);
						w << " % ";
						this->printValue(w, callInst->getArgOperand(1), PRIO_MUL + 1);
					}
					
					return;
				}				
			
				const FunctionInfo* functionInfo = find(boost::begin(functionInfos), boost::end(functionInfos), name, FunctionInfoEquals());
				if (functionInfo != boost::end(functionInfos))
				{
					// standard function (mostly math): replace function name
					if (language.isCPP() || language.isHLSL())
						w << functionInfo->cppName;
					else if (language.isGLSL())
						w << functionInfo->glslName;
					else if (language.isJS())
						w << functionInfo->jsName;
					else
						w << name;
				}
				else if (startsWith(name, "convert_"))
				{
					// convert function
					// get argument
					llvm::Value* arg = callInst->getArgOperand(0);

					// convert functions
					if (language.isGLSL() || language.isHLSL())
					{
						// cast argument to vector if it is scalarized
						llvm::Type* argType = arg->getType();
						bool castArg = getPrintType(arg) == PRINT_VECTOR_SCALAR;
						if (castArg)
							printConstructor(w, argType, INIT_SCALAR), w << '(';						
						
						// get destination type
						ShaderType type = getShaderType(callInst->getType());
					
						printConstructor(w, type, INIT_VECTOR);
						w << '(';
						bool brace = false;
						if (type.type == ShaderType::INT)
						{
							// convert to integer type using round/floor/ceil
							if (endsWith(name, "_rte"))
								w << "round(", brace = true;
							else if (endsWith(name, "_rtn"))
								w << "floor(", brace = true;
							else if (endsWith(name, "_rtp"))
								w << "ceil(", brace = true;
						}
						this->printValue(w, arg, PRIO_NONE);
						if (brace)
							w << ')';
						w << ')';
						if (castArg)
							w << ')';

						return;
					}
					else if (language.isJS())
					{
						// get destination type
						VectorInfo type(substring(name, 8, find(name, '_', 8, name.size())));						

						if (type.type <= VectorInfo::ULONG)
						{
							// convert to integer type (internally a double) using round/floor/ceil or the '| 0' trick
							Priority p = PRIO_NONE;
							if (endsWith(name, "_rte"))
								w << "Math.round(";
							else if (endsWith(name, "_rtn"))
								w << "Math.floor(";
							else if (endsWith(name, "_rtp"))
								w << "Math.ceil(";
							else
							{
								if (priority > PRIO_OR)
									w << '(';
								p = PRIO_OR;
							}

							this->printValue(w, arg, p);
							
							if (p == PRIO_OR)
							{
								w << " | 0";
								if (priority > PRIO_OR)
									w << ')';
							}
							else
							{
								w << ')';
							}
						}
						else
						{						
							// convert to float: use directly
							this->printValue(w, arg, priority);
						}
						return;						
					}
	
					// print convert function
					w << name;
				}
				else
				{
					const ShaderInfo* shaderInfo = find(boost::begin(shaderInfos), boost::end(shaderInfos), mangledName, ShaderInfoEquals());
					if (shaderInfo != boost::end(shaderInfos))
					{
						// shader function
						if (language.isOldGLSL())
							w << shaderInfo->oldGLSLName;
						else if (language.isGLSL())
							w << shaderInfo->glslName;
						else if (language.isHLSL())
							w << shaderInfo->hlslName;
						else
							w << name;
					}
					else
					{
						// other function (e.g. evalSplineTrack1)
						w << "engine." << name;
					}
				}
				
				// print arguments
				w << '(';
				int numOperands = callInst->getNumArgOperands();
				for (int i = 0; i < numOperands; ++i)
				{
					if (i > 0)
						w << ", ";

					llvm::Value* arg = callInst->getArgOperand(i);

					// check for pointer argument
					const llvm::PointerType* pointerType = llvm::dyn_cast<llvm::PointerType>(arg->getType());
					if (pointerType != NULL && pointerType->getAddressSpace() == 0)
					{
						// array. do the same as this->printGlobalForLoad, but omit last index since we want to pass the array as argument
						// and not a pointer to the first element

						// get access indices
						std::vector<llvm::Value*> indices;
						llvm::Value* global = getIndices(arg, indices);
						
						// remove last index
						indices.pop_back();

						// get name and type of global
						std::string name = global->getNameStr();

						// lookup global printer by name
						Pointer<GlobalPrinter> globalPrinter = getGlobalPrinter(this->globalPrinters, name, GlobalPrinterAccess::READ);
						if (globalPrinter != null)
						{
							globalPrinter->printForLoad(w, pointerType->getElementType(), global, indices, *this, PRIO_NONE);
						}
					}
					else
					{
						// scalar, vector or object type

						// cast argument to vector if it is scalarized
						//! not needed for second arg of min() and max()
						llvm::Type* argType = arg->getType();
						bool castArg = getPrintType(arg) == PRINT_VECTOR_SCALAR;
						if (castArg)
							printConstructor(w, argType, INIT_SCALAR), w << '(';
						
						this->printValue(w, arg, PRIO_NONE);

						if (castArg)
							w << ')';
					}
				}
				w << ')';
			}
		}
		break;
	default:
		// error: instruction not supported
		break;
	}
	
	if (binaryOp != BINARY_INVALID)
	{
		Language language = w.getLanguage();
		const BinaryOperator& op = binaryOperators[binaryOp];

		llvm::Value* left = instruction->getOperand(0);
		llvm::Value* right = instruction->getOperand(1);

		// cast to vector if binary op is vector, but the operands are scalar
		bool castToVector = getPrintType(instruction) == PRINT_VECTOR && getPrintType(left) != PRINT_VECTOR
			&& getPrintType(right) != PRINT_VECTOR;

		// javascript: cast result of division to int if arguments are int
		bool castToInt = binaryOp == language.isJS() && BINARY_DIV
			&& left->getType()->isIntegerTy() && right->getType()->isIntegerTy();

		if (castToVector)
			printConstructor(w, instruction->getType(), INIT_SCALAR);
		
		if (castToVector || castToInt || priority > op.priority)
			w << '(';
		
		// left value
		this->printValue(w, left, op.priority);
		
		// operator
		w << ' ' << op.text;
		
		// use "||", "^^" and "&&" instead of "|", "^" and "&" for glsl
		if (language.isGLSL() && binaryOp >= BINARY_OR && binaryOp <= BINARY_AND)
			w << op.text;
		w << ' ';
		
		// right value
		this->printValue(w, right, op.rightPriority);
		
		if (castToInt)
			w << " | 0";
		
		if (castToVector || castToInt || priority > op.priority)
			w << ')';
	}
}

void PrintWalker::printValue(CodeStream& w, llvm::Value* value, int priority)
{
	if (llvm::isa<llvm::Constant>(value))
	{
		printConstant(w, value, PRECISION_HIGH, PRECISION_HIGH);
		return;
	}

	// check if value is an instruction with one use
	if (getNumUses(value) == 1)
	{
		if (llvm::Instruction* instruction = llvm::dyn_cast<llvm::Instruction>(value))
		{
			this->printInstruction(w, instruction, priority);
			return;
		}
	}

	// print name of temp variable
	this->printName(w, value);
}


// helpers for printVector

struct SwizzleValue
{
	llvm::Value* value;
	int swizzle;
	int numComponents;
	
	SwizzleValue()
		: value(NULL), swizzle(0), numComponents(0) {}
};

// check if this is a swizzle, i.e. only one value (or undef) occurs. if yes, return the swizzled value, otherwise NULL
static TrackValue* isSwizzle(TrackValue* values, int numElements)
{
	TrackValue* value = NULL;
	for (int i = 0; i < numElements; ++i)
	{
		TrackValue* v = &values[i];
		if (v->value != NULL)
		{
			if (value == NULL)
				value = v;
			else if (v->value != value->value)
				return NULL;
		}
	}
	return value;
}

// check for neutral swizzle that does not change the value
static bool isNeutralSwizzle(int numInputDimensions, int swizzle, int numOutputDimensions)
{
	// numElements is the number of dimension of the value that gets swizzled
	// float4 a;
	// a.xyzw;
	return numOutputDimensions == numInputDimensions
		&& ((swizzle ^ 0x3210) & ~(~0 << numOutputDimensions * 4)) == 0;
}

static void printSwizzle(CodeStream& w, int numInputDimensions, int swizzle, int numOutputDimensions)
{
	// check if swizzle has no effect (always true for scalar value)
	if (isNeutralSwizzle(numInputDimensions, swizzle, numOutputDimensions))
		return;
		
	w << ".";
	for (int i = 0; i < numOutputDimensions; ++i)
	{
		int index = (swizzle >> i * 4) & 0x0f;
		w << "xyzw"[index];
	}
}


struct SwizzleInfo
{
	int numReferences;
	bool isVector;
	
	SwizzleInfo()
		: numReferences(0), isVector() {}
};
typedef std::pair<llvm::Value* const, SwizzleInfo> SwizzleInfoPair;


void PrintWalker::printVector(CodeStream& w, llvm::Value* llvmValue, TrackValue* values, int numElements, int priority)
{
	llvm::Type* type = llvmValue->getType();

	if (TrackValue* value = isSwizzle(values, numElements))
	{
		// swizzle, e.g. a.xyz
		int swizzle = 0;
		for (int i = 0; i < numElements; ++i)
		{
			// fill in undef values, e.g. a.x?z becomes a.xxz
			int index = values[i].value != NULL ? values[i].index : value->index;
			swizzle |= index << i * 4;
		}
		
		bool llvmValueIsVector = getPrintType(llvmValue) == PRINT_VECTOR;
		bool valueIsVector = getPrintType(value->value) == PRINT_VECTOR;

		// number of input dimensions of value that is swizzled
		int numInputDimensions = getNumElements(value->value);
	
		// number of output dimensions, e.g. a.xyz has 3 output dimensions. if scalar, e.g. a.xxxx, reduce to a.x
		int numOutputDimensions = llvmValueIsVector ? numElements : 1;

		// need to cast if value is vector but the swizzled value is a scalar
		bool cast = llvmValueIsVector && !valueIsVector;
		if (cast)
		{
			printConstructor(w, type, INIT_SCALAR);
			w << '(';
		}

		this->printValue(w, value->value, isNeutralSwizzle(numInputDimensions, swizzle, numOutputDimensions) ? priority : PRIO_MEMBER);

		if (cast)
			w << ')';

		if (valueIsVector)
		{
			// swizzled value is a vector (but result may be a scalar, e.g. a.x)
			printSwizzle(w, numInputDimensions, swizzle, numOutputDimensions);
		}
	}
	else
	{
		// generic, e.g. float4(a.xy, b.w, a.z)

		/*
			this tracks the number of references for each value to create temporary variables.
			e.g. for (a.x, b.y. a.zw) the number of references for a is 2 and for b 1
		*/
		std::map<llvm::Value*, SwizzleInfo> swizzleInfos;

		// join adjacent values (only vectors because scalars can't be swizzled)
		SwizzleValue swizzleValues[4];
		SwizzleValue* swizzleValue = swizzleValues - 1;
		int numSwizzleValues = 0;
		
		llvm::Value* last = NULL;
		for (int i = 0; i < numElements; ++i)
		{
			llvm::Value* value = values[i].value;
			int index = values[i].index;
			if (value == NULL)
			{
				// value is undef
				last = NULL;
				++swizzleValue;
				++numSwizzleValues;
			}
			else
			{
				bool valueIsVector = getPrintType(value) == PRINT_VECTOR;

				// check if new value or scalar value (prevent swizzle of scalar)
				if (value != last || !valueIsVector)
				{
					SwizzleInfo& swizzleInfo = swizzleInfos[value];
					swizzleInfo.isVector = valueIsVector;

					// increment number of references for this value
					++swizzleInfo.numReferences;
					if (swizzleInfo.numReferences == 2 && getNumUses(value) == 1)
					{
						// create a temp variable if we have reached 2 references

						// get real code writer in case w is a temp code writer
						CodeWriter& w2 = this->w;

						// save current line
						std::string line;
						w2.swapLine(line);

						// print variable and value
						this->printVariable(w2, value);
						w2 << " = ";
						this->printValue(w2, value, PRIO_ASSIGN);

						// restore current line
						w2.swapLine(line);
					}
					
					// update current value
					last = value;
										
					// new swizzle value (stores swizzle of value, e.g. value.xyz)
					++swizzleValue;
					swizzleValue->value = value;
					++numSwizzleValues;
				}
				
				swizzleValue->swizzle |= index << swizzleValue->numComponents * 4;
				swizzleValue->numComponents++;
			}
		}

		// print values
		printConstructor(w, type, INIT_SCALAR_LIST);
		w << '(';
		for (int i = 0; i < numSwizzleValues; ++i)
		{
			if (i != 0)
				w << ", ";
						
			llvm::Value* value = swizzleValues[i].value;
			if (value == NULL)
			{
				// undef
				printZero(w, type);
			}
			else
			{
				int numElements = getNumElements(value);
				int swizzle = swizzleValues[i].swizzle;
				int numOutputDimensions = swizzleValues[i].numComponents;
				const SwizzleInfo& swizzleInfo = swizzleInfos[value];
				
				if (!swizzleInfo.isVector)
				{
					// is a scalar
					this->printValue(w, value, PRIO_NONE);
				}
				else
				{
					// need to protect the expression by brackts if we swizzle and the expression is not a temp variable
					bool doSwizzle = !isNeutralSwizzle(numElements, swizzle, numOutputDimensions)
						&& swizzleInfo.numReferences < 2 && getNumUses(value) == 1;
					
					this->printValue(w, value, doSwizzle ? PRIO_MEMBER : PRIO_NONE);
									
					printSwizzle(w, numElements, swizzle, numOutputDimensions);
				}
			}
		}
		w << ')';
	}
}

void PrintWalker::printName(CodeStream& w, llvm::Value* value)
{
	// lookup value
	std::map<llvm::Value*, ValueName>::const_iterator it = this->names.find(value);
	if (it != this->names.end())
	{		
		w << it->second.name;
	}

	// error: name not found
}


void PrintWalker::printVariable(CodeStream& w, llvm::Value* value)
{
	// create variable name
	std::string name = this->nameGenerator.get();

	// add to map
	this->names[value] = ValueName(name);

	// get type
	llvm::Type* type = value->getType();

	// scalarize
	if (getPrintType(value) == PRINT_VECTOR_SCALAR)
		type = llvm::cast<llvm::VectorType>(type)->getElementType();
	
	// print variable
	printShaderVariable(w, ShaderVariable(name, getShaderType(type)), PRECISION_HIGH);
}

void PrintWalker::printGlobalForLoad(CodeStream& w, llvm::Type* type, llvm::Value* ptr, int priority)
{
	// get access indices
	std::vector<llvm::Value*> indices;
	llvm::Value* global = getIndices(ptr, indices);

	// get name and type of global
	std::string name = global->getNameStr();

	// lookup global printer by name
	Pointer<GlobalPrinter> globalPrinter = getGlobalPrinter(this->globalPrinters, name, GlobalPrinterAccess::READ);
	if (globalPrinter != null)
	{
		globalPrinter->printForLoad(w, type, global, indices, *this, priority);
	}
	else
	{
		// error: no global getter for this global
	}
}

void PrintWalker::printGlobalForStore(llvm::Value* value, llvm::Value* ptr)
{
	// get access indices
	std::vector<llvm::Value*> indices;
	llvm::Value* global = getIndices(ptr, indices);

	// get name and type of global
	std::string name = global->getNameStr();

	// lookup global printer by name
	Pointer<GlobalPrinter> globalPrinter = getGlobalPrinter(this->globalPrinters, name, GlobalPrinterAccess::WRITE);
	if (globalPrinter != null)
		globalPrinter->printForStore(this->w, value, global, indices, *this);

	// error: no global getter for this global
}

} // namespace digi

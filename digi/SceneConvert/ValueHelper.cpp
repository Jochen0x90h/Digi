#include <llvm/Constants.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Standard.h>

#include "ValueHelper.h"


namespace digi {

bool isBool(llvm::Type* type)
{
	if (llvm::IntegerType* intType = llvm::dyn_cast<llvm::IntegerType>(type))
		return intType->getBitWidth() <= 8;
	return false;
}

int getNumElements(llvm::Type* type)
{
	if (type->isVectorTy())
	{
		const llvm::VectorType* vectorType = llvm::cast<llvm::VectorType>(type);
		return vectorType->getNumElements();
	}
	return 1;
}

int getNumUses(const llvm::Value* value)
{
	llvm::Value::const_use_iterator it = value->use_begin(), end = value->use_end();
	int n = 0;
	for (; it != end && n < 2; ++it)
	{
		++n;
	}
	return n;
}

int getConstantIndex(llvm::Value* value)
{
	if (llvm::ConstantInt* constantInt = llvm::dyn_cast<llvm::ConstantInt>(value))
	{
		return int(constantInt->getZExtValue());
	}		
	return -1;
}

int4 getConstantIndexVector(llvm::Value* value)
{
	if (llvm::ConstantVector* constantVector = llvm::dyn_cast<llvm::ConstantVector>(value))
	{
		// value is constant vector
		if (llvm::Constant* splat = constantVector->getSplatValue())
		{
			// all components are equal
			return splat4(getConstantIndex(splat));
		}
		
		// get components
		llvm::SmallVector<llvm::Constant*, 4> elements;
		constantVector->getVectorElements(elements);
		int4 indices = splat4(-1);
		int numElements = min(int(elements.size()), 4);
		for (int i = 0; i < numElements; ++i)
		{
			indices[i] = getConstantIndex(elements[i]);
		}
		return indices;
	}

	if (llvm::isa<llvm::ConstantAggregateZero>(value))
	{
			// all components are zero
			return splat4(0);
	}	

	// all components are undefined
	return splat4(-1);
}

boost::optional<double> getConstantDouble(llvm::Value* value)
{
	if (llvm::ConstantInt* constantInt = llvm::dyn_cast<llvm::ConstantInt>(value))
	{
		return double(constantInt->getZExtValue());
	}
	else if (llvm::ConstantFP* constantFP = llvm::dyn_cast<llvm::ConstantFP>(value))
	{
		llvm::APFloat f = constantFP->getValueAPF();
		bool reversible;
		f.convert(llvm::APFloat::IEEEdouble, llvm::APFloat::rmNearestTiesToEven, &reversible);

		return f.convertToDouble();
	}
	else if (llvm::ConstantVector* constantVector = llvm::dyn_cast<llvm::ConstantVector>(value))
	{
		if (llvm::Constant* splat = constantVector->getSplatValue())
		{
			// all components are equal
			return getConstantDouble(splat);
		}
	}
	else if (llvm::isa<llvm::ConstantAggregateZero>(value))
	{
		return 0.0;
	}
	return null;
}

llvm::Value* createFloatVector(llvm::Type* type, float4 value)
{
	if (type->isVectorTy())
	{
		const llvm::VectorType* vectorType = llvm::cast<llvm::VectorType>(type);
		int numElements = vectorType->getNumElements();
		type = vectorType->getElementType();
		
		llvm::Constant* constants[4];
		for (int i = 0; i < numElements; ++i)
		{
			constants[i] = llvm::ConstantFP::get(type, double(value[i]));
		}
		return llvm::ConstantVector::get(llvm::ArrayRef<llvm::Constant*>(constants, numElements));
	}
	return llvm::ConstantFP::get(type, double(value[0]));
}

/*
bool equals(llvm::Value* a, llvm::Value* b)
{
	if (a == b)
		return true;
	
	if (llvm::ConstantInt* constantInta = llvm::dyn_cast<llvm::ConstantInt>(a))
	{
		if (llvm::ConstantInt* constantIntb = llvm::dyn_cast<llvm::ConstantInt>(b))
		{
			return constantInta->getZExtValue() == constantIntb->getZExtValue();
		}
	}
		
	if (llvm::ConstantFP* constantFPa = llvm::dyn_cast<llvm::ConstantFP>(a))
	{
		if (llvm::ConstantFP* constantFPb = llvm::dyn_cast<llvm::ConstantFP>(b))
		{
			bool reversible;

			llvm::APFloat fa = constantFPa->getValueAPF();
			fa.convert(llvm::APFloat::IEEEdouble, llvm::APFloat::rmNearestTiesToEven, &reversible);

			llvm::APFloat fb = constantFPb->getValueAPF();
			fb.convert(llvm::APFloat::IEEEdouble, llvm::APFloat::rmNearestTiesToEven, &reversible);

			return fa.convertToDouble() == fb.convertToDouble();
		}
	}
	
	return false;
}
*/
VectorInfo getVectorInfo(llvm::Type* type)
{
	// get dimension
	int dimension = 1;
	if (type->isVectorTy())
	{
		const llvm::VectorType* vectorType = llvm::cast<llvm::VectorType>(type);
		dimension = vectorType->getNumElements();
		type = vectorType->getElementType();
	}
	
	if (type->isIntegerTy())
	{
		return VectorInfo(isBool(type) ? VectorInfo::BOOL : VectorInfo::INT, dimension);
	}
	else if (type->isFloatingPointTy())
	{
		return VectorInfo(VectorInfo::FLOAT, dimension);
	}
	return VectorInfo();
}


PrintType getPrintType(llvm::Value* value)
{
	if (value->getType()->isVectorTy())
	{
		// vector type

		// check for instruction
		if (llvm::Instruction* instruction = llvm::dyn_cast<llvm::Instruction>(value))
		{
			switch (instruction->getOpcode())
			{
			case llvm::Instruction::InsertElement:
				
				// is scalar if an element is inserted into an undef vector at index 0
				return (llvm::isa<llvm::UndefValue>(instruction->getOperand(0)) && getConstantIndex(instruction->getOperand(2)) == 0) ? PRINT_VECTOR_SCALAR : PRINT_VECTOR;

			case llvm::Instruction::ShuffleVector:
				
				// is scalar if shuffle selector is scalar
				return getPrintType(instruction->getOperand(2));
			
			default:
				
				// is a vector
				return PRINT_VECTOR;
			}
		}
		
		// vector is printed as scalar if all elements of constant vector have the same value
		if (llvm::ConstantVector* constantVector = llvm::dyn_cast<llvm::ConstantVector>(value))
			return constantVector->getSplatValue() != NULL ? PRINT_VECTOR_SCALAR : PRINT_VECTOR;

		// vector is printed as scalar if zeroinitializer
		return llvm::isa<llvm::ConstantAggregateZero>(value) ? PRINT_VECTOR_SCALAR : PRINT_VECTOR;
	}

	// scalar type
	return PRINT_SCALAR;
}

	
void trackValues(TrackValue* values, int numElements, llvm::Value* value)
{
	// check if value is an instruction with one use
	if (getNumUses(value) == 1)
	{
		llvm::Instruction* instruction = llvm::dyn_cast<llvm::Instruction>(value);
		if (instruction != NULL)
		{
			switch (instruction->getOpcode())
			{
			case llvm::Instruction::InsertElement:
				trackValuesInsertElement(values, numElements, instruction);
				return;
			case llvm::Instruction::ExtractElement:
				trackValuesExtractElement(values, numElements, instruction);
				return;
			case llvm::Instruction::ShuffleVector:
				trackValuesShuffleVector(values, numElements, instruction);
				return;
			}
		}
	}
	
	// set value
	if (llvm::ConstantVector* constantVector = llvm::dyn_cast<llvm::ConstantVector>(value))
	{
		llvm::Value* splatValue = constantVector->getSplatValue();
		if (splatValue != NULL)
		{
			for (int i = 0; i < numElements; ++i)
			{
				values[i].value = splatValue;
				values[i].index = 0;
			}		
		}
		else
		{
			llvm::SmallVector<llvm::Constant*, 4> elements;
			constantVector->getVectorElements(elements);
			for (int i = 0; i < numElements; ++i)
			{
				values[i].value = elements[i];
				values[i].index = 0;
			}		
		}
	}
	else
	{
		for (int i = 0; i < numElements; ++i)
		{
			values[i].value = value;
			values[i].index = i;
		}
	}
}

void trackValuesInsertElement(TrackValue* values, int numElements, llvm::Instruction* instruction)
{
	// insert operand1 into vector operand0 at index operand2
	
	// get index from operand2
	int index = getConstantIndex(instruction->getOperand(2));
	if (index == -1)
	{
		// index is variable
		values[0].value = instruction;
		values[0].index = 0;
	}
	else
	{
		// track vector
		trackValues(values, numElements, instruction->getOperand(0));

		// track value that is inserted into the vector
		trackValues(values + index, 1, instruction->getOperand(1));
	}
}

void trackValuesExtractElement(TrackValue* values, int numElements, llvm::Instruction* instruction)
{
	// extract element at index operand1 from operand0
		
	// extract at index operand1
	int index = getConstantIndex(instruction->getOperand(1));						
	if (index == -1)
	{
		// index is variable
		values[0].value = instruction;
		values[0].index = 0;
	}
	else
	{
		int newNumDimensions = getNumElements(instruction->getOperand(0));
		TrackValue newValues[4];

		// get operand0
		trackValues(newValues, newNumDimensions, instruction->getOperand(0));

		values[0] = newValues[index];
	}
}

void trackValuesShuffleVector(TrackValue* values, int numElements, llvm::Instruction* instruction)
{
	// shuffle operand0 and operand1 with operand2

	int newNumDimensions0 = getNumElements(instruction->getOperand(0));
	int newNumDimensions1 = getNumElements(instruction->getOperand(1));
	TrackValue newValues[8];									

	// get operand0
	trackValues(newValues, newNumDimensions0, instruction->getOperand(0));															

	// get operand1
	trackValues(newValues + newNumDimensions0, newNumDimensions1, instruction->getOperand(1));															

	// get operand2 (list of indices)
	int4 indices = getConstantIndexVector(instruction->getOperand(2));

	for (int i = 0; i < numElements; ++i)
	{
		int index = indices[i];
		if (index == -1)
		{
			// undef
			values[i] = TrackValue(NULL, -1);
		}
		else
		{
			values[i] = newValues[index];
		}
	}
}


} // namespace digi

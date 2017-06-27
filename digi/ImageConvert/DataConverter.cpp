#include <cmath>

// note: llvm needs __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS to be defined

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/PassManager.h>

#include <llvm/Analysis/Verifier.h>

#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <llvm/Transforms/Scalar.h>

#include <llvm/Target/TargetData.h>

#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TypeBuilder.h>

#include <clang/Frontend/CodeGenOptions.h>
#include <clang/CodeGen/ModuleBuilder.h>

#include <digi/Utility/StringUtility.h>
#include <digi/Utility/Convert.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/CodeWriterFunctions.h>
#include <digi/EngineVM/Compiler.h>
#include <digi/EngineVM/VMFile.h>

#include "DataConverter.h"


namespace digi {

VectorInfo toVectorInfo(BufferFormat format)
{	
	VectorInfo::Type type = VectorInfo::INVALID;
	bool isStandard = format.getLayoutType() == BufferFormat::STANDARD;
	switch (format.getComponentSize())
	{
	case 1:
		switch (format.type)
		{
		case BufferFormat::NORM:
		case BufferFormat::INT:
			type = VectorInfo::BYTE;
			break;
		default:
			type = isStandard ? VectorInfo::UBYTE : VectorInfo::BYTE;
		}
		break;
	case 2:
		switch (format.type)
		{
		case BufferFormat::NORM:
		case BufferFormat::INT:
		case BufferFormat::FLOAT:
			type = VectorInfo::SHORT;
			break;
		default:
			type = isStandard ? VectorInfo::USHORT : VectorInfo::SHORT;
		}
		break;
	case 4:
		switch (format.type)
		{
		case BufferFormat::FLOAT:
			if (isStandard)
			{
				// use float only if standard format
				type = VectorInfo::FLOAT;
				break;
			}
			// fall through
		case BufferFormat::NORM:
		case BufferFormat::INT:
			type = VectorInfo::INT;
			break;
		default:
			type = isStandard ? VectorInfo::UINT : VectorInfo::INT;
		}
		break;
	}
	return VectorInfo(type, format.getNumComponents());
}

BufferFormat toBufferFormat(VectorInfo type)
{
	BufferFormat format;
	switch (type.type)
	{
	case VectorInfo::BYTE:
		format.type = BufferFormat::INT;
		format.layout = BufferFormat::Layout(BufferFormat::X8 + type.numRows - 1);
		break;
	case VectorInfo::UBYTE:
		format.type = BufferFormat::UINT;
		format.layout = BufferFormat::Layout(BufferFormat::X8 + type.numRows - 1);
		break;
	case VectorInfo::SHORT:
		format.type = BufferFormat::INT;
		format.layout = BufferFormat::Layout(BufferFormat::X16 + type.numRows - 1);
		break;
	case VectorInfo::USHORT:
		format.type = BufferFormat::UINT;
		format.layout = BufferFormat::Layout(BufferFormat::X16 + type.numRows - 1);
		break;
	case VectorInfo::INT:
		format.type = BufferFormat::INT;
		format.layout = BufferFormat::Layout(BufferFormat::X32 + type.numRows - 1);
		break;
	case VectorInfo::UINT:
		format.type = BufferFormat::UINT;
		format.layout = BufferFormat::Layout(BufferFormat::X32 + type.numRows - 1);
		break;
	case VectorInfo::FLOAT:
		format.type = BufferFormat::FLOAT;
		format.layout = BufferFormat::Layout(BufferFormat::X32 + type.numRows - 1);
		break;
	default:
		;
	}		
	return format;
}

float4 getMaxValues(int4 numBits, bool isSigned)
{
	// determine max value for each channel
	int4 ones = splat4(1);
	if (isSigned)
		--numBits;
	numBits = max(numBits, 1);
	return convert_float4((ones << numBits) - ones);
}


// ConverterContext

ConverterContext::ConverterContext()
	: index(0)
{
/*
	this->context = new llvm::LLVMContext();
	
	// create a module
	this->module = new llvm::Module("converter", *this->context);
	
	// create JIT compiler. takes ownership of the module
	this->executionEngine = llvm::ExecutionEngine::create(this->module, false);

	this->passManager = new llvm::FunctionPassManager(this->module);

	this->passManager->add(new llvm::TargetData(*executionEngine->getTargetData()));
	this->passManager->add(llvm::createPromoteMemoryToRegisterPass());
*/

	// create llvm context
	this->context = new llvm::LLVMContext();

	// create a dummy module
	this->module = new llvm::Module("dummy", *this->context);

	// create JIT compiler. takes ownership of the module
	this->executionEngine = llvm::ExecutionEngine::create(this->module, false, NULL,
		//llvm::CodeGenOpt::None);
		llvm::CodeGenOpt::Default);
}

ConverterContext::~ConverterContext()
{
	//delete this->passManager;
	delete this->executionEngine;
	delete this->context;
}


// helpers
/*
namespace
{
	const llvm::Type* getType(BufferFormat format, llvm::IRBuilder<>& builder)
	{
		switch (format.getComponentSize())
		{
			case 1:
				return builder.getInt8Ty();
			case 2:
				return builder.getInt16Ty();
			case 4:
				return builder.getInt32Ty();
			case 8:
				return builder.getInt64Ty();
		}
		return NULL;
	}

	inline llvm::Value* getInt32Constant(llvm::IRBuilder<>& builder, int x)
	{
		return llvm::ConstantInt::get(builder.getInt32Ty(), x);
	}

	inline llvm::Value* getIntConstant(const llvm::Type* type, int x)
	{
		return llvm::ConstantInt::get(type, x);
	}

	inline llvm::Value* getFloatConstant(llvm::IRBuilder<>& builder, float x)
	{
		return llvm::ConstantFP::get(builder.getFloatTy(), x);
	}


	llvm::Value* convertChannelValueToFloat(llvm::Value* value, const llvm::Type* elementType,
		BufferFormat::Type formatType, int numBits, llvm::IRBuilder<>& builder)
	{
		bool isFloat = formatType == BufferFormat::FLOAT || formatType == BufferFormat::UFLOAT;
		
		if (!isFloat)
		{
			// integer
			bool isSigned = formatType == BufferFormat::NORM || formatType == BufferFormat::INT;
			bool isNormalized = formatType == BufferFormat::NORM || formatType == BufferFormat::UNORM;
			
			// convert to float
			if (isSigned)
				value = builder.CreateSIToFP(value, builder.getFloatTy(), "toFloat");
			else
				value = builder.CreateUIToFP(value, builder.getFloatTy(), "toFloat");

			if (isNormalized)
			{
				// normalize
				// signed: for e.g. 8 bit -127..127 map to -1..1
				// unsigned: for e.g. 8 bit 0..255 map to 0..1			
				float scale = 1.0f / (pow(2.0f, float(isSigned ? numBits - 1 : numBits)) - 1.0f);
				value = builder.CreateFMul(value, getFloatConstant(builder, scale));
			}
		}
		else
		{
			//! use llvm convert float16 functions
			
			// float
			if (numBits >= 8 && numBits <= 31)
			{
				bool isSigned = formatType == BufferFormat::FLOAT;
				const llvm::Type* i32Type = builder.getInt32Ty();
				
				if (isSigned)
				{
					// n bit signed float: convert 32 bit to n bit
					// 32 bit: s1 e8 m23 bias=127
					// 16 bit: s1 e5 m10 bias=15
					// n bit: s1 e5 m(n-6) bias=15
					
					// convert to i32
					llvm::Value* sign = builder.CreateZExtOrBitCast(value, i32Type);
					value = builder.CreateZExtOrBitCast(value, i32Type);

					// extract components
					sign = builder.CreateAnd(sign, getIntConstant(i32Type, 1 << (numBits - 1))); // 0x8000 for 16 bit
					value = builder.CreateAnd(value, getIntConstant(i32Type, ~(-1 << (numBits - 1)))); // 0x7c00 | 0x03ff for 16 bit
				
					// adjust exponent bias if exponent and mantissa are not zero
					llvm::Value* zero = getIntConstant(i32Type, 0);
					llvm::Value* checkZero = builder.CreateICmpEQ(value, zero);
					llvm::Value* adjust = builder.CreateSelect(checkZero, zero, getIntConstant(i32Type, (127 - 15) << (numBits - 6))); // 10 for 16 bit
					value = builder.CreateAdd(value, adjust);

					// shift to places in 32 bit float
					sign = builder.CreateShl(sign, getIntConstant(i32Type, 32 - numBits)); // 16 for 16 bit
					value = builder.CreateShl(value, getIntConstant(i32Type, 32 - numBits - 3)); // 13 for 16 bit
					
					// combine
					value = builder.CreateOr(sign, value);
				}
				else
				{
					// n bit unsigned float: convert 32 bit to n bit
					// 32 bit: s1 e8 m23 bias=127
					// 16 bit: e5 m11 bias=15
					// n bit: e5 m(n-5) bias=15
					
					// convert to i32
					value = builder.CreateZExtOrBitCast(value, i32Type);
			
					// adjust exponent bias if exponent and mantissa are not zero
					llvm::Value* zero = getIntConstant(i32Type, 0);
					llvm::Value* checkZero = builder.CreateICmpEQ(value, zero);
					llvm::Value* adjust = builder.CreateSelect(checkZero, zero, getIntConstant(i32Type, (127 - 15) << (numBits - 5))); // 11 for 16 bit
					value = builder.CreateAdd(value, adjust);

					// shift exponent and mantissa to places in 32 bit float
					value = builder.CreateShl(value, getIntConstant(i32Type, 32 - numBits - 4)); // 12 for 16 bit
				}
			}
			
			// bit-cast to 32 bit float
			value = builder.CreateBitCast(value, builder.getFloatTy());
		}
		return value;
	}

	llvm::Value* convertFloatToChannelValue(llvm::Value* value, BufferFormat::Type formatType, int numBits,
		const llvm::Type* elementType, llvm::IRBuilder<>& builder)
	{
		bool isFloat = formatType == BufferFormat::FLOAT || formatType == BufferFormat::UFLOAT;

		if (!isFloat)
		{
			// integer
			bool isSigned = formatType == BufferFormat::NORM || formatType == BufferFormat::INT;
			bool isNormalized = formatType == BufferFormat::NORM || formatType == BufferFormat::UNORM;
		
			// calc min and max value
			llvm::Value* minValue;
			llvm::Value* maxValue;
			if (isSigned)
			{
				// signed: min and max for e.g. 8 bit are -127 and 127
				float f = pow(2.0f, float(numBits - 1)) - 1.0f;
				minValue = getFloatConstant(builder, -f);
				maxValue = getFloatConstant(builder, f);
			}
			else
			{
				// unsigned: min and max for e.g. 8 bit are 0 and 255
				float f = pow(2.0f, float(numBits)) - 1.0f;
				minValue = getFloatConstant(builder, 0.0f);
				maxValue = getFloatConstant(builder, f);
			}

			if (isNormalized)
			{
				// normalized: de-normalize, e.g. for 8 bit 1.0 maps to 127 (signed) or 255 (unsigned)
				value = builder.CreateFMul(value, maxValue);
			}

			// clamp to allowed range
			llvm::Value* minCondition = builder.CreateFCmpOLT(value, minValue);
			value = builder.CreateSelect(minCondition, minValue, value);
			llvm::Value* maxCondition = builder.CreateFCmpOLT(value, maxValue);
			value = builder.CreateSelect(maxCondition, value, maxValue);

			// convert float to integer
			if (isSigned)
				value = builder.CreateFPToSI(value, elementType);
			else
				value = builder.CreateFPToUI(value, elementType);
		}
		else
		{
			// float

			const llvm::Type* i32Type = builder.getInt32Ty();

			// bit-cast to i32
			value = builder.CreateBitCast(value, i32Type);

			if (numBits >= 8 && numBits <= 31)
			{
				// float
				bool isSigned = formatType == BufferFormat::FLOAT;
				
				if (isSigned)
				{		
					// n bit signed float: convert 32 bit to n bit
					// 32 bit: s1 e8 m23 bias=127
					// 16 bit: s1 e5 m10 bias=15
					// n bit: s1 e5 m(n-6) bias=15
								
					// extract components
					llvm::Value* sign = builder.CreateAnd(value, getIntConstant(i32Type, 0x80000000));
					value = builder.CreateAnd(value, getIntConstant(i32Type, 0x7f800000 | 0x007fffff));
				
					// shift to places in n bit float
					sign = builder.CreateLShr(sign, getIntConstant(i32Type, 32 - numBits)); // 16 for 16 bit
					value = builder.CreateLShr(value, getIntConstant(i32Type, 32 - numBits - 3)); // 13 for 16 bit

					// adjust exponent bias
					llvm::Value* adjust = getIntConstant(i32Type, (127 - 15) << (numBits - 6)); // 10 for 16 bit
					value = builder.CreateSub(value, adjust);

					// clamp exponent and mantissa to range 0..2^(numBits-1)-1 (0x0000..0x7fff for 16 bit)
					llvm::Value* minValue = getIntConstant(i32Type, 0);
					llvm::Value* minCondition = builder.CreateICmpSLT(value, minValue);
					value = builder.CreateSelect(minCondition, minValue, value);
					llvm::Value* maxValue = getIntConstant(i32Type, (1 << (numBits - 1)) - 1);
					llvm::Value* maxCondition = builder.CreateICmpSLT(value, maxValue);
					value = builder.CreateSelect(maxCondition, value, maxValue);
					
					// combine
					value = builder.CreateOr(sign, value);
					
					// convert to elementType
					if (elementType->getScalarSizeInBits() < 32)
						value = builder.CreateTrunc(value, elementType);
					else
						value = builder.CreateZExtOrBitCast(value, elementType);
				}
				else
				{
					// n bit unsigned float: convert 32 bit to n bit
					// 32 bit: s1 e8 m23 bias=127
					// 16 bit: e5 m11 bias=15
					// n bit: e5 m(n-5) bias=15
					
					// shift exponent and mantissa to places in n bit float
					value = builder.CreateAShr(value, getIntConstant(i32Type, 32 - numBits - 4)); // 12 for 16 bit

					// adjust exponent bias
					llvm::Value* adjust = getIntConstant(i32Type, (127 - 15) << (numBits - 5)); // 11 for 16 bit
					value = builder.CreateSub(value, adjust);

					// clamp exponent and mantissa to range 0..2^numBits-1 (0x0000..0xffff for 16 bit)
					llvm::Value* minValue = getIntConstant(i32Type, 0);
					llvm::Value* minCondition = builder.CreateICmpSLT(value, minValue);
					value = builder.CreateSelect(minCondition, minValue, value);
					llvm::Value* maxValue = getIntConstant(i32Type, (1 << numBits) - 1);
					llvm::Value* maxCondition = builder.CreateICmpSLT(value, maxValue);
					value = builder.CreateSelect(maxCondition, value, maxValue);
								
					// convert to elementType
					if (elementType->getScalarSizeInBits() < 32)
						value = builder.CreateTrunc(value, elementType);
					else
						value = builder.CreateZExtOrBitCast(value, elementType);
				}
			}
		}
		return value;
	}
	
} // anonymous namespace
*/

// ConverterBuilder
/*
ConverterBuilder::ConverterBuilder(Pointer<ConverterContext> context)
	: context(context)
{
	llvm::LLVMContext& c = *context->context;

	// get types
	const llvm::Type* voidType = llvm::TypeBuilder<void, false>::get(c);
	const llvm::Type* pointerType = llvm::TypeBuilder<void*, false>::get(c);
	const llvm::Type* sizeType = llvm::TypeBuilder<size_t, false>::get(c);

	// create function that takes a pointer to destination and source data and number of elements
	// e.g. void f(void* srcData, void* dstData, size_t dstStride, size_t numElements);
	this->function = llvm::cast<llvm::Function>(context->module->getOrInsertFunction(toString(context->index++),
		voidType, pointerType, pointerType, sizeType, sizeType, (llvm::Type*)NULL));

	// create basic blocks
	this->entry = llvm::BasicBlock::Create(c, "Entry", this->function);
	this->loop = llvm::BasicBlock::Create(c, "Loop", this->function);
	this->exit = llvm::BasicBlock::Create(c, "Exit", this->function);
}

void ConverterBuilder::load(BufferFormat format)
{
	// get pointers to the arguments of the function
	llvm::Function::arg_iterator it = this->function->arg_begin();
	llvm::Value* srcDataArg = it;


	// build instructions in entry block
	llvm::IRBuilder<> builder(this->entry);

	// get source types
	const llvm::Type* srcElementType = getType(format, builder);
	llvm::Type* srcPointerType = llvm::PointerType::get(srcElementType, 0);

	// create stack variable and store data pointer
	llvm::Value* srcDataVar = builder.CreateAlloca(srcPointerType);
	builder.CreateStore(builder.CreateBitCast(srcDataArg, srcPointerType), srcDataVar);
	

	// build instructions in loop block
	builder.SetInsertPoint(this->loop);

	// load data pointer
	llvm::Value* srcData = builder.CreateLoad(srcDataVar);
	
	// load element
	int numComponents = format.getNumComponents();
	for (int i = 0; i < numComponents; ++i)
	{
		// get component/element pointer
		llvm::Value* elementPointer = builder.CreateConstGEP1_32(srcData, i);
		
		// load component/element
		this->values[i] = builder.CreateLoad(elementPointer, "load");
	}

	// increment and store data pointer
	srcData = builder.CreateConstGEP1_32(srcData, format.getNumComponents());
	builder.CreateStore(srcData, srcDataVar);
}

void ConverterBuilder::depackToFloat(BufferFormat format, float4 defaultValues)
{
	llvm::IRBuilder<> builder(this->loop);

	const llvm::Type* elementType = getType(format, builder);

	const BufferFormat::LayoutInfo& layoutInfo = format.getLayoutInfo();
	if (layoutInfo.type == BufferFormat::STANDARD)
	{
		// standard format. each channel is stored in a basic type element (e.g. XYZW8)
					
		// iterate over the 4 channels
		for (int channelIndex = 0; channelIndex < 4; ++channelIndex)
		{				
			llvm::Value* value;

			// check if source contains the channel
			if (channelIndex < layoutInfo.numComponents)
			{
				// get channel value
				value = this->values[channelIndex];
				
				// convert to float
				value = convertChannelValueToFloat(value, elementType, format.type, layoutInfo.componentSize * 8, builder);
			}
			else
			{
				// source does not provide the channel: load default value
				value = getFloatConstant(builder, defaultValues[channelIndex]);
			}
			this->values[channelIndex] = value;
		}
	}
	else if (layoutInfo.type == BufferFormat::COMBINED)
	{
		// combined format. all channels are in one component (e.g. X10Y10Z10W2)
		bool isSignedInt = format.type == BufferFormat::NORM || format.type == BufferFormat::INT;

		// get element
		llvm::Value* element = this->values[0];

		// iterate over the 4 channels
		int bitPos = 0;
		for (int channelIndex = 0; channelIndex < 4; ++channelIndex)
		{				
			llvm::Value* value = element;
			
			int numBits = layoutInfo.numBits[channelIndex];
			
			// check if source contains the channel
			if (numBits != 0)
			{
				// shift to lsb (logical, unsigned)
				if (bitPos > 0)
					//value = builder.CreateLShr(value, getInt32Constant(builder, bitPos));
					value = builder.CreateLShr(value, getIntConstant(elementType, bitPos));

				// signed integer: add channel-msb to make unsigned
				if (isSignedInt)
					value = builder.CreateAdd(value, getIntConstant(elementType, 1 << (numBits - 1)));

				// mask bits of channel, e.g. 0x000000ff for 8 bit
				int mask = ~(0xffffffff << numBits);								
				value = builder.CreateAnd(value, getIntConstant(elementType, mask));

				// signed: sub channel-msb to make signed
				if (isSignedInt)
					value = builder.CreateSub(value, getIntConstant(elementType, 1 << (numBits - 1)));
										
				// convert to float
				value = convertChannelValueToFloat(value, elementType, format.type, numBits, builder);
			}
			else
			{
				// source does not provide the channel: load default value
				value = getFloatConstant(builder, defaultValues[channelIndex]);
			}
			this->values[channelIndex] = value;

			bitPos += numBits;
		}			
	}
}

void ConverterBuilder::setValue(int channelIndex, float value)
{
	llvm::IRBuilder<> builder(this->loop);
	this->values[channelIndex] = getFloatConstant(builder, value);
}

void ConverterBuilder::permute(int swizzle)
{
	llvm::Value* values[4];
	for (int i = 0; i < 4; ++i)
	{
		values[i] = this->values[((swizzle >> i * 4) & 0x03)];
	}
	std::copy(values, values + 4, this->values);
}

namespace
{
	// value * factor
	llvm::Value* createMul(llvm::IRBuilder<>& builder, llvm::Value* value, float factor)
	{
		llvm::Value* c = getFloatConstant(builder, factor);
		if (factor == 0.0f)
			return c;
		return builder.CreateFMul(value, c);
	}

	// value1 + value2 * factor
	llvm::Value* createMad(llvm::IRBuilder<>& builder, llvm::Value* value1, llvm::Value* value2, float factor)
	{
		if (factor == 0.0f)
			return value1;
		return builder.CreateFAdd(value1, builder.CreateFMul(value2, getFloatConstant(builder, factor)));
	}
}

void ConverterBuilder::transform(float4x4 matrix)
{
	llvm::IRBuilder<> builder(this->loop);

	llvm::Value* values[4];
	for (int j = 0; j < 4; ++j)
	{
		llvm::Value* value = createMul(builder, this->values[0], matrix.x[j]);
		value = createMad(builder, value, this->values[1], matrix.y[j]);
		value = createMad(builder, value, this->values[2], matrix.z[j]);
		value = createMad(builder, value, this->values[3], matrix.w[j]);
		values[j] = value;
	}
	std::copy(values, values + 4, this->values);
}

void ConverterBuilder::process(const std::string& code)
{
		// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer.get());
	if (compileResult == null)
		return false;

	// get compiled module (take ownership)
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return false;

}

void ConverterBuilder::packFromFloat(BufferFormat format)
{
	llvm::IRBuilder<> builder(this->loop);
	const llvm::Type* elementType = getType(format, builder);

	const BufferFormat::LayoutInfo& layoutInfo = format.getLayoutInfo();	
	if (layoutInfo.type == BufferFormat::STANDARD)
	{
		// iterate over the 4 channels
		for (int channelIndex = 0; channelIndex < 4; ++channelIndex)
		{				
			// get input float value
			llvm::Value* value = this->values[channelIndex];
			
			// check if destination contains the channel
			if (channelIndex < layoutInfo.numComponents)
			{
				// output component
				this->values[channelIndex] = convertFloatToChannelValue(value, format.type, layoutInfo.componentSize * 8,
					elementType, builder);
			}
		}
	}
	else if (layoutInfo.type == BufferFormat::COMBINED)
	{
		// combined format. all channels are in one component (e.g. X10Y10Z10W2)
		llvm::Value* element = NULL;

		// iterate over the 4 color channels
		int bitPos = 0;
		for (int channelIndex = 0; channelIndex < 4; ++channelIndex)
		{				
			int numBits = layoutInfo.numBits[channelIndex];
			
			// check if destination contains the channel
			if (numBits != 0)
			{
				llvm::Value* value = convertFloatToChannelValue(this->values[channelIndex], format.type, numBits,
					elementType, builder);
				
				// mask bits of channel, e.g. 0x000000ff for 8 bit
				int mask = ~(0xffffffff << numBits);
				value = builder.CreateAnd(value, getIntConstant(elementType, mask));

				// shift from lsb to bit position
				if (bitPos > 0)
					value = builder.CreateShl(value, getIntConstant(elementType, bitPos));
									
				// combine with other values
				if (element == NULL)
					element = value;
				else
					element = builder.CreateOr(element, value);
			}
			
			bitPos += numBits;			
		}
		
		// output element
		this->values[0] = element;
	}
}

void ConverterBuilder::store(BufferFormat format)
{
	// get pointers to the arguments of the function
	llvm::Function::arg_iterator it = this->function->arg_begin();
	llvm::Value* srcDataArg = it++;
	llvm::Value* dstDataArg = it++;
	llvm::Value* dstStrideArg = it;


	// build instructions in entry block
	llvm::IRBuilder<> builder(this->entry);

	// get destination types
	const llvm::Type* dstElementType = getType(format, builder);
	llvm::Type* dstPointerType = llvm::PointerType::get(dstElementType, 0);

	// create stack variable and store data pointer
	llvm::Value* dstDataVar = builder.CreateAlloca(dstPointerType);
	builder.CreateStore(builder.CreateBitCast(dstDataArg, dstPointerType), dstDataVar);


	// build instructions in loop block
	builder.SetInsertPoint(this->loop);

	// load data pointer
	llvm::Value* dstData = builder.CreateLoad(dstDataVar);

	// store element
	int numComponents = format.getNumComponents();
	for (int i = 0; i < numComponents; ++i)
	{
		// get component/element pointer
		llvm::Value* elementPointer = builder.CreateConstGEP1_32(dstData, i);
				
		// store component
		builder.CreateStore(this->values[i], elementPointer);	
	}

	// increment and store data pointer
	dstData = builder.CreateGEP(dstData, builder.CreateMul(dstStrideArg, llvm::ConstantInt::get(dstStrideArg->getType(), format.getNumComponents())));
	builder.CreateStore(dstData, dstDataVar);
}

void ConverterBuilder::storeToBuffer(BufferFormat format, bool bigEndian)
{
	// get pointers to the arguments of the function
	llvm::Function::arg_iterator it = this->function->arg_begin();
	llvm::Value* srcDataArg = it++;
	llvm::Value* dstDataArg = it++;
	llvm::Value* dstStrideArg = it;


	// build instructions in entry block
	llvm::IRBuilder<> builder(this->entry);

	// get destination types
	const llvm::Type* i8Type = builder.getInt8Ty();
	const llvm::Type* dstPointerType = builder.getInt8PtrTy();

	// create stack variable and store data pointer
	llvm::Value* dstDataVar = builder.CreateAlloca(dstPointerType);
	builder.CreateStore(dstDataArg, dstDataVar);
	

	// build instructions in loop block
	builder.SetInsertPoint(this->loop);

	// load data pointer
	llvm::Value* dstData = builder.CreateLoad(dstDataVar);

	// store element
	int numComponents = format.getNumComponents();
	int componentSize = format.getComponentSize();
	for (int componentIndex = 0; componentIndex < numComponents; ++componentIndex)
	{	
		// store as bytes
		for (int i = 0; i < componentSize; ++i)
		{
			llvm::Value* value = this->values[componentIndex];

			// extract byte
			value = builder.CreateLShr(value, (bigEndian ? (componentSize - 1 - i) : i) * 8);
			
			// cast to i8
			value = builder.CreateTruncOrBitCast(value, i8Type);
			
			// get byte pointer
			llvm::Value* elementPointer = builder.CreateConstGEP1_32(dstData, componentIndex * componentSize + i);

			// store byte
			builder.CreateStore(value, elementPointer);	
		}
	}
	
	// increment and store data pointer
	dstData = builder.CreateGEP(dstData, dstStrideArg);
	builder.CreateStore(dstData, dstDataVar);
}

Pointer<DataConverter> ConverterBuilder::get()
{
	llvm::LLVMContext& c = *this->context->context;

	// finish all basic blocks
	{
		const llvm::Type* sizeType = llvm::TypeBuilder<size_t, false>::get(c);

		// get pointers to the arguments of the function
		llvm::Function::arg_iterator it = this->function->arg_begin();
		llvm::Value* srcDataArg = it++;
		llvm::Value* dstDataArg = it++;
		llvm::Value* dstStrideArg = it++;
		llvm::Value* numElementsArg = it;
		
		// set names of arguments
		srcDataArg->setName("srcData");  	
		dstDataArg->setName("dstData");  	
		dstStrideArg->setName("dstStride");  	
		numElementsArg->setName("numElements");  	
		
		// build instructions in entry block
		llvm::IRBuilder<> builder(this->entry);
		
		// create number of elements on stack
		llvm::Value* numElementsVar = builder.CreateAlloca(sizeType);

		// store number of elements
		builder.CreateStore(numElementsArg, numElementsVar);

		// branch to basic block "Loop"
		builder.CreateBr(loop);
		

		// build instructions in loop block
		builder.SetInsertPoint(this->loop);

		// load number of elements
		llvm::Value* numElements = builder.CreateLoad(numElementsVar);
		
		// decrement number of elements
		llvm::Value* decrement = getIntConstant(sizeType, 1);
		numElements = builder.CreateSub(numElements, decrement);
		
		// store number of elements
		builder.CreateStore(numElements, numElementsVar);
		
		// check if zero
		llvm::Value* zero = getIntConstant(sizeType, 0);
		llvm::Value* checkZero = builder.CreateICmpEQ(numElements, zero);
		
		// jump back if not zero
		builder.CreateCondBr(checkZero, exit, loop);

		
		// build instructions in exit block
		builder.SetInsertPoint(exit);

		// add return instruction
		builder.CreateRetVoid();
	}

	#ifndef NDEBUG
		// verify function	
		if (verifyFunction(*this->function))
			return null;
	#endif

	// optimize function
	this->context->passManager->run(*this->function);
	
	// create and return converter
	return new DataConverter(this->context, this->function);
}
*/


// DataConverter

namespace
{
	intptr_t getPointerToFunction(llvm::ExecutionEngine* executionEngine, llvm::Module* module, StringRef name)
	{
		llvm::Function* function = module->getFunction(llvm::StringRef(name.data(), name.length()));
		if (function == NULL)
			return 0;
		return (intptr_t)executionEngine->getPointerToFunction(function);
	}
}

Pointer<DataConverter> DataConverter::create(Pointer<ConverterContext> context, const std::string& code)
{
	Compiler compiler(Compiler::VM_OPENGL); //! opengl not necessary

	// code generator options
	clang::CodeGenOptions codeGenOptions;
	codeGenOptions.DisableLLVMOpts = 1;
	codeGenOptions.OptimizationLevel = 0;
	//codeGenOptions.OptimizationLevel = 2;
	codeGenOptions.Inlining = clang::CodeGenOptions::NormalInlining;
		
	// compile
	llvm::OwningPtr<clang::CodeGenerator> astConsumer(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		"module", // module name
		codeGenOptions,
		*context->context));

	Pointer<CompileResult> result = VMFile::compile(compiler, code, astConsumer.get());
	if (result == null)
		return null;
	
	// get module (take ownership)
	llvm::Module* module = astConsumer->ReleaseModule();
	if (module == NULL)
		return null;

	// add module to execution engine
	context->executionEngine->addModule(module);
				
	// get function pointer
	Convert convert = (Convert)getPointerToFunction(
		context->executionEngine, module, "_convert");
	if (convert == NULL)
		return null;

	return new DataConverter(context, convert);
}

DataConverter::~DataConverter()
{	
}


// ConverterWriter

namespace
{
	void depack(CodeWriter& w, const std::string& input, BufferFormat format, int numOutputChannels)
	{
		int numChannels = format.getNumChannels();
		int4 numBits = format.getNumBits();

		if (format.getLayoutType() == BufferFormat::STANDARD)
		{
			// standard format: load source data
			w << input;
		}
		else
		{
			// combined format
			int4 bitPositions = format.getBitPositions();
		
			if (format.isSigned())
				w << "((";

			// shift to lsb and mask
			w << "((" << input << " >> " << varVector(bitPositions, numChannels) << ") & "
				<< varVector((1 << numBits) - 1, numChannels, true) << ")";

			if (format.isSigned())
			{
				// signed: flip sign bit and subtract half range
				w << " ^ " << varVector(1 << numBits - 1, numChannels, true) << ") - "
					<< varVector((1 << numBits - 1), numChannels, true) << ')';
			}
		}

		// truncate channels if necessary
		if (numChannels > numOutputChannels)
			w << '.' << substring("xyzw", 0, numOutputChannels);
	}
} // anonymous namespace

void ConverterWriter::depackToInt(CodeWriter& w, const std::string& input, BufferFormat format,
	int numOutputChannels, int4 defaultValues)
{
	int numChannels = format.getNumChannels();
	bool isInt = format.isInt32();

	// expand to numOutputChannels channels if necessary
	if (numChannels < numOutputChannels)
		w << "vector" << ident(numOutputChannels) << "(";
	
	// convert_int if necessary
	if (!isInt)
		w << "convert_" << VectorInfo(VectorInfo::INT, numChannels).toString() << '(';
		
	depack(w, input, format, numOutputChannels);
		
	// close convert_int
	if (!isInt)
		w << ')';

	// fill channels that are not supplied by source data with default values
	if (numChannels < numOutputChannels)
	{
		for (int i = numChannels; i < numOutputChannels; ++i)
		{
			w << ", " << defaultValues[i];
		}
		w << ')';
	}
}

void ConverterWriter::depackToFloat(CodeWriter& w, const std::string& input, BufferFormat format,
	int numOutputChannels, float4 defaultValues)
{
	int numChannels = format.getNumChannels();

	// expand to numOutputChannels channels if necessary
	if (numChannels < numOutputChannels)
		w << "vector" << ident(numOutputChannels) << "(";
	
	if (format.isFloat32())
	{
		// float
		w << input;
	}
	else if (format.isFloat16())
	{
		// half
		//! very simple, does not handle 0.0, denorm and inf correctly
		w << "as_" << VectorInfo(VectorInfo::FLOAT, numChannels).toString();
		std::string i = Code () << "convert_" << VectorInfo(VectorInfo::INT, numChannels).toString() << '(' << input << ')';
		w << "((" << i << " & 0x8000) << 16 | (" << i << " & 0x7fff) + 0x1c000 << 13)";
	}
	else
	{
		w << "convert_" << VectorInfo(VectorInfo::FLOAT, numChannels).toString() << '(';
		depack(w, input, format, numOutputChannels);
		w << ')';

		// scale normalized formats
		if (format.isNormalized())
		{
			int4 numBits = format.getNumBits();

			// divide by maximum integer value
			w << " * " << varVector(1.0f / getMaxValues(numBits, format.isSigned()), min(numChannels, numOutputChannels), true);
		}
	}

	// fill channels that are not supplied by input with default values
	if (numChannels < numOutputChannels)
	{
		for (int i = numChannels; i < numOutputChannels; ++i)
		{
			w << ", " << defaultValues[i];
		}
		w << ')';
	}
}

void ConverterWriter::beginConverter()
{
	w << "extern \"C\" void _convert(ubyte* srcData, size_t srcStride, void* global, "
		"ubyte* dstData, size_t dstStride, size_t numElements)\n";
	w.beginScope();
		
	w << "while (numElements > 0)\n";
	w.beginScope();
}

void ConverterWriter::endConverter()
{
	w << "--numElements;\n";
	w.endScope();
	w.endScope();
}

void ConverterWriter::load(BufferFormat format)
{
	VectorInfo type = toVectorInfo(format);
	std::string typeString = type.toString();
	
	this->current = this->nameGenerator.get();
	w << typeString << " " << this->current << " = *(" << (type.numRows >= 2 ? "packed_" : "") << typeString << "*)srcData;\n";

	w << "srcData += srcStride;\n";
}

void ConverterWriter::depackToInt(BufferFormat format, int4 defaultValues)
{
	std::string var = this->nameGenerator.get();

	w << "int4 " << var << " = ";
	ConverterWriter::depackToInt(w, this->current, format, 4, defaultValues);
	w << ";\n";
	
	swap(this->current, var);
}

void ConverterWriter::depackToFloat(BufferFormat format, float4 defaultValues)
{
	std::string var = this->nameGenerator.get();

	w << "float4 " << var << " = ";
	ConverterWriter::depackToFloat(w, this->current, format, 4, defaultValues);
	w << ";\n";
	
	swap(this->current, var);
}

void ConverterWriter::setValue(int channelIndex, float value)
{
	w << this->current << '.' << "xyzw"[channelIndex] << " = " << value << ";\n";
}

void ConverterWriter::permute(int swizzle)
{
	if ((swizzle & 0x3333) != 0x3210)
	{
		std::string var = this->nameGenerator.get();

		w << "float4 " << var << " = " << this->current << '.';
		for (int i = 0; i < 4; ++i)
		{
			w << "xyzw"[(swizzle >> i * 4) & 0x03];
		} 
		w << ";\n";

		swap(this->current, var);
	}
}

void ConverterWriter::transform(float4x4 matrix)
{
	std::string var = this->nameGenerator.get();
	
	w << "float4 " << var << " = " << matrix << " * " << this->current << ";\n";
	
	swap(this->current, var);
}

void ConverterWriter::callFunc()
{
	std::string var = this->nameGenerator.get();
	
	w << "float4 " << var << " = main(" << this->current << ", *(Global*)global);\n";
	
	swap(this->current, var);	
}

void ConverterWriter::callProc()
{
	w << "main(" << this->current << ", *(Global*)global);\n";
}

void ConverterWriter::packFromFloat(BufferFormat format)
{
	std::string var = this->nameGenerator.get();

	VectorInfo type = toVectorInfo(format);
	std::string typeString = type.toString();

	int numChannels = format.getNumChannels();
	int4 numBits = format.getNumBits();
	bool isFloat = format.isFloat32();
	float4 maxValues = getMaxValues(numBits, format.isSigned());
			
	if (format.getLayoutType() == BufferFormat::STANDARD)
	{
		// standard format (each channel in own machine component)
		w << typeString << ' ' << var << " = ";

		// convert to destination type if necessary
		if (!isFloat)
			w << "convert_" << toVectorInfo(format).toString() << "_sat_rte(";

		w << this->current;

		// truncate channels if necessary
		if (numChannels < 4)
			w << '.' << substring("xyzw", 0, numChannels);

		// scale normalized formats
		if (format.isNormalized())
		{
			// multiply by maximum integer value
			w << " * " << maxValues.x;
		}

		// close convert
		if (!isFloat)
			w << ')';
	}
	else
	{
		// combined format
		int4 bitPositions = format.getBitPositions();
		int numChannels = format.getNumChannels();
		
		std::string type = VectorInfo(format.isSigned() ? VectorInfo::INT : VectorInfo::UINT, numChannels).toString();
		w << type << ' ' << var << " = ";
		if (format.isSigned())
			w << '(';
		w << "convert_" << type << "_rte(";
		w << "clamp(" << this->current;

		// truncate channels if necessary
		if (numChannels < 4)
			w << '.' << substring("xyzw", 0, numChannels);

		// scale normalized formats
		if (format.isNormalized())
		{
			// multiply by maximum integer value
			w << " * " << varVector(maxValues, numChannels, true);
		}
		
		// min/max value for clamp
		float4 minValues = format.isSigned() ? -maxValues - 1.0f : float4();
		w << ", " << varVector(minValues, numChannels, true) << ", " << varVector(maxValues, numChannels, true) << "))";
	
		// mask out sign bits
		if (format.isSigned())
		{
			w << " & " << varVector((1 << numBits) - 1, numChannels, true) << ')';

			// shift to position
			w << " << " << varVector(bitPositions, numChannels) << ";\n";
		}
		else
		{	
			// shift to position
			w << " << " << varVector(convert_uint4(bitPositions), numChannels) << ";\n";
		}
		
		std::string var2 = this->nameGenerator.get();

		// combine channels
		switch (format.getComponentSize())
		{
		case 1:
			w << "ubyte";
			break;
		case 2:
			w << "ushort";
			break;
		case 4:
			w << "uint";
			break;
		}
		w << ' ' << var2 << " = ";
		for (int i = 0; i < numChannels; ++i)
		{
			if (i != 0)
				w << " | ";
			w << var << '.' << "xyzw"[i];
		}
		
		swap(var2, var);
	}

	w << ";\n";

	swap(this->current, var);
}

void ConverterWriter::store(BufferFormat format, DataConverter::Mode mode)
{
	if (mode == DataConverter::NATIVE)
	{
		VectorInfo type = toVectorInfo(format);
		std::string typeString = type.toString();
		
		w << "*(" << (type.numRows >= 2 ? "packed_" : "") << typeString << "*)dstData = " << this->current << ";\n";
	}
	else
	{
		// store into byte buffer
		int numComponents = format.getNumComponents();
		int componentSize = format.getComponentSize();
		
		bool isFloat = format.isFloat32();
		
		int dstIndex = 0;
		for (int c = 0; c < numComponents; ++c)
		{
			// get one component as unsigned integer
			std::string temp = this->nameGenerator.get();
			switch (componentSize)
			{
			case 1:
				w << "ubyte ";
				break;
			case 2:
				w << "ushort ";
				break;
			case 4:
				w << "uint ";
				break;
			}
			w << temp << " = ";
			if (isFloat)
				w << "as_int(";
			w << this->current;
			if (numComponents > 1)
				w << '.' << "xyzw"[c];		
			if (isFloat)
				w << ')';
			w << ";\n";
			
			// write component to buffer byte wise
			for (int i = 0; i < componentSize; ++i)
			{
				w << "dstData[" << dstIndex++ << "] = " << temp;
				int shift = (mode == DataConverter::BE ? componentSize - 1 - i : i) * 8;
				if (shift != 0)
					w << " >> " << shift;
				w << ";\n";
			}
		}
	}
	w << "dstData += dstStride;\n";
}

} // namespace digi

#ifndef digi_ImageConvert_Converter_h
#define digi_ImageConvert_Converter_h

#include <limits>

#include <digi/Utility/Object.h>
#include <digi/Image/BufferFormat.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/NameGenerator.h>
#include <digi/CodeGenerator/TypeInfo.h>


// forward-declare llvm classes
namespace llvm {

class LLVMContext;
class Module;
class ExecutionEngine;
class FunctionPassManager;
class Function;
class BasicBlock;
class Value;

}


namespace digi {

/// @addtogroup ImageConvert
/// @{

// convert buffer format to vector info
VectorInfo toVectorInfo(BufferFormat format);

// convert vector info to buffer format
BufferFormat toBufferFormat(VectorInfo type);

// get maximum values for given number of bits
float4 getMaxValues(int4 numBits, bool isSigned);


class ConverterContext : public Object
{
	friend class ConverterBuilder;
	friend class DataConverter;
public:
	
	ConverterContext();
	virtual ~ConverterContext();

protected:

	// context
	llvm::LLVMContext* context;

	// module
	llvm::Module* module;
		
	// JIT compiler
	llvm::ExecutionEngine* executionEngine;

	// function pass manager for optimizations
	llvm::FunctionPassManager* passManager;
	
	// index for function name generation
	int index;
};	


class DataConverter : public Object
{
public:

	/// load/store mode
	enum Mode
	{
		/// native byte order
		NATIVE,
		
		/// little endian
		LE,
		
		/// big endian
		BE
	};

	// stride is counted in elements for typed data and in bytes for data buffers
	typedef void (*Convert)(void* srcData, size_t srcStride, void* global,
		void* dstData, size_t dstStride, size_t numElements);

	static Pointer<DataConverter> create(Pointer<ConverterContext> context, const std::string& code);

	DataConverter(Pointer<ConverterContext> context, Convert convert)
		: convert(convert), context(context) {}
	
	virtual ~DataConverter();


	// pointer to compiled convert function
	Convert convert;

protected:

	// pointer to context to keep it alive
	Pointer<ConverterContext> context;
};	


class ConverterWriter
{
public:
	ConverterWriter(CodeWriter& w)
		: w(w) {}

	static void depackToInt(CodeWriter& w, const std::string& input, BufferFormat format,
		int numOutputChannels, int4 defaultValues);

	static void depackToFloat(CodeWriter& w, const std::string& input, BufferFormat format,
		int numOutputChannels, float4 defaultValues);

	void beginConverter();
	void endConverter();
	void load(BufferFormat format);
	void depackToFloat(BufferFormat format, float4 defaultValues = make_float4(0, 0, 0, 1));
	void depackToInt(BufferFormat format, int4 defaultValues = int4());
	void setValue(int channelIndex, float value);
	void permute(int swizzle);
	void transform(float4x4 matrix);
	void callFunc();
	void callProc();
	void packFromFloat(BufferFormat format);
	void store(BufferFormat format, DataConverter::Mode mode);

protected:
	CodeWriter& w;
	NameGenerator nameGenerator;
	std::string current;
};

/// @}

} // namespace Digi

#endif

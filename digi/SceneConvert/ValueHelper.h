#include <boost/optional.hpp>

#include <digi/Math/All.h>
#include <digi/CodeGenerator/TypeInfo.h>

#include <llvm/Instructions.h>
#include <llvm/Constants.h>


namespace digi {

// returns true if we treat the type as a bool value
bool isBool(llvm::Type* type);

// get number of vector elements (1 for scalar)
int getNumElements(llvm::Type* type);

// get number of vector elements (1 for scalar)
inline int getNumElements(llvm::Value* value) {return getNumElements(value->getType());};

// get number of uses of value, result clamped to maximum of 2
int getNumUses(const llvm::Value* value);

// get constant index, returns -1 if not constant or undef
int getConstantIndex(llvm::Value* value);

// get constant index vector, returns -1 if not constant or undef
int4 getConstantIndexVector(llvm::Value* value);

// get constant scalar value or vector splat value. returns null if undef or other value
boost::optional<double> getConstantDouble(llvm::Value* value);

// compares two values. returns true if pointers or values are equal
//bool equals(llvm::Value* a, llvm::Value* b);

// create a uint32 constant
inline llvm::Value* createUInt32(llvm::LLVMContext& c, int value)
{
	return llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), value);
}

// crate a float scalar or vector of given floating point type
llvm::Value* createFloatVector(llvm::Type* type, float4 value);

// get VectorInfo for llvm::Type (only BOOL, INT and FLOAT)
VectorInfo getVectorInfo(llvm::Type* type);


enum PrintType
{
	// is a scalar
	PRINT_SCALAR,
	
	// vector that is printed as scalar (e.g. a.xxxx -> a.x)
	PRINT_VECTOR_SCALAR,

	// is a vector
	PRINT_VECTOR
};

// return the type as which the value is printed
PrintType getPrintType(llvm::Value* value);


// for value tracking of vector insert/extract/shuffle
struct TrackValue
{
	llvm::Value* value;
	int index;
	
	TrackValue()
		: value(), index() {}
	TrackValue(llvm::Value* value, int index)
		: value(value), index(index) {}
};


void trackValues(TrackValue* values, int numElements, llvm::Value* value);

void trackValuesInsertElement(TrackValue* values, int numElements, llvm::Instruction* instruction);

void trackValuesExtractElement(TrackValue* values, int numElements, llvm::Instruction* instruction);

void trackValuesShuffleVector(TrackValue* values, int numElements, llvm::Instruction* instruction);
	

} // namespace digi

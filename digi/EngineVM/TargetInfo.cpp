#include "TargetInfo.h"


namespace digi {


// TargetInfo

TargetInfo::TargetInfo()
	: clang::TargetInfo(std::string()) // triple
{
	TLSSupported = false;
	NoAsmVariants = false;
	PointerWidth = PointerAlign = sizeof(void*) * 8;
	IntWidth = IntAlign = 32;
	LongWidth = LongAlign = 64;
	LongLongWidth = LongLongAlign = 64;
	FloatWidth = 32;
	FloatAlign = 32;
	DoubleWidth = 64;
	DoubleAlign = 64;
	LongDoubleWidth = 64;
	LongDoubleAlign = 64;

	if (PointerWidth == 32)
	{
		SizeType = UnsignedInt;
		PtrDiffType = SignedInt;
		IntPtrType = SignedInt;
		DescriptionString = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32";
	}
	else
	{
		SizeType = UnsignedLong;
		PtrDiffType = SignedLong;
		IntPtrType = SignedLong;	
		DescriptionString = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32";
	}
}
		
TargetInfo::~TargetInfo()
{
}

void TargetInfo::getTargetDefines(const clang::LangOptions &Opts,
	clang::MacroBuilder &Builder) const
{
	
}

void TargetInfo::getTargetBuiltins(const clang::Builtin::Info *&Records,
	unsigned &NumRecords) const
{
	
}

const char *TargetInfo::getVAListDeclaration() const
{
	return "";
}

const char *TargetInfo::getClobbers() const
{
	return "";
}

void TargetInfo::getGCCRegNames(const char * const *&Names,
	unsigned &NumNames) const
{
	
}
		
void TargetInfo::getGCCRegAliases(const GCCRegAlias *&Aliases,
	unsigned &NumAliases) const
{
	
}
		
bool TargetInfo::validateAsmConstraint(const char *&Name,
	TargetInfo::ConstraintInfo &info) const
{
	return false;
}


} // namespace digi

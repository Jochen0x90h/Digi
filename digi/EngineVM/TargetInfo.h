#ifndef digi_EngineVM_TargetInfo_h
#define digi_EngineVM_TargetInfo_h

#include <string>

#include <clang/Basic/TargetInfo.h>


namespace digi {

/// @addtogroup EngineVM
/// @{


class TargetInfo : public clang::TargetInfo
{
	public:

		TargetInfo();
		virtual ~TargetInfo();
	
		/// getTargetDefines - Appends the target-specific #define values for this
		/// target set to the specified buffer.
		virtual void getTargetDefines(const clang::LangOptions &Opts,
			clang::MacroBuilder &Builder) const;


		/// getTargetBuiltins - Return information about target-specific builtins for
		/// the current primary target, and info about which builtins are non-portable
		/// across the current set of primary and secondary targets.
		virtual void getTargetBuiltins(const clang::Builtin::Info *&Records,
			unsigned &NumRecords) const;

		/// getVAListDeclaration - Return the declaration to use for
		/// __builtin_va_list, which is target-specific.
		virtual const char *getVAListDeclaration() const;



		// Returns a string of target-specific clobbers, in LLVM format.
		virtual const char *getClobbers() const;


		virtual void getGCCRegNames(const char * const *&Names,
			unsigned &NumNames) const;
		
		virtual void getGCCRegAliases(const GCCRegAlias *&Aliases,
			unsigned &NumAliases) const;
		
		virtual bool validateAsmConstraint(const char *&Name,
			TargetInfo::ConstraintInfo &info) const;

};


/// @}

} // namespace digi

#endif

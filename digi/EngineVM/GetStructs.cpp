#include <clang/AST/DeclGroup.h>
#include <clang/AST/DeclCXX.h>

#include "GetStructs.h"


namespace digi {


// GetStructs

GetStructs::GetStructs(clang::CodeGenerator* codeGenerator)
	: CodeGenWrapper(codeGenerator), globalDecl(), instanceDecl()
{
}

GetStructs::~GetStructs()
{
}

void GetStructs::handleDecl(clang::Decl* decl)
{
	// check if this is a struct declaration
	if (clang::RecordDecl* recordDecl = llvm::dyn_cast<clang::RecordDecl>(decl))
	{
		llvm::StringRef name = recordDecl->getName();
		if (name == "Global")
			this->globalDecl = recordDecl;
		else if (name == "Instance")
			this->instanceDecl = recordDecl;
	}
}
	
} // namespace digi

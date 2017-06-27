#include <clang/AST/DeclGroup.h>
#include <clang/AST/DeclCXX.h>

#include "GlobalCollector.h"


namespace digi {


// GlobalCollector

GlobalCollector::GlobalCollector(clang::CodeGenerator* codeGenerator)
	: CodeGenWrapper(codeGenerator)
{
}

GlobalCollector::~GlobalCollector()
{
}

void GlobalCollector::handleDecl(clang::Decl* decl)
{
	// check if this is a variable declaration
	if (clang::VarDecl* varDecl = llvm::dyn_cast<clang::VarDecl>(decl))
	{
		// get variable name
		std::string name = varDecl->getNameAsString();
		
		this->decls[name] = varDecl;
	}
}

	
} // namespace digi

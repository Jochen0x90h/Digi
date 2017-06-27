#include <clang/AST/DeclGroup.h>
#include <clang/AST/DeclCXX.h>

#include "CodeGenWrapper.h"


namespace digi {


// CodeGenWrapper

CodeGenWrapper::CodeGenWrapper(clang::CodeGenerator* codeGenerator)
	: codeGenerator(codeGenerator)
{
}

CodeGenWrapper::~CodeGenWrapper()
{
}

void CodeGenWrapper::HandleTopLevelDecl(clang::DeclGroupRef DG)
{
	// call wrapped method
	this->codeGenerator->HandleTopLevelDecl(DG);
	
	// iterate over decls in group
	for (clang::DeclGroupRef::iterator I = DG.begin(), E = DG.end(); I != E; ++I)
	{
		if (clang::LinkageSpecDecl* lDecl = llvm::dyn_cast<clang::LinkageSpecDecl>(*I))
		{
			// linkage (e.g. extern "C")
			clang::DeclContext::decl_iterator it = lDecl->decls_begin();
			for (; it != lDecl->decls_end(); ++it)
			{
				this->handleDecl(*it);
			}
		}
		else
		{
			this->handleDecl(*I);
		}
	}
}

	
} // namespace digi

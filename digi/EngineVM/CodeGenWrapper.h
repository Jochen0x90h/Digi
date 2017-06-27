#ifndef digi_EngineVM_CodeGenWrapper_h
#define digi_EngineVM_CodeGenWrapper_h

#include <clang/AST/DeclBase.h>
#include <clang/CodeGen/ModuleBuilder.h>


namespace digi {

/// @addtogroup EngineVM
/// @{

/*
	wraps clang::CodeGenerator and handles all top level declarations while compiling.
	
	clang::CodeGenerator derives from clang::ASTConsumer
*/
class CodeGenWrapper : public clang::CodeGenerator
{
public:

	// wraps codeGenerator (takes ownership)
	CodeGenWrapper(clang::CodeGenerator* codeGenerator);
	virtual ~CodeGenWrapper();

	// wrap methods
	virtual void Initialize(clang::ASTContext &Context) {this->codeGenerator->Initialize(Context);}
	virtual void HandleTopLevelDecl(clang::DeclGroupRef DG);
	virtual void HandleTranslationUnit(clang::ASTContext &Ctx) {this->codeGenerator->HandleTranslationUnit(Ctx);}
	virtual void HandleTagDeclDefinition(clang::TagDecl *D)
	{
		//std::string name = D->getNameAsString();
		this->codeGenerator->HandleTagDeclDefinition(D);
	}
	virtual void CompleteTentativeDefinition(clang::VarDecl *D) {this->codeGenerator->CompleteTentativeDefinition(D);}
	virtual void HandleVTable(clang::CXXRecordDecl *RD, bool DefinitionRequired) {this->codeGenerator->HandleVTable(RD, DefinitionRequired);}
	virtual void PrintStats() {this->codeGenerator->PrintStats();}
	virtual llvm::Module* GetModule() {return this->codeGenerator->GetModule();}
	virtual llvm::Module* ReleaseModule() {return this->codeGenerator->ReleaseModule();}


	// analyze global variable
	virtual void handleDecl(clang::Decl* decl) = 0;

	
	// the wrapped code generator
	llvm::OwningPtr<clang::CodeGenerator> codeGenerator;
};


/// @}

} // namespace digi

#endif

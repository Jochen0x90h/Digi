#ifndef digi_SceneConvert_ParticlePass_h
#define digi_SceneConvert_ParticlePass_h

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Function.h>
#include <llvm/Pass.h>

#include "ShaderOptions.h"
#include "ShaderType.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

// ParticlePass, splits particle create/update into uniform and particle code
class ParticlePass : public llvm::FunctionPass
{
public:

	enum Scope
	{
		UNIFORM = 0,
		PARTICLE = 1,
	};

	enum Flags
	{
		USED_BY_UNIFORM = 1,
		USED_BY_PARTICLE = 2,
	};	


	// llvm pass identification, replacement for typeid 
	static char ID;

	ParticlePass();

	virtual ~ParticlePass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool doInitialization(llvm::Module& module);

	virtual bool runOnFunction(llvm::Function& function);
	
	
	llvm::OwningPtr<llvm::Module> modules[2];

	
	// transfer variables (uniform -> particle)
	std::vector<ShaderType> create_u2p;
	std::vector<ShaderType> update_u2p;
};


// ShapeParticlePass, splits shape particle instancer into uniform and particle code
class ShapeParticlePass : public llvm::FunctionPass
{
public:

	enum Scope
	{
		UNIFORM = 0,
		PARTICLE = 1,
	};

	enum Flags
	{
		USED_BY_UNIFORM = 1,
		USED_BY_PARTICLE = 2,
	};	


	// llvm pass identification, replacement for typeid 
	static char ID;

	ShapeParticlePass()
		: llvm::FunctionPass(ID)
	{
	}

	virtual ~ShapeParticlePass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool doInitialization(llvm::Module& module);

	virtual bool runOnFunction(llvm::Function& function);
	
	
	llvm::OwningPtr<llvm::Module> modules[2];

	
	// transfer variables (uniform -> particle)
	std::vector<ShaderType> u2p;
};

/// @}

} // namespace digi

#endif

#include <llvm/Instructions.h>
#include <llvm/Module.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Support/IRBuilder.h>

#include <digi/System/Log.h>
#include <digi/CodeGenerator/TypeInfo.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "CopyWalker.h"
#include "ParticlePass.h"
#include "PrintHelper.h"


namespace digi {

// ParticlePass

ParticlePass::ParticlePass()
	: llvm::FunctionPass(ID)
{
}

ParticlePass::~ParticlePass()
{
}

void ParticlePass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	analysisUsage.setPreservesAll();

	// tell llvm that we don't add or remove basic blocks or modify terminator instructions
	//analysisUsage.setPreservesCFG();

	// tell llvm wich other passes we need
//	analysisUsage.addRequired<llvm::TargetData>();
//	analysisUsage.addRequired<llvm::LoopInfo>();
	analysisUsage.addRequired<llvm::PostDominatorTree>();	
}

bool ParticlePass::doInitialization(llvm::Module& module)
{
	llvm::LLVMContext& c = module.getContext();
	
	// create modules for copied functions
	this->modules[UNIFORM].reset(new llvm::Module("particleU", c));
	this->modules[PARTICLE].reset(new llvm::Module("particleP", c));

	return false;	
} 

bool ParticlePass::runOnFunction(llvm::Function& function)
{
	llvm::StringRef name = function.getName();
	if (name == "create")
	{
		// create shader walker, creates the function in the modules. this object has ownership of modules
		CopyWalker walker(&this->getAnalysis<llvm::PostDominatorTree>(), function, this->modules);
	
		// set scopes of global inputs (get accessed by load instructions)
		walker.setScope(function, "emitter", UNIFORM);
		walker.setScope(function, "uniform", UNIFORM);
		walker.setScope(function, "particle", PARTICLE);
		walker.setScope(function, "index", PARTICLE);
		walker.setScope(function, "id", PARTICLE);
		walker.setScope(function, "seed", PARTICLE);

		// classify instructions

		// input/output
		walker.classifyInstructions(function, "particle", PARTICLE, USED_BY_PARTICLE);
		walker.classifyInstructions(function, "seed", PARTICLE, USED_BY_PARTICLE);
		
		// start processing at entry block
		walker.doBasicBlock(NULL, &function.getEntryBlock(), NULL);

		walker.fixAndGetTransfer(UNIFORM, PARTICLE, this->create_u2p, "create_u2p", false);
	}
	else if (name == "update")
	{
		// create shader walker, creates the function in the modules. this object has ownership of modules
		CopyWalker walker(&this->getAnalysis<llvm::PostDominatorTree>(), function, this->modules);
		
		// set scopes of global inputs (get accessed by load instructions)
		walker.setScope(function, "uniform", UNIFORM);
		walker.setScope(function, "particle", PARTICLE);
		walker.setScope(function, "index", PARTICLE);
		walker.setScope(function, "id", PARTICLE);
		walker.setScope(function, "seed", PARTICLE);

		// classify instructions

		// input/output
		walker.classifyInstructions(function, "particle", PARTICLE, USED_BY_PARTICLE);
		walker.classifyInstructions(function, "seed", PARTICLE, USED_BY_PARTICLE);

		// output
		walker.classifyInstructions(function, "alive", PARTICLE, USED_BY_PARTICLE);
		
		// start processing at entry block
		walker.doBasicBlock(NULL, &function.getEntryBlock(), NULL);

		walker.fixAndGetTransfer(UNIFORM, PARTICLE, this->update_u2p, "update_u2p", false);
	}
	else
	{
		// error: unexprected function
		dWarning("unexpected particle function '" << name << "' encountered");
	}

	return false;
}
	
char ParticlePass::ID = 0;

static llvm::RegisterPass<ParticlePass> registerParticlePass("digi-particle", "process particles");


// ShapeParticlePass

ShapeParticlePass::~ShapeParticlePass()
{
}

void ShapeParticlePass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	analysisUsage.setPreservesAll();

	// tell llvm that we don't add or remove basic blocks or modify terminator instructions
	//analysisUsage.setPreservesCFG();

	// tell llvm wich other passes we need
//	analysisUsage.addRequired<llvm::TargetData>();
//	analysisUsage.addRequired<llvm::LoopInfo>();
	analysisUsage.addRequired<llvm::PostDominatorTree>();	
}

bool ShapeParticlePass::doInitialization(llvm::Module& module)
{
	llvm::LLVMContext& c = module.getContext();
	
	// create modules for copied functions
	this->modules[UNIFORM].reset(new llvm::Module("shapeU", c));
	this->modules[PARTICLE].reset(new llvm::Module("shapeP", c));

	return false;	
} 

bool ShapeParticlePass::runOnFunction(llvm::Function& function)
{
	llvm::StringRef name = function.getName();
	if (name == "main")
	{
		// create shader walker, creates the function in the modules. this object has ownership of modules
		CopyWalker walker(&this->getAnalysis<llvm::PostDominatorTree>(), function, this->modules);
	
		// set scopes of global inputs (get accessed by load instructions)
		walker.setScope(function, "transform", UNIFORM);
		walker.setScope(function, "parentMatrix", UNIFORM);
		walker.setScope(function, "uniform", UNIFORM);
		walker.setScope(function, "particle", PARTICLE);
		walker.setScope(function, "index", PARTICLE);
		walker.setScope(function, "particleMatrix", PARTICLE);

		// classify instructions
		walker.classifyInstructions(function, "index", PARTICLE, USED_BY_PARTICLE);
		walker.classifyInstructions(function, "particleMatrix", PARTICLE, USED_BY_PARTICLE);
		
		// start processing at entry block
		walker.doBasicBlock(NULL, &function.getEntryBlock(), NULL);

		walker.fixAndGetTransfer(UNIFORM, PARTICLE, this->u2p, "u2p", false);
	}
	else
	{
		// error: unexprected function
		dWarning("unexpected shape particle instancer function '" << name << "' encountered");
	}

	return false;
}
	
char ShapeParticlePass::ID = 0;

static llvm::RegisterPass<ShapeParticlePass> registerShapeParticlePass("digi-shapeParticle", "process shape particle instancers");

} // namespace digi

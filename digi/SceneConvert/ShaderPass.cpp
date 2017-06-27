#include <llvm/Instructions.h>
#include <llvm/Module.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Support/IRBuilder.h>

#include <digi/System/Log.h>
#include <digi/CodeGenerator/TypeInfo.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "ShaderWalker.h"
#include "ShaderPass.h"
#include "ValueHelper.h"
#include "PrintHelper.h"


namespace digi {


ShaderPass::~ShaderPass()
{
}

void ShaderPass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	//analysisUsage.setPreservesAll();

	// tell llvm that we don't add or remove basic blocks or modify terminator instructions.
	// module is modified to flip y for calls to sample()
	analysisUsage.setPreservesCFG();

	// tell llvm wich other passes we need
//	analysisUsage.addRequired<llvm::TargetData>();
//	analysisUsage.addRequired<llvm::LoopInfo>();
	analysisUsage.addRequired<llvm::PostDominatorTree>();	
}

bool ShaderPass::doInitialization(llvm::Module& module)
{
	llvm::LLVMContext& c = module.getContext();
	
	// create modules for resulting functions for material, transform, vertex shader and pixel shader
	this->modules[ShaderWalker::MATERIAL].reset(new llvm::Module("shaderM", c));
	this->modules[ShaderWalker::TRANSFORM].reset(new llvm::Module("shaderT", c));
	this->modules[ShaderWalker::VERTEX].reset(new llvm::Module("shaderV", c));
	this->modules[ShaderWalker::PIXEL].reset(new llvm::Module("shaderP", c));

	return false;	
} 

bool ShaderPass::runOnFunction(llvm::Function& function)
{
	llvm::StringRef name = function.getName();
	if (name == "main")
	{
		// create shader walker, creates one function in the modules. this object has ownership of modules
		ShaderWalker walker(&this->getAnalysis<llvm::PostDominatorTree>(), function, this->modules,
			this->avoidPixelShader);
	
		// flip y coordinate of texture sample() functions
		if (this->flipY)
		{
			walker.sampleFlipY(function, "_Z6samplePU3AS2iDv2_f"); // sample(Texture2D, float2)
			//walker.sampleFlipY(function, "sample3D");
			//walker.sampleFlipY(function, "sampleCube");
		}			
	
		// set scopes of global inputs (get accessed by load instructions)
		walker.setScope(function, "viewMatrix", ShaderWalker::MATERIAL, ShaderWalker::INPUT);
		walker.setScope(function, "projectionMatrix", ShaderWalker::MATERIAL, ShaderWalker::INPUT);
		walker.setScope(function, "viewProjectionMatrix", ShaderWalker::MATERIAL, ShaderWalker::INPUT);
		walker.setScope(function, "viewport", ShaderWalker::MATERIAL, ShaderWalker::INPUT);
		walker.setScope(function, "deformer", ShaderWalker::MATERIAL, ShaderWalker::INPUT);
		walker.setScope(function, "material", ShaderWalker::MATERIAL, ShaderWalker::INPUT);
		walker.setScope(function, "matrix", ShaderWalker::TRANSFORM, ShaderWalker::INPUT); // world matrix of shape
		//walker.setScope(function, "transform", ShaderWalker::TRANSFORM, ShaderWalker::INPUT);
		walker.setScope(function, "objectId", ShaderWalker::TRANSFORM, ShaderWalker::INPUT);
		walker.setScope(function, "vertex", ShaderWalker::VERTEX, ShaderWalker::INPUT);
		walker.setScope(function, "s_o", ShaderWalker::VERTEX, ShaderWalker::INPUT); // scale/offset for packed vertex data
		walker.setScope(function, "frontFacing", (this->avoidPixelShader && this->frontFacingIsUniform) ?
			ShaderWalker::VERTEX : ShaderWalker::PIXEL, ShaderWalker::INPUT);
		walker.setScope(function, "pointCoord", ShaderWalker::PIXEL, ShaderWalker::INPUT);
		walker.setScope(function, "fragCoord", ShaderWalker::PIXEL, ShaderWalker::INPUT);

		// classify instructions (starting at outputs)

		// save value infos that stores the sopes of the global inputs
		std::map<const llvm::Value*, ShaderWalker::ValueInfo> valueInfos = walker.valueInfos;

		// classify sort output
		std::pair<llvm::Value*, int> sort = walker.classifyInstructions(function, "sort",
			ShaderWalker::MATERIAL, ShaderWalker::USED_BY_MATERIAL);
		if (sort.second == ShaderWalker::MATERIAL)
		{
			// sort is driven by material
			this->sortMode = SORT_MATERIAL;
		}
		else
		{
			// revert value infos as the output code doesn't need to write to sort
			walker.valueInfos.swap(valueInfos);
			
			if (sort.first == NULL)
			{
				// sort is not written to
				this->sortMode = SORT_NEVER;
			}
			else
			{
				// get sort value
				boost::optional<double> sortConstant = getConstantDouble(sort.first);
				if (sortConstant != null)
				{
					// sort is constant
					this->sortMode = sortConstant != 0.0 ? SORT_ALWAYS : SORT_NEVER;
				}
				else
				{
					// sort is driven by e.g. a texture, therefore always sort
					this->sortMode = SORT_ALWAYS;
				}
			}
		}
		
		// classify flip output
		walker.classifyInstructions(function, "flip", ShaderWalker::TRANSFORM, ShaderWalker::USED_BY_TRANSFORM);
		
		// classify vertex position output (projected)
		walker.classifyInstructions(function, "pPosition", ShaderWalker::VERTEX, ShaderWalker::USED_BY_VERTEX);
		
		// classify view space position output for user-defined clip planes
		walker.classifyInstructions(function, "vPosition", ShaderWalker::VERTEX, ShaderWalker::USED_BY_VERTEX);
				
		// classify point size output for sprites
		walker.classifyInstructions(function, "pointSize", ShaderWalker::VERTEX, ShaderWalker::USED_BY_VERTEX);

		// classify discard function
		walker.classifyInstructions(function, "discard", ShaderWalker::PIXEL, ShaderWalker::USED_BY_PIXEL);

		// classify output of pixel shader
		walker.classifyInstructions(function, "output", ShaderWalker::PIXEL, ShaderWalker::USED_BY_PIXEL);
		walker.classifyInstructions(function, "outputs", ShaderWalker::PIXEL, ShaderWalker::USED_BY_PIXEL);

		
		// start processing at entry block
		walker.doBasicBlock(NULL, &function.getEntryBlock(), NULL);

		// fix code and get transfer variables (e.g. shader uniforms for material -> pixel)
		walker.fixAndGetTransfer(ShaderWalker::MATERIAL, ShaderWalker::TRANSFORM, this->m2t, "m2t", false);
		walker.fixAndGetTransfer(ShaderWalker::MATERIAL, ShaderWalker::VERTEX, this->m2v, "m2v", false);
		walker.fixAndGetTransfer(ShaderWalker::MATERIAL, ShaderWalker::PIXEL, this->m2p, "m2p", false);
		walker.fixAndGetTransfer(ShaderWalker::MATERIAL, ShaderWalker::PIXEL, this->m2po, "m2po", true);
		walker.fixAndGetTransfer(ShaderWalker::TRANSFORM, ShaderWalker::VERTEX, this->t2v, "t2v", false);
		walker.fixAndGetTransfer(ShaderWalker::TRANSFORM, ShaderWalker::PIXEL, this->t2p, "t2p", false);
		walker.fixAndGetTransfer(ShaderWalker::VERTEX, ShaderWalker::PIXEL, this->v2p, "v2p", false);
	}
	else
	{
		// error: unexprected function
		dWarning("unexpected shader function '" << name << "' encountered");
	}

	return false;
}

	
char ShaderPass::ID = 0;

static llvm::RegisterPass<ShaderPass> registerShaderPass("digi-shader", "process shader");

} // namespace digi

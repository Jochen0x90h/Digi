#ifndef digi_SceneConvert_PrintPass_h
#define digi_SceneConvert_PrintPass_h

#include <llvm/Function.h>
#include <llvm/Pass.h>

#include <digi/CodeGenerator/CodeWriter.h>

#include "GlobalPrinter.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{
/*
struct DevLang
{
	Pointer<IODevice> device;
	Language language;

	DevLang()
		: language()
	{}
	DevLang(Pointer<IODevice> device, Language language)
		: device(device), language(language) {}
};
*/
class PrintPass : public llvm::FunctionPass
{
public:

	// llvm pass identification, replacement for typeid 
	static char ID;

	PrintPass()
		: llvm::FunctionPass(ID), devices(), globalPrinters()
	{
	}

	// note: pointer to devices and globalPrinters is stored
	PrintPass(std::map<std::string, Pointer<IODevice> >* devices, Language language,
		std::map<std::string, GlobalPrinterAccess>* globalPrinters)
		: llvm::FunctionPass(ID), devices(devices), language(language), globalPrinters(globalPrinters)
	{
	}

	virtual ~PrintPass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool runOnFunction(llvm::Function& function);
	
	// for every function a device for output
	std::map<std::string, Pointer<IODevice> >* devices;
	
	// language of generated code
	Language language;
	
	// for every global variable a printer object and access flags
	std::map<std::string, GlobalPrinterAccess>* globalPrinters;
};


/// @}

} // namespace digi

#endif

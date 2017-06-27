#include "HeaderBinder.h"


// HeaderBinder

HeaderBinder::HeaderBinder(Pointer<IODevice> dev, StringRef macroName, const std::map<std::string, std::string>& conditionals,
	const std::map<std::string, std::string>& functionNames, Mode mode)
	: dev(dev), macroName(macroName), conditionals(conditionals), functionNames(functionNames), mode(mode)
{
}

HeaderBinder::~HeaderBinder()
{
}

// FunctionCollector

FunctionCollector::FunctionCollector()
	: functionNames(), mode()
{
}

FunctionCollector::~FunctionCollector()
{
}

// WrapperGenerator

WrapperGenerator::WrapperGenerator(Pointer<IODevice> dev)
	: dev(dev)
{
}

WrapperGenerator::~WrapperGenerator()
{
}

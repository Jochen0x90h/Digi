#ifndef HeaderBinder_h
#define HeaderBinder_h

#include <iostream>
#include <fstream>
#include <map>

// #including boost/filesystem fails on gcc because rtti is off in HeaderBinder.cpp
#include <boost/version.hpp>

#include <digi/Utility/Pointer.h>
#include <digi/Utility/StringRef.h>
#include <digi/System/IODevice.h>

// clang
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclGroup.h>
#include <clang/AST/DeclCXX.h>


using namespace digi;

namespace boost {

#if BOOST_VERSION >= 105000
namespace filesystem
#else
namespace filesystem3
#endif
{
	class path;
}

}

enum Mode
{
	USE,
	CAST_TO_FUNCTION_TYPE,
	WGL_GET_PROC_ADDRESS,
};


// ast consumer that creates bindings of a llvm virtual machine to header files
// http://developer.apple.com/mac/library/DOCUMENTATION/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_entrypts/opengl_entrypts.html
// note: clang::ASTConsumer has no rtti, therefore the implementation is split in two parts: ctor and dtor in file with no rtti
class HeaderBinder : public clang::ASTConsumer
{
public:

	HeaderBinder(Pointer<IODevice> dev, StringRef macroName, const std::map<std::string, std::string>& conditionals,
		const std::map<std::string, std::string>& functionNames, Mode mode);
	virtual ~HeaderBinder();
	
	void handleDecl(clang::Decl* decl);

	// overrides method from clang::ASTConsumer
	virtual void HandleTopLevelDecl(clang::DeclGroupRef declGroup);
	
protected:

	Pointer<IODevice> dev;
	std::string macroName;
	const std::map<std::string, std::string>& conditionals;
	const std::map<std::string, std::string>& functionNames;
	Mode mode;
};


// coolects all functions in a header file
class FunctionCollector : public clang::ASTConsumer
{
public:
	FunctionCollector();
	virtual ~FunctionCollector();

	void handleDecl(clang::Decl* decl);

	// overrides method from clang::ASTConsumer
	virtual void HandleTopLevelDecl(clang::DeclGroupRef declGroup);

	#if BOOST_VERSION >= 105000
	void writeSetFunctions(const boost::filesystem::path& path);
	void writeNumFunctions(const boost::filesystem::path& path);
	#else
	void writeSetFunctions(const boost::filesystem3::path& path);
	void writeNumFunctions(const boost::filesystem3::path& path);
	#endif

	std::map<std::string, std::string> conditionals;
	const std::map<std::string, std::string>* functionNames;
	Mode mode;

protected:

	std::vector<std::pair<std::string, std::string> > functions;
};

// ast consumer that creates a wrappes for each function in a header file
class WrapperGenerator : public clang::ASTConsumer
{
public:

	WrapperGenerator(Pointer<IODevice> dev);
	~WrapperGenerator();
	
	void handleDecl(clang::Decl* decl);

	// overrides method from clang::ASTConsumer
	virtual void HandleTopLevelDecl(clang::DeclGroupRef declGroup);
	
protected:

	Pointer<IODevice> dev;
};

#endif

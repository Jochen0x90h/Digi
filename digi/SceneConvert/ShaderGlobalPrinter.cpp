#include "PrintWalker.h"
#include "ShaderGlobalPrinter.h"


namespace digi {

namespace
{
	void printOutputValue(CodeStream& w, llvm::Value* value, PrintWalker& printWalker)
	{
		// if the value is printed as scalar we have to cast to vector
		bool castToVector = getPrintType(value) != PRINT_VECTOR;
		if (castToVector)
		{
			printConstructor(w, value->getType(), INIT_SCALAR);
			w << '(';		
		}
		printWalker.printValue(w, value, PRIO_ASSIGN);
		if (castToVector)
			w << ')';
		w << ";\n";
	}
}


ShaderGlobalPrinter::~ShaderGlobalPrinter()
{
}

void ShaderGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	// print store into output variables
	llvm::StringRef name = global->getName();	
	Language language = w.getLanguage();
	/*
	if (name == "sort")
	{
		// sort flag
		w << "sort = ";
		printWalker.printValue(value, PRIO_ASSIGN, w);
		w << ";\n";
	}
	else*/ if (name == "vPosition")
	{
		// view space position for user clip planes
		w << "gl_ClipVertex = ";
		printWalker.printValue(w, value, PRIO_ASSIGN);
		w << ";\n";		
	}
	else if (name == "pPosition")
	{
		// projected position
		w << "gl_Position = ";
		printWalker.printValue(w, value, PRIO_ASSIGN);
		w << ";\n";
	}
	else if (name == "pointSize")
	{
		// point size
		w << "gl_PointSize = ";
		printWalker.printValue(w, value, PRIO_ASSIGN);
		w << ";\n";	
	}
	else if (name == "output")
	{
		if (language.isOldGLSL())
		{
			w << "gl_FragColor";
		}
		else
		{
			std::string name = '_' + this->nameGenerator.getCurrent();
			w << name;
			
			// set type of output variable (is one vector)
			this->output = ShaderVariable(name, ShaderType(ShaderType::FLOAT, 4));
		}
		w << " = ";
		printOutputValue(w, value, printWalker);
	}
	else if (name == "outputs")
	{
		if (language.isOldGLSL())
		{
			w << "gl_FragData";
		}
		else
		{
			std::string name = '_' + this->nameGenerator.getCurrent();
			w << name;
			
			// set type of output variable (is array of vectors)
			this->output = ShaderVariable(name, ShaderType(ShaderType::FLOAT, 4, getConstantIndex(indices[0])));
		}
		w << "[";
		printWalker.printValue(w, indices[0], PRIO_NONE);
		w << "] = ";
		printOutputValue(w, value, printWalker);
	}
}

void ShaderGlobalPrinter::printForLoad(CodeStream& w, llvm::Type*, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
	// print load from input variables
	llvm::StringRef name = global->getName();	

	if (name == "frontFacing")
	{
		w << "(gl_FrontFacing ? 1.0 : -1.0)";
	}
	else if (name == "pointCoord")
	{
		// point coordinates
		w << "gl_PointCoord";
	}
	else if (name == "fragCoord")
	{
		// fragment coordinates
		w << "gl_FragCoord";
	}
}

} // namespace digi

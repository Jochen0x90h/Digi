#include <digi/Utility/MapUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/FileSystem.h>
#include <digi/CodeGenerator/CodeWriter.h>

#include "HeaderBinder.h"


// HeaderBinder

void HeaderBinder::handleDecl(clang::Decl* decl)
{
	if (clang::FunctionDecl* functionDecl = llvm::dyn_cast<clang::FunctionDecl>(decl))
	{
		if (const clang::FunctionProtoType* functionType = functionDecl->getType()->getAs<clang::FunctionProtoType>())
		{
			CodeWriter w(this->dev);

			// get function name
			std::string functionName = functionDecl->getNameAsString();
			w.writeComment(functionName);

			// check if there is a conditional (#ifdef) for this name
			std::map<std::string, std::string>::const_iterator cIt = this->conditionals.find(functionName);
			if (cIt != this->conditionals.end())
				w << cIt->second << "\n\t";

			w << this->macroName << "(module, \"" << functionName << "\", ";
			if (this->mode == CAST_TO_FUNCTION_TYPE)
			{
				// cast to function type to disambiguate overloads (e.g. sin(float) and sin(double))
				
				// result type
				w << '(' << functionType->getResultType().getAsString() << " (*)(";

				// iterate over arguments
				clang::FunctionProtoType::arg_type_iterator it = functionType->arg_type_begin();
				int i = 0;
				for (; it != functionType->arg_type_end(); ++it, ++i)
				{
					if (i > 0)
						w << ", ";
					w << it->getAsString();
				}
				w << "))";
			}

			// print function name (also rename)
			w << getValue(this->functionNames, functionName, functionName) << ");\n";

			// close conditional
			if (cIt != this->conditionals.end())
				w << "#endif\n";
			
			w.writeLine();
			w.flush();
/*			
			// print result type
			std::cout << "result " << functionType->getResultType().getAsString() << std::endl;

			// create function type
			w << "argumentTypes.resize(" << functionDecl->getNumParams() << ");\n";

			// iterate over arguments
			clang::FunctionProtoType::arg_type_iterator it = functionType->arg_type_begin();
			int i = 0;
			for (; it != functionType->arg_type_end(); ++it, ++i)
			{
				w << "argumentTypes[" << i << "] = llvm::TypeBuilder<" << it->getAsString() << ", false>::get(context);\n";
			}

			// create function type
			w << "functionType = llvm::FunctionType::get(llvm::TypeBuilder<" << functionType->getResultType().getAsString()
				<< ", false>::get(context), argumentTypes, false);\n";

			// create function
			w << this->macroName << "(module, " << name << ", functionType);\n\n";
*/
		}
	}
}

void HeaderBinder::HandleTopLevelDecl(clang::DeclGroupRef declGroup)
{
	for (clang::DeclGroupRef::iterator it = declGroup.begin(); it != declGroup.end(); ++it)
	{
		clang::LinkageSpecDecl* lDecl = llvm::dyn_cast<clang::LinkageSpecDecl>(*it);
		if (lDecl != NULL)
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
			this->handleDecl(*it);			
		}
	}
}


// FunctionCollector

void FunctionCollector::handleDecl(clang::Decl* decl)
{
	if (clang::FunctionDecl* functionDecl = llvm::dyn_cast<clang::FunctionDecl>(decl))
	{
		if (const clang::FunctionProtoType* functionType = functionDecl->getType()->getAs<clang::FunctionProtoType>())
		{
			// get function name
			std::string functionName = functionDecl->getNameAsString();

			// determine host name (may be with cast and rename)
			std::string hostName;
			{
				StringRefCodeStream w(hostName);

				// cast to void*
				w << "(void*)";

				// write function definition
				if (this->mode == CAST_TO_FUNCTION_TYPE)
				{
					// cast to function type to disambiguate overloads (e.g. sin(float) and sin(double))
					
					// result type
					w << '(' << functionType->getResultType().getAsString() << " (*)(";

					// iterate over arguments
					clang::FunctionProtoType::arg_type_iterator it = functionType->arg_type_begin();
					int i = 0;
					for (; it != functionType->arg_type_end(); ++it, ++i)
					{
						if (i > 0)
							w << ", ";
						w << it->getAsString();
					}
					w << "))";
				}
				w << getValue(*this->functionNames, functionName, functionName);
			}
			this->functions += std::make_pair(functionName, hostName);
		}
	}
}

void FunctionCollector::HandleTopLevelDecl(clang::DeclGroupRef declGroup)
{
	for (clang::DeclGroupRef::iterator it = declGroup.begin(); it != declGroup.end(); ++it)
	{
		clang::LinkageSpecDecl* lDecl = llvm::dyn_cast<clang::LinkageSpecDecl>(*it);
		if (lDecl != NULL)
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
			this->handleDecl(*it);			
		}
	}
}

void FunctionCollector::writeSetFunctions(const fs::path& path)
{
	sort(this->functions);

	CodeWriter w(path);

	// write functions in alphabetical order
	typedef std::pair<std::string, std::string> FunctionPair;
	foreach (const FunctionPair& p, this->functions)
	{
		const std::string& functionName = p.first;
		w << "f->name = \"" << functionName << "\";\n";

		std::map<std::string, std::string>::const_iterator cIt = this->conditionals.find(functionName);
		if (cIt != conditionals.end())
		{
			w << cIt->second << "\n";
			w << "\tf->function = " << p.second << ";\n";
			w << "#else\n";
			w << "\tf->function = NULL;\n";
			w << "#endif\n";
		}
		else
		{
			w << "f->function = " << p.second << ";\n";
		}
		w << "++f;\n";
	}

	w.close();
}

void FunctionCollector::writeNumFunctions(const fs::path& path)
{
	CodeWriter w(path);
	w << "enum {NUM_FUNCTIONS = " << this->functions.size() << "};\n";
	w.close();
}


// WrapperGenerator

void WrapperGenerator::handleDecl(clang::Decl* decl)
{
	if (clang::FunctionDecl* functionDecl = llvm::dyn_cast<clang::FunctionDecl>(decl))
	{
		if (const clang::FunctionProtoType* functionType = functionDecl->getType()->getAs<clang::FunctionProtoType>())
		{
			CodeWriter w(this->dev);

			// get function name
			std::string functionName = functionDecl->getNameAsString();

			// get result type
			std::string resultType = functionType->getResultType().getAsString();

			// write function definition
			w << resultType << ' ' << functionName << '(';
			int i = 0;
			for (clang::FunctionProtoType::arg_type_iterator it = functionType->arg_type_begin(); it != functionType->arg_type_end(); ++it, ++i)
			{
				if (i > 0)
					w << ", ";
				w << it->getAsString() << ' ' << char('a' + i);
			}
			w << ")\n";
			w.beginScope();

			// write function implementation
			w << "return ::" << functionName << '(';
			int numArgs = i;
			for (i = 0; i < numArgs; ++i)
			{
				if (i > 0)
					w << ", ";
				w << char('a' + i);
			}
			w << ");\n";

			w.endScope();
			w.writeLine();
		}
	}
}

void WrapperGenerator::HandleTopLevelDecl(clang::DeclGroupRef declGroup)
{
	for (clang::DeclGroupRef::iterator it = declGroup.begin(); it != declGroup.end(); ++it)
	{
		clang::LinkageSpecDecl* lDecl = llvm::dyn_cast<clang::LinkageSpecDecl>(*it);
		if (lDecl != NULL)
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
			this->handleDecl(*it);			
		}
	}
}

#ifndef digi_CodeGenerator_NodeWriter_h
#define digi_CodeGenerator_NodeWriter_h

#include <string>
#include <fstream>
#include <sstream>

#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/Pointer.h>
#include <digi/Utility/MapUtility.h>

#include "CodeWriter.h"
#include "Path.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

struct PathState
{
	Path path;
	int written;
	
	PathState(const Path& path)
		: path(path), written(false) {}
};


/**
	The NodeWriter has a current node and can access its properties in
	the substitute function:

	$variable   : local variable
	$.attribute : attribute of current node
	$@attribute : type of attribute of current node
	$:          : node target path of current node
	$&          : name of node (only useful for comments)
	$@          : type of node (only useful for comments)
*/
class NodeWriter : public CodeWriter
{
	friend class TreeNode;
	friend class Attribute;
	
public:

	NodeWriter(Pointer<IODevice> dev = null)
		: CodeWriter(dev) {}
	NodeWriter(const std::string& fileString)
		: CodeWriter(fileString) {}
	virtual ~NodeWriter();
		
	void push(Pointer<LeafNode> node) {this->paths.push_back(Path(node));}
	void push(const Path& path) {this->paths.push_back(path);}
	void pop() {this->paths.pop_back();}

	void close() {this->CodeWriter::close();}

	
	// map from scope index to scope name
	std::map<int, std::string> scopes;

protected:

	virtual void writeLineInternal();
	
	// node/path stack
	std::vector<PathState> paths;
};

/// @}

} // namespace digi

#endif

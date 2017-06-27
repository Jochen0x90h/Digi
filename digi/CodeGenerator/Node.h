#ifndef digi_CodeGenerator_Node_h
#define digi_CodeGenerator_Node_h

#include <string>
#include <map>
#include <set>

#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/Object.h>
#include <digi/Utility/Pointer.h>
#include <digi/Math/All.h>

#include "CodeWriterFunctions.h"
#include "TreeNode.h"
#include "NodeVisitor.h"
#include "NodeWriter.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

class Node : public TreeNode
{
	
	public:

		Node(int scope = 1)
			: scope(scope) {}
		Node(StringRef name, int scope = 1)
			: TreeNode(name), scope(scope) {}
		virtual ~Node();
	
		// get the type name of the node (e.g. "Transform")
		virtual std::string getNodeType();

	// code generation
		
		// propagate scopes
		virtual void propagateScopes();

		// does nothing since this node is not an attribute of its parent
		virtual void propagateScopesAttributes(int scope);

		// propagate scope to this node and to its attributes
		virtual void propagateScopesNode(int scope);

		// get scope of this node
		virtual int getScope();

		// recursively sets the scope to this node and all its children (not recommended)
		virtual void addScopeRecursive(int scopeAdd);


		// calls inherited writeUpdateCodeInternal and writeUpdateCodeNode to update nodes without output attributes
		virtual void visit(NodeVisitor& v, const std::string& path, int fromScope, int toScope);

		// write update code for attributes of this node
		virtual void visitNode(NodeVisitor& v, int minScope, int maxScope);

		// write update code for attributes of a Node. does nothing since this is a Node.
		virtual void visitAttributes(NodeVisitor& v, int minScope, int maxScope);

		// write update code for this node. overwrite this method to write update code of a derived node
		virtual void writeUpdateCodeThis(NodeWriter& w);


		void setNodeScope(int scope) {this->scope = scope;}
		
	protected:
		
		// scope of node
		int scope;
};

/// @}

} // namespace digi

#endif 

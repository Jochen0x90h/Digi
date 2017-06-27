#include <digi/CodeGenerator/Node.h>
#include "GeometryDependent.h"


namespace digi {


// GeometryDependent

GeometryDependent::~GeometryDependent()
{
}


// GeometryVisitor

GeometryVisitor::~GeometryVisitor()
{
}

void GeometryVisitor::visitNode(Pointer<Node> node)
{
	if (GeometryDependent* geometryDependent = dynamic_cast<GeometryDependent*>(node.getPointer()))
	{
		this->w.push(node);
		this->w.beginScope();
		geometryDependent->writeUpdateCodeThis(w, this->geometry);
		this->w.endScope();
		this->w.pop();
	}
	else
	{
		this->WriteUpdateVisitor::visitNode(node);
	}
}

} // namespace digi

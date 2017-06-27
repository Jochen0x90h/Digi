#include "Node.h"
#include "NodeVisitor.h"


namespace digi {

// NodeVisitor

NodeVisitor::~NodeVisitor()
{
}

void NodeVisitor::visitConnection(const Connection& src, const Path& dstPath)
{
}

void NodeVisitor::visitAttribute(Pointer<Attribute> attribute)
{
}

void NodeVisitor::visitNode(Pointer<Node> node)
{
}

void NodeVisitor::visitNodeAttribute(Pointer<Attribute> attribute)
{
}


// TargetTypeVisitor

TargetTypeVisitor::~TargetTypeVisitor()
{
}

void TargetTypeVisitor::visitAttribute(Pointer<Attribute> attribute)
{
	if ((attribute->getFlags() & (Attribute::CONSTANT | Attribute::REFERENCE)) == 0)
		this->type->addMember(makeTargetPath(attribute->getPath(1)), attribute->getType());
}


// WriteInitVisitor

WriteInitVisitor::~WriteInitVisitor()
{
}

void WriteInitVisitor::visitAttribute(Pointer<Attribute> attribute)
{
	attribute->writeInitCode(this->w);
}


// WriteUpdateVisitor

WriteUpdateVisitor::~WriteUpdateVisitor()
{
}

void WriteUpdateVisitor::visitNode(Pointer<Node> node)
{
	this->w.push(node);
	this->w.beginScope();
	node->writeUpdateCodeThis(this->w);
	this->w.endScope();
	this->w.pop();
}

void WriteUpdateVisitor::visitConnection(const Connection& src, const Path& dstPath)
{
	Connection resolved = src.resolveConnection();

	// check if source attribute is in an inaccessible inner scope
	if (resolved.attribute != null && resolved.attribute->getScope() > maxScope)
		w << "// ";

	dstPath.writeVariable(this->w);
	this->w << " = ";
	resolved.writeVariable(this->w);
	this->w << ";\n";
}

} // namespace digi

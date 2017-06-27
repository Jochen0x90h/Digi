#include <gtest/gtest.h>

#include <digi/Utility/foreach.h>
#include <digi/CodeGenerator/Type.h>
#include <digi/CodeGenerator/NameMangler.h>
#include <digi/CodeGenerator/BinOpNode.h>
#include <digi/CodeGenerator/ScriptNode.h>
#include <digi/CodeGenerator/NameGenerator.h>

#include "InitLibraries.h"

using namespace digi;


namespace
{
	// test foreach on attributes and push/pop
	class MyNode : public Node
	{
		public:
		
			MyNode() {}
			virtual ~MyNode() {}
			
			void generateUpdateCodeThis(NodeWriter& w)
			{
				foreach (Path path, this->getPathElementList("foo.bar"))
				{
					w.push(path);
					
					w << "$.x = 0.0f;\n";
					
					w.pop();
				}		
			}
	};
	
	// test recursive generation of a part of the graph
	class RecursionNode : public Node
	{
	public:

		RecursionNode()
		{
			this->addAttribute("input1", "float3", Attribute::INPUT | Attribute::STOP);
			this->addOutput("output", "float3");
		}

		virtual void writeUpdateCodeThis(NodeWriter& w)
		{		
			// write inner graph
			{
				const int scope = 1;
				TargetTypeVisitor targetTypeVisitor;
				
				this->visit(targetTypeVisitor, "input1", scope, scope);

				std::string scope1Name = "inner";		
				targetTypeVisitor.type->writeVariable(w, scope1Name);
				scope1Name.swap(w.scopes[1]);
				WriteUpdateVisitor writeUpdateVisitor(w, scope);

				this->visit(writeUpdateVisitor, "input1", scope, scope);
				w << "float3 $input1 = $.input1;\n";

				scope1Name.swap(w.scopes[1]);
			}
			
			w << "$.output = $input1;\n";
		}

	};
}


TEST(CodeGenerator, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(CodeGenerator, CodeWriter)
{
		std::string str;
		StringRefCodeStream w(str);
		
		bool b = false;
		char ch = 'x';
		int8_t y = -1;
		uint8_t uy = 2;
		int16_t s = -3;
		uint16_t us = 4;
		int32_t i = -5;
		uint32_t ui = 6;
		int64_t l = -7;
		uint64_t ul = 8;
		size_t size = 9;
		float f = 10.0f;
		double d = 11.0;
				
		w << b << ch << y << uy << s << us << i << ui << l << ul << size << f << d;
		w << StringRef("s1");
		w << "s2";
		w << std::string("s3");
		const char* cstr = "s4";
		w << cstr;
		EXPECT_EQ(str, "falsex-12u-34u-56u-78u9u10.0f11.0s1s2s3s4");
}

TEST(CodeGenerator, NamedType)
{
	const char* typeString = "int";
		
	Pointer<Type> type1 = new NamedType("int");
	Pointer<Type> type2 = Type::create(typeString);
		
	EXPECT_EQ(type1->toString(), typeString);
	EXPECT_EQ(type2->toString(), typeString);
}

TEST(CodeGenerator, ArrayType)
{
	const char* typeString = "[5]int";

	Pointer<Type> type1 = new ArrayType(new NamedType("int"), 5);
	Pointer<Type> type2 = Type::create(typeString);
		
	EXPECT_EQ(type1->toString(), typeString);
	EXPECT_EQ(type2->toString(), typeString);
}

TEST(CodeGenerator, StructType)
{
	const char* typeString = "{foo int,bar [5]int}";

	Pointer<StructType> type1 = new StructType();
	type1->members += Variable("foo", new NamedType("int"));
	type1->members += Variable("bar", new ArrayType(new NamedType("int"), 5));
	Pointer<Type> type2 = Type::create(typeString);

	EXPECT_EQ(type1->toString(), typeString);
	EXPECT_EQ(type2->toString(), typeString);
		
	std::vector<std::string> membersArray;
	type1->getMembers(membersArray);
		
	std::set<std::string> membersSet;
	type1->getMembers(membersSet);
	
}

TEST(CodeGenerator, NameMangler)
{
	EXPECT_EQ(NameMangler::mangle("convert_float3", VectorInfo(VectorInfo::INT, 3)), "_Z14convert_float3Dv3_i");
	
	EXPECT_EQ(NameDemangler::demangle("_Z14convert_float3Dv3_i"), "convert_float3");	
}

TEST(CodeGenerator, Node)
{
	EXPECT_EQ(startsWithPath(".foo.bar.x", ".foo.bar.xy"), false);
	EXPECT_EQ(startsWithPath(".foo.bar.x", ".foo.bar.x"), true);
	EXPECT_EQ(startsWithPath(".foo.bar.x", ".foo.bar"), true);
	EXPECT_EQ(startsWithPath(".foo.bar.x", ".foo.b"), false);

	EXPECT_EQ(getPathElement(".foo.bar"), ".foo");
	EXPECT_EQ(getPathElement(".foo.bar", 4), ".bar");

	Pointer<Node> node = new Node("node");

	node->addConstant("foo", vector3(1.0f, 2.0f, 3.0f));
	node->connect("foo.y", Path(node, "foo.z"));

	node->addAttribute("zzz.bar", "float");

	node->addAttribute("bar.1.x", "float");
	node->addAttribute("bar.3.x", "float");
	node->addAttribute("bar.4.x", "float");
		
	// test if attribute with name that already exists gets rejected
	EXPECT_EQ(node->addAttribute("foo", "float"), null);
		
	// test getType
	EXPECT_EQ(node->getType("foo"), "float3");
	EXPECT_EQ(node->getType("foo.x"), "float");
	EXPECT_EQ(node->getType("foo.y"), "float");
	EXPECT_EQ(node->getType("foo.z"), "float");

	// test getInitializer
	EXPECT_EQ(node->getInitializer("foo"), "vector3(1.0f, 2.0f, 3.0f)");
	EXPECT_EQ(node->getInitializer("foo.x"), "1.0f");
	EXPECT_EQ(node->getInitializer("foo.y"), "2.0f");
	EXPECT_EQ(node->getInitializer("foo.z"), "3.0f");

	// test isConnected
	EXPECT_EQ(node->isConnected("foo"), true);
	EXPECT_EQ(node->isConnected("foo.x"), false);
	EXPECT_EQ(node->isConnected("foo.y"), true);
	EXPECT_EQ(node->isConnected("foo.z"), false);
	EXPECT_EQ(node->isConnected("zzz.bar"), false);
		
	// test hasAttribute
	EXPECT_EQ(node->hasAttribute("zzz"), true);
	EXPECT_EQ(node->hasTypedAttribute("zzz"), false);

	// test iteration over children of attribute "zzz"
	foreach (Path path, node->getPathElementList("zzz"))
	{
		EXPECT_EQ(path.path, ".zzz.bar");
	}

	static const char* paths[] = {".bar.1", ".bar.3", ".bar.4"};
	int i = 0;
	foreach (Path path, node->getPathElementList("bar"))
	{
		EXPECT_EQ(path.path, paths[i]);
		++i;
	}
}

TEST(CodeGenerator, NodeStruct)
{
	Pointer<TreeNode> graph = new TreeNode("graph");

	Pointer<Node> node = new MyNode();
	graph->addNode(node, "node");
	node->addInput("foo.bar[1].x", "float");
	node->addInput("foo.bar[2].x", "float");
	node->addInput("foo.bar[3].x", "float");
	node->addInput("foo.bar[3].y", "float");
	node->addOutput("output", "float");

	Pointer<Node> subNode = new Node();
	node->addNode(subNode, "subNode");
	subNode->addAttribute("bar", "float");

	// find sub-node
	Pointer<Node> subNodeFound = staticCast<Node>(node->findAttribute("subNode", Node::FIND_UNTYPED).node);
	EXPECT_EQ(subNode, subNodeFound);

	NodeWriter w("testNodeStruct.h");
	w.scopes[1] = "local";
	
	Pointer<StructType> t = graph->getTargetType(1);
	t->writeType(w, "T1Local");
	w.writeLine();
	
	w.writeLine("// init");
	w.beginScope();
	graph->writeInitCode(w, 1);
	w.endScope();
	w.writeLine();

	w.writeLine("// update");
	w.beginScope();
	graph->writeUpdateCode(w, 1);
	w.endScope();

	w.close();
}

TEST(CodeGenerator, NodeGraph)
{
	// graph 1
	Pointer<TreeNode> g1 = new TreeNode("g1");
	Pointer<Node> n1 = new ScriptNode("$.output = $.input1 + $.input2;\n");
	n1->addInput("input1", "float3");
	n1->addState("input2", "float3");
	n1->setInitializer("input2", vector3(1.0f, 2.0f, 3.0f));
	n1->addOutput("output", "float3");
	g1->addNode(n1, "n1");
	
	Pointer<Node> n2 = new BinOpNode(splat3(2.0f), BinOpNode::OP_MUL);
	g1->addNode(n2, "n2");
	n2->connect("input1", Path(n1, "output"));

	// a state attribute that connects to graph 1
	n2->addAttribute("attr", "float3", Path(n1, "output"), 0, 0);
	
	// graph 2 (connects to graph 1)
	Pointer<TreeNode> g2 = new TreeNode("g2");
	Pointer<Node> n3 = new BinOpNode(5.0f, BinOpNode::OP_DIV);
	g2->addNode(n3, "n3");
	n3->connect("input1", Path(n2, "output.x"));
	
	Pointer<Node> n4 = new BinOpNode(2.0f, BinOpNode::OP_MUL);
	g2->addNode(n4, "n4");
	n4->connect("input1", Path(n3, "output"));

	// graph 3 (connects to graph 1 and graph 2)
	Pointer<TreeNode> g3 = new TreeNode("g3");
	Pointer<Node> n5 = new BinOpNode(BinOpNode::OP_SUB, "float");
	g3->addNode(n5, "n5");
	n5->connect("input1", Path(n2, "output.y"));
	n5->connect("input2", Path(n4, "output"));
	

	EXPECT_EQ(n1->getPath(), "g1.n1");


	NodeWriter w("testNodeGraph.h");
	w.scopes[0] = "state";
	w.scopes[1] = "local";

	// generate structures
	Pointer<StructType> state = g1->getTargetType(0);
	Pointer<StructType> t1 = g1->getTargetType(1);
	Pointer<StructType> t2 = g2->getTargetType(1);
	Pointer<StructType> t12 = g2->getConnectionTargetType(g1);
	Pointer<StructType> t3 = g3->getTargetType(1);
	Pointer<StructType> t13 = g3->getConnectionTargetType(g1);
	Pointer<StructType> t23 = g3->getConnectionTargetType(g2);

	
	state->writeVariable(w, "State");
	t1->writeVariable(w, "T1Local");
	t2->writeVariable(w, "T2Local");
	t12->writeVariable(w, "T1T2");
	t3->writeVariable(w, "T3Local");
	t13->writeVariable(w, "T1T3");
	t23->writeVariable(w, "T2T3");
	w.writeLine();

	// graph 1
	w.writeLine("// init state");
	w.beginScope();
	g1->writeInitCode(w, 0);
	w.endScope();
	w.writeLine();

	w.writeLine("// init g1");
	w.beginScope();
	g1->writeInitCode(w, 1);
	w.endScope();
	w.writeLine();

	w.writeLine("// update g1");
	w.beginScope();
	g1->writeUpdateCode(w, 0, 1);
	w.writeLine();

	w.writeLine("// output to g2");
	g2->writeOutputCode(w, g1, "output." + g2->getName());
	w.writeLine();

	w.writeLine("// output to g3");
	g3->writeOutputCode(w, g1, "output." + g3->getName());
	w.endScope();
	w.writeLine();

	// graph 2 (connects to graph 1)
	w.writeLine("// init g2");
	w.beginScope();
	g2->writeInitCode(w, 1);
	w.endScope();
	w.writeLine();
	
	w.writeLine("// update g2");
	w.beginScope();

	w.writeLine("// input from g1");
	g2->writeInputCode(w, g1, "input");
	w.writeLine();

	g2->writeUpdateCode(w, 1);
	w.writeLine();

	w.writeLine("// output to g3");
	g3->writeOutputCode(w, g2, "output." + g3->getName());
	w.endScope();
	w.writeLine();
	
	// graph 3 (connects to graph 1 and graph 2)
	w.writeLine("// init g3");
	w.beginScope();
	g3->writeInitCode(w, 1);
	w.endScope();
	w.writeLine();
	
	w.writeLine("// update g3");
	w.beginScope();
	
	w.writeLine("// input from g1");	
	g3->writeInputCode(w, g1, "i1");
	w.writeLine();
	
	w.writeLine("// input from g2");	
	g3->writeInputCode(w, g2, "i2");
	w.writeLine();

	g3->writeUpdateCode(w, 1);
	w.endScope();

	w.close();
}

TEST(CodeGenerator, NodeGraph2)
{
	Pointer<TreeNode> g = new TreeNode("g");

	// scope 1
	Pointer<Node> s1n1 = new ScriptNode("$.output = $.input1 + $.input2;\n");
	s1n1->addInput("input1", "float3");
	s1n1->addState("input2", "float3");
	//s1n1->setInitializer("input2", vector3(1.0f, 2.0f, 3.0f));
	s1n1->addOutput("output", "float3");
	g->addNode(s1n1, "s1n1");
	
	Pointer<Node> s1n2 = new BinOpNode(splat3(2.0f), BinOpNode::OP_MUL);
	g->addNode(s1n2, "s1n2");
	s1n2->connect("input1", Path(s1n1, "output"));
	
	// a state attribute that connects to scope 1
	s1n2->addAttribute("attr", "float3", Path(s1n1, "output"), 0, 0);
	
	// scope 2 (connects to scope 1)
	Pointer<Node> s2n1 = new BinOpNode(5.0f, BinOpNode::OP_DIV);
	s2n1->setNodeScope(2);
	g->addNode(s2n1, "s2n1");
	s2n1->connect("input1", Path(s1n1, "output.x"));
	//s2n1->setScope("input1", 2);
	
	Pointer<Node> s2n2 = new BinOpNode(vector3(1.0f, 2.0f, 3.0f), BinOpNode::OP_MUL);
	//s2n2->setNodeScope(2);
	g->addNode(s2n2, "s2n2");
	s2n2->connect("input1.x", Path(s2n1, "output")); // input1.y to scope 2
	s2n2->connect("input1.y", Path(s1n2, "output.x")); // input1.y to scope 1
	s2n2->connect("input1.z", Path(s1n2, "output.y")); // input1.z to scope 1

	// scope 3 (connects to scope 1 and 2)
	Pointer<Node> s3n1 = new BinOpNode(BinOpNode::OP_SUB, "float");
	s3n1->setNodeScope(3);
	g->addNode(s3n1, "s3n1");
	//s3n1->connect("input1", Path(s1n2, "output.y"));
	s3n1->connect("input1", Path(s1n1, "input2.y"));
	s3n1->connect("input2", Path(s2n2, "output.x"));
	
	// propagate scopes
	g->propagateScopes();
	
	
	// test
	EXPECT_EQ(s1n1->getPath(), "g.s1n1");

	
	// write
	NodeWriter w("testNodeGraph2.h");
	w.scopes[0] = "state";
	w.scopes[1] = "scope1";
	w.scopes[2] = "scope2";
	w.scopes[3] = "scope3";
	
	// generate structures
	Pointer<StructType> state = g->getTargetType(0);
	Pointer<StructType> t1 = g->getTargetType(1);
	Pointer<StructType> t2 = g->getTargetType(2);
	Pointer<StructType> t12 = g->getOutputTargetType(0, 1, 2);
	Pointer<StructType> t3 = g->getTargetType(3);
	Pointer<StructType> t13 = g->getOutputTargetType(0, 1, 3);
	Pointer<StructType> t23 = g->getOutputTargetType(2, 2, 3);

	
	state->writeVariable(w, "State");
	t1->writeVariable(w, "T1");
	t2->writeVariable(w, "T2");
	t12->writeVariable(w, "T12");
	t3->writeVariable(w, "T3");
	t13->writeVariable(w, "T13");
	t23->writeVariable(w, "T23");
	w.writeLine();

	// state
	w.writeLine("// init state");
	w.beginScope();
	g->writeInitCode(w, 0);
	w.endScope();
	w.writeLine();

	// scope 1
	w.writeLine("// init scope 1");
	w.beginScope();
	g->writeInitCode(w, 1);
	w.endScope();
	w.writeLine();

	w.writeLine("// update state and scope 1");
	w.beginScope();
	g->writeUpdateCode(w, 0, 1);
	w.writeLine();

	w.writeLine("// output to scope 2");
	g->writeOutputCode(w, 0, 1, 2, "output");
	std::vector<std::string> assignments;
	g->getOutputAssignments(w.scopes, assignments, 0, 1, 2);
	w.writeLine();

	w.writeLine("// output to scope 3");
	g->writeOutputCode(w, 0, 1, 3, "output");
	w.endScope();
	w.writeLine();

	// scope 2 (connects to scope 1)
	w.writeLine("// init scope 2");
	w.beginScope();
	g->writeInitCode(w, 2);
	w.endScope();
	w.writeLine();
	
	w.writeLine("// update scope 2");
	w.beginScope();

	g->writeUpdateCode(w, 2);
	w.writeLine();

	w.writeLine("// output to scope 3");
	g->writeOutputCode(w, 2, 2, 3, "output");
	w.endScope();
	w.writeLine();
	
	// scope 3 (connects to scope 1 and scope 2)
	w.writeLine("// init scope 3");
	w.beginScope();
	g->writeInitCode(w, 3);
	w.endScope();
	w.writeLine();
	
	w.writeLine("// update scope 3");
	w.beginScope();
	g->writeUpdateCode(w, 3);
	w.endScope();

	w.close();
}

TEST(CodeGenerator, NodeGraph3)
{
	Pointer<TreeNode> g = new TreeNode("g");

	Pointer<Node> n1 = new ScriptNode("$.output = $.input1 + $.input2;\n");
	n1->addInput("input1", "float3");
	n1->addState("input2", "float3");
	n1->setInitializer("input2", vector3(1.0f, 2.0f, 3.0f));
	n1->addOutput("output", "float3");
	g->addNode(n1, "n1");

	// this node evaluates the sub-graph connected to input1 recursively inside writeUpdateCodeThis
	Pointer<Node> s1n2 = new RecursionNode();
	g->addNode(s1n2, "s1n2");
	s1n2->connect("input1", Path(n1, "output"));
	
	Pointer<Node> n3 = new BinOpNode(BinOpNode::OP_MUL, "float3");
	g->addNode(n3, "n3");
	n3->connect("input1", Path(s1n2, "input1"));
	n3->connect("input2", Path(s1n2, "output"));

	// write
	NodeWriter w("testNodeGraph3.h");
	w.scopes[0] = "state";
	w.scopes[1] = "s1";
	
	Pointer<StructType> state = g->getTargetType(0);
	state->writeVariable(w, "state");
	w.writeLine();
	Pointer<StructType> t1 = n3->getTargetTypeOfConnected(1);
	t1->writeVariable(w, "s1");
	w.writeLine();
	
	// state
	w.writeLine("// init state");
	w.beginScope();
	g->writeInitCode(w, 0);
	w.endScope();
	w.writeLine();

	// scope 1
	w.writeLine("// init scope 1");
	w.beginScope();
	g->writeInitCode(w, 1);
	w.endScope();
	w.writeLine();

	w.writeLine("// update state and scope 1");
	w.beginScope();
	g->writeUpdateCode(w, 0, 1);
	w.writeLine();
	w.endScope();
}

TEST(CodeGenerator, ConvertIntToIdentifier)
{
	char buf[10];
	const char* b = buf;
		
	*convertIntToIdentifier(buf, 0) = 0;
	EXPECT_EQ(b, std::string("a"));

	*convertIntToIdentifier(buf, 26) = 0;
	EXPECT_EQ(b, std::string("A"));

	*convertIntToIdentifier(buf, 52) = 0;
	EXPECT_EQ(b, std::string("ab"));

	*convertIntToIdentifier(buf, 52*2) = 0;
	EXPECT_EQ(b, std::string("ac"));

	*convertIntToIdentifier(buf, 52*62-1) = 0;
	EXPECT_EQ(b, std::string("Z9"));

	*convertIntToIdentifier(buf, 52*62) = 0;
	EXPECT_EQ(b, std::string("aab"));
}

TEST(CodeGenerator, NameGenerator)
{
	NameGenerator nameGenerator;
	EXPECT_EQ(nameGenerator.get(), std::string("a"));
	nameGenerator.next(0);
	EXPECT_EQ(nameGenerator.get(), std::string("b"));
	nameGenerator.next();
	EXPECT_EQ(nameGenerator.get(), std::string("d"));
	nameGenerator.next(2);
	EXPECT_EQ(nameGenerator.get(), std::string("g"));
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

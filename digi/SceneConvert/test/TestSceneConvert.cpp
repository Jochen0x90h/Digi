#include <gtest/gtest.h>

#include <digi/Base/VersionInfo.h>
#include <digi/Utility/ArrayUtility.h>
#include <digi/System/MemoryDevices.h>
#include <digi/System/Log.h>
#include <digi/System/ConsoleLogChannel.h>
#include <digi/Math/GTestHelpers.h>
#include <digi/Image/JPEGWrapper.h>
#include <digi/Image/PNGWrapper.h>
#include <digi/Engine/ParameterType.h>
#include <digi/Engine/RenderJob.h>
#include <digi/EngineVM/Compiler.h>
#include <digi/Scene/SceneFile.h>
#include <digi/Scene/Shape.h>
#include <digi/Scene/Mesh.h>
#include <digi/SceneConvert/CompileHelper.h>
#include <digi/SceneConvert/compileShader.h>
#include <digi/SceneConvert/compileSceneWebGL.h>
#include <digi/SceneConvert/generateSceneOpenGL.h>
#include <digi/SceneConvert/generateSceneWebGL.h>
#include <digi/SceneConvert/PrintPass.h>
#include <digi/SceneConvert/ScalarizerPass.h>
#include <digi/SceneConvert/TextureDataInfo.h>

#include "InitLibraries.h"

using namespace digi;


static float animXValuesData[] =
{
	0.0f,
	1.0f,
	2.0f,
	3.0f
};

static float animKeysData[] =
{
	      0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	0.0f, 0.0f
};


static float3 positionData[] =
{
	{-0.4f,-0.4f, 0.0f},
	{ 0.4f,-0.4f, 0.0f},
	{ 0.0f, 0.4f, 0.0f}
};

static float2 texCoordData1[] =
{
	{0.0f, 1.0f},
	{1.0f, 1.0f},
	{0.5f, 0.0f}
};

static float2 texCoordData2[] =
{
	{-0.2f, -0.2f},
	{ 1.2f, -0.2f},
	{-0.2f,  1.2f}
};

static ushort indices[] = {0, 1, 2};

static void writeString(const std::string& str, const fs::path& path)
{
	Pointer<IODevice> file = File::create(path);
	file->write(str.data(), str.size());
	file->close();
}


TEST(SceneConvert, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
		
	// matrix must be 16-aligned for vm
	EXPECT_EQ(offsetof(RenderJob, matrix) & 15, 0);

	// add log channel for compiler warnings
	Log::addChannel(new ConsoleLogChannel());
}

TEST(SceneConvert, GetSequenceNameAndIndex)
{
	std::pair<std::string, int> pe = getSequenceNameAndIndex("");
	std::pair<std::string, int> pm = getSequenceNameAndIndex("sequence");
	std::pair<std::string, int> p1 = getSequenceNameAndIndex("sequence1");
	std::pair<std::string, int> p2 = getSequenceNameAndIndex("sequence.2");
	std::pair<std::string, int> p3 = getSequenceNameAndIndex("sequence.03");
	
	EXPECT_EQ(pe.first, "");
	EXPECT_EQ(pm.first, "sequence.#");
	EXPECT_EQ(p1.first, "sequence#");
	EXPECT_EQ(p2.first, "sequence.#");
	EXPECT_EQ(p3.first, "sequence.#");

	EXPECT_EQ(pe.second, -1);
	EXPECT_EQ(pm.second, -1);
	EXPECT_EQ(p1.second, 1);
	EXPECT_EQ(p2.second, 2);
	EXPECT_EQ(p3.second, 3);
}

TEST(SceneConvert, CompileShader)
{
	std::string code;
	{
		Pointer<IODevice> dev = new StringRefDevice(code);
		CodeWriter w2(dev);
	
		Pointer<Type> deformerType = Type::create("{f float,floats [10]float,vectors [10]float4,matrices [10]float4x4}");
		Pointer<Type> materialType = Type::create("{color float4}");
		Pointer<Type> vertexType = Type::create("{a1 float,a2 float2,b2 float2,a3 float3,index int}");
		
		// variables
		writeBindingVariable(w2, "deformer", deformerType);
		writeBindingVariable(w2, "material", materialType);
		writeBindingVariable(w2, "vertex", vertexType);
		writeBindingVariable(w2, "vPosition", "float3");
		writeBindingVariable(w2, "pPosition", "float4");			
		writeBindingVariable(w2, "sort", "bool");
		writeBindingVariable(w2, "discard", "bool");
		writeBindingVariable(w2, "output", "float4");
		writeBindingVariable(w2, "outputs", Type::create("[8]float4"));
		w2.writeLine();
		
		// shader function
		w2 << "extern \"C\" void main()\n";
		w2.beginScope();
		
		// test if clause
		w2 << "float3 f3;\n";
		w2 << "if (vertex.a1 < 0.0f)\n";
		w2 << "  f3 = vertex.a3;\n";
		w2 << "else\n";
		w2 << "  f3 = vertex.a1;\n"; // assign scalar to vector
						
		w2 << "float x = 0;\n";
		//w2 << "for (int i = 0; i < 10; ++i)\n";
		//w2 << "  x += deformer.ar[i];\n";
		
		w2 << "vPosition = vector3(5.1f, 5.2f, 5.3f);\n";
		w2 << "float4 p;\n";
		w2 << "p.x = -vertex.a1 + 1.0f / sqrt(vertex.a1) * deformer.floats[vertex.index] + (deformer.vectors[vertex.index].y + material.color.x * deformer.matrices[0].z.w);\n";
		//w2 << "p.x = -vertex.a1 + 1.0f / sqrt(vertex.a1) * deformer.f;\n";
		w2 << "p.y = round(vertex.a1) + frac(vertex.a1) + step(0.5f, vertex.a1) * 3.0f;\n";
		w2 << "p.zw = min(vertex.a2, vertex.b2) + min(vertex.a2, 0.0f) + min(splat2(0.0f), vertex.a2);\n";
		w2 << "pPosition = p;\n";

		w2 << "float3 color = normalize(f3);\n";
		w2 << "float3 alpha = material.color.w;\n";

		w2 << "sort = alpha.x < 0.99f;\n";
		w2 << "discard = (color.x < 0.01f) & (color.y < 0.01f) & (color.z < 0.01f) & (alpha.x < 0.01f);\n";
		w2 << "output = vector4(color, alpha.x);\n";
		w2 << "outputs[1] = vector4(color, alpha.x);\n";
		w2.endScope();
	}

	// input code
	writeString(code, "shader.cpp");

	// compile
	std::string materialTransfer;
	std::string transformTransfer;
	std::string vertexShader;
	std::string pixelShader;
	std::vector<ShaderVariable> transferVariables;
	std::vector<ShaderVariable> materialVariables;
	std::vector<ShaderVariable> materialTextures;
	std::vector<ShaderVariable> transformVariables;
	std::map<std::string, ShaderVariable> vertexBindings;
	SortMode sortMode;
	int useFlags;
	int2 numTextures;
	
	ShaderOptions shaderOptions;
	
	Compiler compiler(Compiler::INTERMEDIATE);

	compileShader(
			compiler, code,
			int3(),
			int3(),
			materialTransfer,
			transformTransfer,
			vertexShader,
			pixelShader,
			transferVariables,
			materialVariables,
			materialTextures,
			transformVariables,
			vertexBindings,
			sortMode,
			useFlags,
			numTextures,
			Language::CPP, Language::GLSL_1_2,
			shaderOptions,
			true);
		
	// output code
	writeString(materialTransfer, "shader.m.cpp");
	writeString(transformTransfer, "shader.t.cpp");
	writeString(vertexShader, "shader.vs.glsl");
	writeString(pixelShader, "shader.ps.glsl");
}

TEST(SceneConvert, CompileSceneWebGL)
{
	std::string code;
	std::map<std::string, int> buffers;
	{
		Pointer<IODevice> dev = new StringRefDevice(code);
		CodeWriter w2(dev);
				
		Pointer<Type> stateType = Type::create("{f1 float,v1 float3}");
		Pointer<Type> buffersType = Type::create("{b [100]float}");
		buffers["b"] = 0;
		Pointer<Type> uniformsType = Type::create("{x float}");
		Pointer<Type> transformsType = Type::create("[1]Transform");
		Pointer<Type> boundingBoxesType = Type::create("[1]BoundingBox");
		
		// variables
		writeBindingVariable(w2, "state", stateType);
		writeBindingVariable(w2, "buffers", buffersType);
		writeBindingVariable(w2, "uniforms", uniformsType);
		writeBindingVariable(w2, "transforms", transformsType);
		writeBindingVariable(w2, "boundingBoxes", boundingBoxesType);
		w2.writeLine();
		
		// initState
		w2 << "extern \"C\" void initState()\n";
		w2.beginScope();
		w2 << "state.f1 = buffers.b[55];\n";
		w2 << "state.v1 = 0.0f;\n";
		w2.endScope();

		// update
		w2 << "extern \"C\" void update()\n";
		w2.beginScope();
		w2 << "state.f1 *= 3.0f;\n";
		w2 << "state.v1 *= 5.0f * state.f1;\n";
		w2 << "uniforms.x = state.f1;\n";
		w2 << "transforms[0].matrix.x.x = state.f1;\n";
		w2 << "boundingBoxes[0].center = 0.0f;\n";
		w2 << "boundingBoxes[0].size = 1.0f;\n";
		w2.endScope();
	}

	// input code
	writeString(code, "scene.cpp");

	// compile
	Compiler compiler(Compiler::INTERMEDIATE);

	Pointer<SplittedGlobalPrinter> statePrinter;
	Pointer<SplittedGlobalPrinter> uniformsPrinter;
	Pointer<SplittedGlobalPrinter> transformsPrinter;
	Pointer<SplittedGlobalPrinter> boundingBoxesPrinter;
	std::string initStateCode;
	std::string updateCode;
	int useFlags;
	compileSceneWebGL(
		compiler,
		code,
		buffers,
		statePrinter,
		uniformsPrinter,
		transformsPrinter,
		boundingBoxesPrinter,
		initStateCode,
		updateCode,
		useFlags);			
			
	// output code
	writeString(initStateCode, "scene.i.js");
	writeString(updateCode, "scene.u.js");
	
	int3 o0 = statePrinter->getOffset(".v1.y");
	int3 o1 = uniformsPrinter->getOffset(".x");
	int3 o2 = transformsPrinter->getOffset("[0].matrix.w.w");
	int3 o3 = boundingBoxesPrinter->getOffset("[0].size");
	EXPECT_VECTOR_EQ(o0, vector3(0, 2, 0));
	EXPECT_VECTOR_EQ(o1, vector3(0, 0, 0));
	EXPECT_VECTOR_EQ(o2, vector3(0, 15, 0));
	EXPECT_VECTOR_EQ(o3, vector3(0, 3, 0));

	int3 s0 = statePrinter->getSize();
	int3 s1 = uniformsPrinter->getSize();
	int3 s2 = transformsPrinter->getSize();
	int3 s3 = boundingBoxesPrinter->getSize();
	EXPECT_VECTOR_EQ(s0, vector3(0, 4, 0));
	EXPECT_VECTOR_EQ(s1, vector3(0, 1, 0));
	EXPECT_VECTOR_EQ(s2, vector3(1, 16, 0));
	EXPECT_VECTOR_EQ(s3, vector3(0, 6, 0));
}

TEST(SceneConvert, SceneConvert)
{
	Pointer<SceneFile> sceneFile = new SceneFile();
	
	// texture
	sceneFile->textures += new TextureImage("digi-credit", Texture::TEXTURE, loadPNG("digi-credit.png"));
	
	// texture sequence
	Pointer<TextureSequence> textureSequence = new TextureSequence("sequence.#", Texture::TEXTURE);
	sceneFile->textures += textureSequence;
	ImageSequence& sequence = textureSequence->imageSequence;
	sequence[1] = loadJPEG("sequence.1.jpg");
	sequence[2] = loadJPEG("sequence.2.jpg");
	sequence[3] = loadJPEG("sequence.3.jpg");
	
	// font texture
	sceneFile->textures += new TextureImage("Cyberbit", Texture::TEXTURE, loadPNG("Cyberbit.png"));

	// symbol map
	Pointer<SymbolMap> symbolMap = new SymbolMap("symbolMap");
	sceneFile->textures += symbolMap;
	symbolMap->uv2posX = 2.46154f;
	symbolMap->uv2posY = 2.46154f;
	symbolMap->symbols["a"] = SymbolMap::Rect(0.015625f, 0.015625f, 0.25f, 0.375f);
	symbolMap->symbols["b"] = SymbolMap::Rect(0.296875f, 0.015625f, 0.265625f, 0.375f);
	symbolMap->symbols["c"] = SymbolMap::Rect(0.59375f, 0.015625f, 0.21875f, 0.375f);
	symbolMap->symbols["d"] = SymbolMap::Rect(0.015625f, 0.421875f, 0.28125f, 0.375f);
	//symbolMap->symbols["ft"] =
	
	// scene
	Pointer<Scene> scene = new Scene("scene");
	sceneFile->scenes += scene;
	
	// scene state
	scene->stateType = new StructType();
	scene->stateType->addMember("time", "float");
	scene->stateType->addMember("lastTime", "float");
	scene->stateType->addMember("deformer1.scale", "float");
	scene->stateType->addMember("deformer1.offset", "float3");
	scene->stateType->addMember("node1.color", "float3");
	scene->stateType->addMember("node1.texture", "Texture2D");
	scene->stateType->addMember("node1.textureSequence", "[4]Texture2D");
	scene->stateType->addMember("node1.textureIndex", "int");
	scene->stateType->addMember("node1.matrix", "float4x4");
	scene->stateType->addMember("text.font", "Texture2D");
	scene->stateType->addMember("text.text", "string");
	scene->stateType->addMember("text.symbols", "TextSymbols");

	// scene parameters that are accessible from the outside
	scene->attributes +=
		Scene::Attribute("time", P_FLOAT, "time"),
		Scene::Attribute("deformerOffset", P_FLOAT3, "deformer1.offset"),
		Scene::Attribute("deformerScale", P_FLOAT, "deformer1.scale"),
		Scene::Attribute("nodeColor", P_FLOAT3, "node1.color"),
		Scene::Attribute("nodeTexture", P_TEXTURE_2D, "node1.texture"),
		Scene::Attribute("nodeTextureSequence", P_TEXTURE_2D | (4 << 8), "node1.textureSequence"),
		Scene::Attribute("nodeTextureIndex", P_INT, "node1.textureIndex"),
		Scene::Attribute("text", P_STRING, "text.text");

	scene->textureBindings +=
		Scene::TextureBinding("digi-credit", P_TEXTURE_2D, "node1.texture"),
		Scene::TextureBinding("sequence.#", P_TEXTURE_2D | (4 << 8), "node1.textureSequence"),
		Scene::TextureBinding("Cyberbit", P_TEXTURE_2D, "text.font"),
		Scene::TextureBinding("symbolMap", P_SYMBOL_MAP, "text.symbols");

	// named buffers
	Pointer<Buffer> animXValues = new Buffer(BufferFormat(BufferFormat::X32, BufferFormat::FLOAT), 4);
	animXValues->setData(animXValuesData);
	Pointer<Buffer> animKeys = new Buffer(BufferFormat(BufferFormat::X32, BufferFormat::FLOAT), 10);
	animKeys->setData(animKeysData);
	scene->buffers +=
		NamedBuffer("animXValues", animXValues),
		NamedBuffer("animKeys", animKeys);

	// scene init state function
	scene->initStateCode =
		"state.node1.color = make_float3(1, 1, 1);\n"
		"state.node1.matrix = float4x4Identity();\n";
	
	// scene update function
	scene->updateCode =
		"state.deformer1.scale = evalBezierTrack(buffers.animXValues, buffers.animKeys, 4, mod(state.time, 3.0f));\n"
		"float3 center, size;\n"
		"macro_text(vector2(0.1f, 0.1f), vector3(0.4f, 0.0f, 1.0f), splat4(8.0f), vector2(0, 0), state.text.text, state.text.symbols, center, size);\n"
		"float timeStep = state.time - state.lastTime;\n"
		//"uniforms._deformer.scale = state.deformer1.scale;\n"
		//"uniforms._deformer.offset = state.deformer1.offset;\n"
		"uniforms._particleSystem.timeStep = timeStep;\n"
		"uniforms.e._particleSystem.numNewParticles = 1;\n"
		//"uniforms._shader1.color = state.node1.color;\n"
		//"uniforms._shader1.sampler = state.node1.texture;\n"
		//"uniforms._shader2.sampler = (state.node1.textureIndex >= 0 && state.node1.textureIndex < 4) ? state.node1.textureSequence[state.node1.textureIndex] : 0;\n"
		//"uniforms._shader3.sampler = state.text.font;\n"
		"transforms[0].matrix = state.node1.matrix;\n"
		"transforms[0].visible = true;\n"
		"transforms[1].matrix = matrix4x4Translate(vector3(0.7f, 0.0f, 0.0f)) * state.node1.matrix;\n" // particle shape
		"transforms[1].visible = true;\n"
		"transforms[2].matrix = matrix4x4Translate(vector3(-7.0f, 0.0f, 0.0f)) * state.node1.matrix;\n" // counter particle scale of 0.1
		"transforms[2].visible = true;\n"
		"transforms[3].matrix = matrix4x4Translate(vector3(-0.95f, 0.0f, 0.0f)) * state.node1.matrix;\n" // text
		"transforms[3].visible = true;\n"
		"boundingBoxes[0].center = 0.0f;\n"
		"boundingBoxes[0].size = 1.0f;\n"
		"state.lastTime = state.time;\n";
	
	Pointer<ConverterContext> context = new ConverterContext();
	Pointer<BufferConverter> converter = new BufferConverter(context);

	// vertex buffers
	Pointer<Buffer> positionBuffer1 = new Buffer(BufferFormat(BufferFormat::XYZ32, BufferFormat::FLOAT), 3);
	positionBuffer1->setData(positionData);
	Pointer<Buffer> positionBuffer2 = converter->convert(positionBuffer1, BufferFormat(BufferFormat::XYZ16, BufferFormat::NORM));

	Pointer<Buffer> texCoordBuffer1 = new Buffer(BufferFormat(BufferFormat::XY32, BufferFormat::FLOAT), 3);
	texCoordBuffer1->setData(texCoordData1);
	Pointer<Buffer> texCoordBuffer2 = new Buffer(BufferFormat(BufferFormat::XY32, BufferFormat::FLOAT), 3);
	texCoordBuffer2->setData(texCoordData2);

	// deformer for buffer 2
	Pointer<Deformer> deformer = new Deformer();
	deformer->name = "deformer";
	deformer->path = "_deformer";
	deformer->assignments += ".scale = state.deformer1.scale";
	deformer->assignments += ".offset = state.deformer1.offset";
	deformer->uniformType = Type::create("{scale float,offset float3}");
	deformer->inputType = Type::create("{position float3}");
	deformer->outputType = Type::create("{position float3}");
	deformer->code = "output.position = input.position * deformer.scale + deformer.offset;\n";

				
	// mesh (is a triangle)
	Pointer<ConstantMesh> constantMesh = new ConstantMesh(Mesh::DOUBLE_SIDED);
	constantMesh->indices.assign(boost::begin(indices), boost::end(indices));


	// shader1 (transparent texture multiplied with material color)
	Pointer<Shader> shader1 = new Shader();
	shader1->name = "shader1";
	shader1->path = "_shader1";
	shader1->assignments += ".color = state.node1.color";
	shader1->assignments += ".sampler = state.node1.texture";
	shader1->materialType = Type::create("{color float3,sampler Texture2D}");
	shader1->geometry = Shader::POSITION;
	shader1->inputFields += Shader::InputField("texCoord", "float2", Shader::TEXCOORD);
	
	shader1->code =
		"float4 t = sample(material.sampler, input.texCoord);\n"
		"float3 color = material.color * t.xyz * t.w;\n" // pre-multiply color with alpha
		"float3 alpha = t.w;\n";
		//"float4 col = vector4(material.color, 1.0f);\n"
		//"float4 tmp; if (material.color.x > 0.5f) if (col.y < 0.1f) tmp = col; else tmp = col.wzyx; else tmp = col * texture2D(material.sampler, input.texCoord); color = tmp;\n";
	
		
	// shader2 (opaque animated texture)
	Pointer<Shader> shader2 = new Shader();
	shader2->name = "shader2";
	shader2->path = "_shader2";
	shader2->assignments += ".sampler = (state.node1.textureIndex >= 0 && state.node1.textureIndex < 4) ? state.node1.textureSequence[state.node1.textureIndex] : 0";
	shader2->materialType = Type::create("{sampler Texture2D}");			
	shader2->geometry = Shader::POSITION;
	shader2->inputFields += Shader::InputField("texCoord", "float2", Shader::TEXCOORD);

	shader2->code =
		"float3 color = sample(material.sampler, input.texCoord).xyz;\n"
		"float3 alpha = 1.0f;\n";

	// shader3 (font texture)
	Pointer<Shader> shader3 = new Shader();
	shader3->name = "shader3";
	shader3->path = "_shader3";
	shader3->assignments += ".sampler = state.text.font";
	shader3->materialType = Type::create("{sampler Texture2D}");
	shader3->geometry = Shader::POSITION;
	shader3->inputFields += Shader::InputField("texCoord", "float2", Shader::TEXCOORD);

	shader3->code =
		"float3 t = sample(material.sampler, input.texCoord).xyz;\n"
		"float3 color = t;\n" // white * alpha
		"float3 alpha = t;\n";
	

	// shapes
	Pointer<ShapeInstancer> shape1 = new ShapeInstancer(0, shader1, constantMesh);
	shape1->setField("position", positionBuffer1, BufferVertexField::POSITION);
	shape1->setField("texCoord", texCoordBuffer1, BufferVertexField::TEXCOORD);

	Pointer<ShapeInstancer> shape1b = new ShapeInstancer(0, shader1, constantMesh);
	shape1b->setField("position", positionBuffer1, BufferVertexField::POSITION);
	shape1b->setField("texCoord", texCoordBuffer1, BufferVertexField::TEXCOORD);

	Pointer<ShapeInstancer> shape2 = new ShapeInstancer(0, deformer, shader2, constantMesh);
	shape2->setField("position", positionBuffer2, BufferVertexField::POSITION);
	shape2->setField("texCoord", texCoordBuffer2, BufferVertexField::TEXCOORD);

	
	// instancers for the two primitives
	Pointer<MultiInstancer> instancer1 = new MultiInstancer();
	scene->instances += NamedInstance(0, instancer1, "object1");
	instancer1->instancers += shape1;

	// this additional shape should lead to an extra render function
	instancer1->instancers += shape1b;
	
	Pointer<MultiInstancer> instancer2 = new MultiInstancer();
	scene->instances += NamedInstance(0, instancer2, "object2");
	instancer2->instancers += shape2;
	
	// this additional shape should not lead to an extra render function
	instancer2->instancers += shape2;


	// particle system
	Pointer<ParticleSystem> particleSystem = new ParticleSystem();
	particleSystem->name = "particleSystem";
	particleSystem->path = "_particleSystem";
	
	particleSystem->emitterUniformType = Type::create("{numNewParticles int}");
	particleSystem->maxNumParticles = 5;		
	
	particleSystem->uniformType = Type::create("{timeStep float}");
	particleSystem->particleType = Type::create("{position float3,size float,index int}");
	
	// particle create and update code
	particleSystem->createCode =
		"particle.position = vector3(-0.2f + 0.1f * float(id), 0.0f, 0.0f);\n"
		"particle.size = 0.1f;\n"
		"particle.index = 0;\n";
	particleSystem->updateCode =
		"particle.position.y += uniform.timeStep * 0.05f;\n";
	
	Pointer<Deformer> particleDeformer = new Deformer();
	particleDeformer->name = "particleDeformer";
	particleDeformer->path = "_particleDeformer";
	particleDeformer->uniformType = Type::create("{}");
	particleDeformer->inputType = Type::create("{}");
	particleDeformer->outputType = Type::create("{sprite float3}");
	particleDeformer->code = "output.sprite = vector3(0.1f, 0.2f, 0.1f);\n"; // sizeX, sizeY, twist

	// particle shape instancer (renders particle system as sprites)
	Pointer<ShapeInstancer> particleShapeInstancer = new ShapeInstancer(0, particleDeformer, shader2, particleSystem);
	particleShapeInstancer->setField("position", "position");
	scene->instances += NamedInstance(1, particleShapeInstancer, "particleShape");
	
	// particle instancer (instances the two triangles per particle)
	Pointer<ParticleInstancer> particleInstancer = new ParticleInstancer();
	particleInstancer->name = "instancer";
	particleInstancer->path = "_instancer";
	scene->instances += NamedInstance(2, particleInstancer, "particleInstancer");
	particleInstancer->particleSystem = particleSystem;
	particleInstancer->uniformType = Type::create("{}");
	particleInstancer->code =
		"particleMatrix = matrix4x4TranslateScale(particle.position, splat3(particle.size));\n";
	
	// add the two triangles to the particle instancer
	add(particleInstancer->instances) +=
		Instance(2, instancer1),
		Instance(2, instancer2);


	// text mesh
	Pointer<TextMesh> textMesh = new TextMesh();
	textMesh->name = "text";
	textMesh->maxNumSymbols = 16;
	//textMesh->symbols["a"] = TextMesh::Rect(0, 0, 0.5f, 0.5f);
	//textMesh->symbols["b"] = TextMesh::Rect(0, 0, 0.5f, 0.5f);
	//textMesh->symbols["c"] = TextMesh::Rect(0, 0, 0.5f, 0.5f);
	//textMesh->symbols["ft"] = TextMesh::Rect(0, 0, 0.5f, 0.5f);
	
	
	// text shape instancer (renders text mesh)
	Pointer<ShapeInstancer> textShapeInstancer = new ShapeInstancer(0, shader3, textMesh);
	textShapeInstancer->setField("position", "position");
	textShapeInstancer->setField("texCoord", "symbol");
	scene->instances += NamedInstance(3, textShapeInstancer, "textShape");


	// output path for test files
	fs::path outputPath = ".";

	// save
	{
		// may throw exception which is caught by cxxtest
		ObjectWriter w(outputPath / "test.digi");
		w.writeData("DIGI", 4);
		sceneFile->save(w);
		w.close();

		// TS_FAIL("failed to create file test.digi");
	}
	
	// load
	{
		ObjectReader r(outputPath / "test.digi");
		EXPECT_EQ(r.read<uint32_t>(), 0x49474944); // "DIGI"
		Pointer<SceneFile> sceneFile2 = SceneFile::load(r);
		r.close();
	}
	
	// compile for gl
	{
		SceneOptions options;
		options.api = GraphicsApi(GraphicsApi::GL, 210);
		options.mapBuffer = true;
		options.shadersInCode = true;
		options.deformersInShaders = false;
		options.dataMode = SceneOptions::KEEP;
		
		// add color and pick layer
		options.shaderOptions.resize(2);
		ShaderOptions& color = options.shaderOptions[0];
		ShaderOptions& pick = options.shaderOptions[1];
		color.layerName = color.outputCode = "color";
		color.layerIndex = 0;
		pick.layerName = pick.outputCode = "pick";
		pick.layerIndex = -1;
		
		CodeWriter w(outputPath / "Scenes.gl.h");
		DataWriter d(outputPath / "Scenes.gl.dat");

		SceneStatistics stats;
		generateSceneOpenGL(converter, scene, w, d, options, stats);

		w.close();
		d.close();
	}

	// compile for gl es
	{
		SceneOptions options;
		options.api = GraphicsApi(GraphicsApi::GLES, 200);
		options.mapBuffer = false;
		options.shadersInCode = true;
		options.deformersInShaders = true;
		options.dataMode = SceneOptions::REDUCE2;
		ShaderOptions& shaderOptions = options.shaderOptions[0];
		shaderOptions.positionPrecision = PRECISION_MEDIUM;
		shaderOptions.colorPrecision = PRECISION_LOW;
		shaderOptions.samplePrecision = PRECISION_HIGH;

		CodeWriter w(outputPath / "Scenes.es.h");
		DataWriter d(outputPath / "Scenes.es.dat");

		SceneStatistics stats;
		generateSceneOpenGL(converter, scene, w, d, options, stats);

		w.close();
		d.close();
	}

	// compile for webgl
	{
		SceneOptions options;
		options.api = GraphicsApi(GraphicsApi::WEBGL, 100);
		options.deformersInShaders = true;
		options.dataMode = SceneOptions::REDUCE2 | SceneOptions::COMPRESS | SceneOptions::USE_FLOAT;

		CodeWriter w(outputPath / "Scenes.web.js", Language::JS);
		DataWriter d(outputPath / "Scenes.web.dat");

		SceneStatistics stats;
		generateSceneWebGL(converter, scene, w, d, options, stats);

		w.close();
		d.close();
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

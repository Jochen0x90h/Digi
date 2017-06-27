#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Ascii.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/SetUtility.h>
#include <digi/System/Log.h>
#include <digi/Data/DataWriter.h>
#include <digi/SceneConvert/generateSceneWebGL.h>
#include <digi/SceneConvert/generateTextureWebGL.h>

#include "writeForWebGL.h"


namespace digi {

namespace
{
	struct TypeInfo
	{
		// 0: int, 1: float, 2: texture
		int baseType;
		
		// number of elements for vector and matrix types
		int numElements;
	};

	static const TypeInfo typeInfos[] =
	{
		{0, 1}, // T_BOOL
		{0, 2}, // T_BOOL2
		{0, 3}, // T_BOOL3
		{0, 4}, // T_BOOL4
		{0, 1}, // T_INT
		{0, 2}, // T_INT2
		{0, 3}, // T_INT3
		{0, 4}, // T_INT4
		{1, 1}, // T_FLOAT
		{1, 2}, // T_FLOAT2
		{1, 3}, // T_FLOAT3
		{1, 4}, // T_FLOAT4
		{1, 16}, // T_FLOAT4X4

		{1, 8}, // T_PROJECTION

		{2, 1}, // T_TEXTURE					
	};

	// ensures that output is valid JSON, e.g. converts functions to strings
	class JsonWriter : public CodeWriter
	{
	public:
		JsonWriter(const fs::path& path, Language language)
			: CodeWriter(path, language), scope(OBJECT), state(VALUE), level(0) {}
		
	protected:

		/// write current line (including comma and new lines of previous lines and indent of current line)
		virtual void writeLineInternal()
		{
			// add deferred comma
			line.insert(0, this->deferredComma, ',');
			
			if (this->state != FUNCTION)
			{
				// remove empty lines
				int numNewLines = std::min(this->numNewLines, 1);

				// add deferred new lines
				line.insert(this->deferredComma, numNewLines, '\n');

				// add indent
				line.insert(this->deferredComma + numNewLines, this->indent, '\t');
			}
			this->deferredComma = 0;
			this->numNewLines = 1;

			std::string::iterator it = this->line.begin();
			
			while (*it != 0)
			{
				// store start of white spaces and skip white spaces
				std::string::iterator spaces = it;
				while (Ascii::isSpace(*it))
					++it;

				if (state != FUNCTION)
				{
					switch (*it)
					{
					case '"':
						// quoted attribute or string value
						do {
							++it;
						} while (*it != '"');
						++it;
						break;
					case ':':
						// attribute : value
						++it;
						state = VALUE;
						break;
					case '[':
						// begin array
						this->scopes.push_back(this->scope);
						this->scope = ARRAY;
						this->state = VALUE;
						++it;
						break;
					case '{':
						// begin object
						this->scopes.push_back(this->scope);
						this->scope = OBJECT;
						this->state = ATTRIBUTE;
						++it;
						break;
					case ']':
					case '}':
						// end array or object
						this->scope = this->scopes.back();
						this->scopes.pop_back();
						++it;
						break;
					case ',':
						// next value
						state = this->scope == OBJECT ? ATTRIBUTE : VALUE;
						++it;
						break;
					case '/':
						// line comment. also erase spaces before comment
						it = this->line.erase(spaces, this->line.end());
						break;
					case 0:
						// end of line. erase trailing spaces
						this->line.erase(spaces, it);
						break;
					default:
						if (state == ATTRIBUTE)
						{
							// identifier attribute: json only supports quoted attribute
							it = this->line.insert(it, '"') + 1;
							while (Ascii::isIdentifier(*it))
								++it;
							it = this->line.insert(it, '"') + 1;
						}
						else if (this->line.compare(it - this->line.begin(), 8, "function") == 0)
						{
							// function
							it = this->line.insert(it, '"') + 1;
							it = this->line.insert(it, '(') + 9;
							this->state = FUNCTION;
						}
						else
						{
							// floating point number, true, false, null
							while (Ascii::isFloat(*it) || (*it >= 'a' && *it <= 'z'))
								++it;
						}
					}
				}
				else
				{
					// erase leading spaces
					it = this->line.erase(spaces, it);
				
					// function
					while (*it != 0)
					{
						if (*it == '{')
						{
							// start of scope
							++this->level;
						}
						else if (*it == '}')
						{
							// end of scope
							if (--this->level == 0)
							{
								// end of function
								++it;
								it = this->line.insert(it, ')') + 1;
								it = this->line.insert(it, '"') + 1;
								this->state = VALUE;
								break;
							}
						}
						else if (*it == '/' && *(it + 1) == '/')
						{
							// line comment
							it = this->line.erase(it, this->line.end());
							break;
						}
						else if (*it == '\\' && *(it + 1) == 0)
						{
							// line continuation
							it = this->line.erase(it, this->line.end());
							break;
						}
						else if (*it == '"' || *it == '\\')
						{
							// escape some characters
							it = this->line.insert(it, '\\') + 1;
						}
						++it;
					}
				}
			}
			
			//if (this->state != FUNCTION)
			//	line.append("\n");
			
			this->dev->write(this->line.data(), this->line.length());
		}
		
		enum Scope
		{
			ARRAY,
			OBJECT,
		};

		enum State
		{
			VALUE,
			ATTRIBUTE,
			FUNCTION,
		};
				
		Scope scope;
		std::vector<Scope> scopes;

		State state;
		
		// nesting level for function state
		int level;
	};

	
	void writeForWebGL(Pointer<SceneFile> sceneFile, CodeWriter& w, DataWriter& d, const fs::path& textureDir,
		const SceneOptions& options, SceneStatistics& stats)
	{
		// create converters
		Pointer<ConverterContext> converterContext = new ConverterContext();
		Pointer<ImageConverter> imageConverter = new ImageConverter(converterContext);
		Pointer<BufferConverter> bufferConverter = new BufferConverter(converterContext);
		
		// number of files (data file and textures) to load
		int numFiles = 1;
				
		// write textures
		w << "textures:\n";
		w.beginScope();
		std::set<std::string> textureNames;
		foreach (Pointer<Texture> texture, sceneFile->textures)
		{
			const std::string& name = texture->name;
			textureNames.insert(name);
			
			// begin namespace for texture
			w << '"' << name << "\":\n";
			w.beginScope();
			
			// generate code and data
			if (Pointer<TextureImage> textureImage = dynamicCast<TextureImage>(texture))
			{
				// texture
				if (options.texturesInFiles)
					numFiles += generateTextureWebGL(imageConverter, textureImage->image, w,
						textureDir, name, texture->type, options);
				else
					generateTextureWebGL(imageConverter, textureImage->image, w, d, texture->type, options);
			}
			else if (Pointer<TextureSequence> textureSequence = dynamicCast<TextureSequence>(texture))
			{
				// texture sequence
				if (options.texturesInFiles)
					numFiles += generateTextureSequenceWebGL(imageConverter, textureSequence->imageSequence, w,
						textureDir, name, texture->type, options);
				else
					generateTextureSequenceWebGL(imageConverter, textureSequence->imageSequence, w, d, texture->type,
						options);
			}			

			// end namespace for texture
			w.endScope(',');
			w.writeLine();
		}
		w.endScope(',');
		w.writeLine();
		
		// write scenes
		w << "scenes:\n";
		w.beginScope();
		foreach (Pointer<Scene> scene, sceneFile->scenes)
		{
			const std::string& name = scene->name;
			
			// sort all attributes since engine uses binary search
			scene->sortAttributes();

			// begin namespace for scene
			w << '"' << name << "\":\n";
			w.beginScope();
			
			// generate code and data
			Pointer<SplittedGlobalPrinter> statePrinter =
				generateSceneWebGL(bufferConverter, scene, w, d, options, stats);

			// attribute infos
			if (!scene->attributes.empty())
			{
					w << "attributes:\n";
					w.beginScope();
					if (statePrinter)
					{
						foreach (const Scene::Attribute& attribute, scene->attributes)
						{
							int typeIndex = attribute.type & 0xff;
							int baseType = typeInfos[typeIndex].baseType;
							int begin = max(statePrinter->getOffset('.' + attribute.path));
							int length = max(int(attribute.type >> 8), 1) * typeInfos[typeIndex].numElements;
							int end = begin + length;
							
							w << "\"" << attribute.name << "\": {t: " << baseType << ", b: " << begin << ", ";
							
							// int and float have end index, textures have length
							if (baseType < 2)
								w << "e: " << end;
							else
								w << "l: " << length;
							
							w << "}";
							w.writeLine(',');
						}
					}
					w.endScope(',');
					w.writeLine();
			}

			// texture bindings
			{
				w << "textureBindings:\n";
				w.beginArray();
				if (statePrinter)
				{
					foreach (const Scene::TextureBinding& textureBinding, scene->textureBindings)
					{
						// get texture name (stem of file name, e.g. "sequence.#")
						const std::string& textureName = textureBinding.textureName;
												
						// check if texture exists
						if (contains(textureNames, textureName))
						{											
							//const Scene::Attribute& attribute = *parameterIt;
						
							int begin = max(statePrinter->getOffset('.' + textureBinding.path));
						//! type check	
							// texture variable name, offset in istate
							w << "{n: \"" << textureName << "\", b: " << begin << "}";
							w.writeLine(',');
						}
					}
				}
				w.endArray(',');
				w.writeLine();
			}

			// attribute sets and clips

			// objects
			if (!scene->instances.empty())
			{
				// get and sort objects
				typedef std::pair<std::string, int> ObjectInfo;
				std::vector<ObjectInfo> objectInfos;
				foreach (const NamedInstance& instance, scene->instances)
				{
					int index = int(objectInfos.size());
					objectInfos += ObjectInfo(instance.name, index);
				}
				sort(objectInfos);

				w << "objects:\n";
				w.beginScope();
				foreach (const ObjectInfo& objectInfo, objectInfos)
				{
					w << "\"" << objectInfo.first << "\": " << objectInfo.second;
					w.writeLine(',');
				}
				w.endScope(',');
				w.writeLine();
			}

			// end namespace for scene
			w.endScope(',');
			w.writeLine();
		}
		w.endScope(',');
		w.writeLine();
		
		// async loading
		w << "numFiles: " << numFiles;
		w.writeLine(',');

		// check compatibility
		w << "check: function ()";
		w.beginScope();
		
		// check used number of textures and extensions
	// error code 1 reserved for webgl version
	// error code 2 reserved for inka3d version
		if (stats.numTextures.x > 0)
		{
			w << "if (gl.getParameter(gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS) < " << stats.numTextures.x << ")\n";
			w << "\treturn 4;\n";
			w << "if (gl.getExtension(\"OES_texture_float\") == null)\n";
			w << "\treturn 5;\n";

			//w << "if (gl.getExtension(\"OES_texture_float\") == null)\n";
			//w << "\talert(\"floating point textures are not supported\");\n";
			//w << "if (gl.getParameter(gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS) < " << stats.numTextures.x << ")\n";
			//w << "\talert(\"not enough vertex textures, need " << stats.numTextures.x << "\");\n";
		}
		if (stats.numTextures.y > 8)
		{
			w << "if (gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS) < " << stats.numTextures.y << ")\n";
			w << "\treturn 3;\n";

			//w << "if (gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS) < " << stats.numTextures.y << ")\n";
			//w << "\talert(\"not enough pixel textures, need " << stats.numTextures.y << "\");\n";
		}
		w << "return 0;\n";
		w.endScope();		
	}

} // anonymous namespace

void writeForWebGL(Pointer<SceneFile> sceneFile, const fs::path& outputPath, bool json,
	const SceneOptions& options, SceneStatistics& stats)
{
	// path of data file
	fs::path datPath = outputPath;
	datPath.replace_extension(".dat");
	
	// directory where texture files are stored
	fs::path textureDir = outputPath.parent_path();

	DataWriter d(datPath);
	if (!json)
	{
		// javascript
		CodeWriter w(outputPath, Language::JS);
		std::string name = outputPath.stem().string();
		w << "var " << name << " = function(engine)\n";
		w.beginScope();
		w << "'use strict';\n";
		w << "var s = ";
		w.beginScope();
		writeForWebGL(sceneFile, w, d, textureDir, options, stats);
		w.endScope(';');
		w << "return s;\n";
		w.endScope();
		w << "(inka3dEngine);\n";
		w.close();
	}
	else
	{
		// json
		JsonWriter w(outputPath, Language::JS);
		w.beginScope();
		writeForWebGL(sceneFile, w, d, textureDir, options, stats);
		w.endScope();
		w.close();
	}
	d.close();
}

} // namespace digi

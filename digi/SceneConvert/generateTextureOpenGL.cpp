#include <digi/Utility/IntUtility.h>

#include "generateTextureOpenGL.h"


namespace digi {

namespace
{
	typedef ImageFormat IF;

	// formats for opengl 3.3
	const TextureFormat textureFormatsGL[] =
	{
		{200, 299, IF::Y,    IF::X8,          IF::UNORM, "GL_LUMINANCE8",           "GL_LUMINANCE",       "GL_UNSIGNED_BYTE"                  },
		{200, 299, IF::Y,    IF::X16,         IF::UNORM, "GL_LUMINANCE16",          "GL_LUMINANCE",       "GL_UNSIGNED_SHORT"                 },

		{200, 299, IF::YA,   IF::XY8,         IF::UNORM, "GL_LUMINANCE8_ALPHA8",    "GL_LUMINANCE_ALPHA", "GL_UNSIGNED_BYTE"                  },
		{200, 299, IF::YA,   IF::XY16,        IF::UNORM, "GL_LUMINANCE16_ALPHA16",  "GL_LUMINANCE_ALPHA", "GL_UNSIGNED_SHORT"                 },

		{300, 999, IF::R,    IF::X8,          IF::UNORM, "GL_R8",                   "GL_RED",             "GL_UNSIGNED_BYTE"                  },
		{300, 999, IF::R,    IF::X16,         IF::UNORM, "GL_R16",                  "GL_RED",             "GL_UNSIGNED_SHORT"                 },
		{300, 999, IF::R,    IF::X16,         IF::FLOAT, "GL_R16F",                 "GL_RED",             "GL_HALF_FLOAT"                     },
		{300, 999, IF::R,    IF::X32,         IF::FLOAT, "GL_R32F",                 "GL_RED",             "GL_FLOAT"                          },

		{300, 999, IF::RG,   IF::XY8,         IF::UNORM, "GL_RG8",                  "GL_RG",              "GL_UNSIGNED_BYTE"                  },
		{300, 999, IF::RG,   IF::XY16,        IF::UNORM, "GL_RG16",                 "GL_RG",              "GL_UNSIGNED_SHORT"                 },
		{300, 999, IF::RG,   IF::XY16,        IF::FLOAT, "GL_RG16F",                "GL_RG",              "GL_HALF_FLOAT"                     },
		{300, 999, IF::RG,   IF::XY32,        IF::FLOAT, "GL_RG32F",                "GL_RG",              "GL_FLOAT"                          },

		{200, 999, IF::RGB,  IF::XYZ8,        IF::UNORM, "GL_RGB8",                 "GL_RGB",             "GL_UNSIGNED_BYTE"                  },
		{200, 999, IF::RGB,  IF::XYZ16,       IF::UNORM, "GL_RGB16",                "GL_RGB",             "GL_UNSIGNED_SHORT"                 },
		{300, 999, IF::RGB,  IF::XYZ16,       IF::FLOAT, "GL_RGB16F",               "GL_RGB",             "GL_HALF_FLOAT"                     },
		{300, 999, IF::RGB,  IF::XYZ32,       IF::FLOAT, "GL_RGB32F",               "GL_RGB",             "GL_FLOAT"                          },
		{200, 999, IF::RGB,  IF::X5Y6Z5,      IF::UNORM, "GL_RGB5",                 "GL_RGB" ,            "GL_UNSIGNED_SHORT_5_6_5_REV"       },
		{300, 999, IF::RGB,  IF::X11Y11Z10,   IF::UNORM, "GL_R11F_G11F_B10F",       "GL_RGB",             "GL_UNSIGNED_INT_10F_11F_11F_REV"   }, // http://www.opengl.org/registry/specs/EXT/packed_float.txt
		
		{200, 999, IF::RGBA, IF::XYZW8,       IF::UNORM, "GL_RGBA8",                "GL_RGBA",            "GL_UNSIGNED_BYTE"                  },
		{200, 999, IF::RGBA, IF::XYZW16,      IF::UNORM, "GL_RGBA16",               "GL_RGBA",            "GL_UNSIGNED_SHORT"                 },
		{300, 999, IF::RGBA, IF::XYZW16,      IF::FLOAT, "GL_RGBA16F",              "GL_RGBA",            "GL_HALF_FLOAT"                     },
		{300, 999, IF::RGBA, IF::XYZW32,      IF::FLOAT, "GL_RGBA32F",              "GL_RGBA",            "GL_FLOAT"                          },
		{200, 999, IF::RGBA, IF::X4Y4Z4W4,    IF::UNORM, "GL_RGBA4",                "GL_RGBA",            "GL_UNSIGNED_SHORT_4_4_4_4_REV"     },
		{200, 999, IF::RGBA, IF::X5Y5Z5W1,    IF::UNORM, "GL_RGB5_A1",              "GL_RGBA",            "GL_UNSIGNED_SHORT_1_5_5_5_REV"     },
		{200, 999, IF::RGBA, IF::X8Y8Z8W8,    IF::UNORM, "GL_RGBA8",                "GL_RGBA",            "GL_UNSIGNED_INT_8_8_8_8_REV"       },
		{200, 999, IF::RGBA, IF::X10Y10Z10W2, IF::UNORM, "GL_RGB10_A2",             "GL_RGBA",            "GL_UNSIGNED_INT_2_10_10_10_REV"    },
	};

	// formats for opengl es 2.0
	const TextureFormat textureFormatsGLES[] =
	{
		{200, 299, IF::Y,    IF::X8,          IF::UNORM, "GL_LUMINANCE",            "GL_LUMINANCE",       "GL_UNSIGNED_BYTE"                  },

		{200, 299, IF::A,    IF::X8,          IF::UNORM, "GL_ALPHA",                "GL_ALPHA",           "GL_UNSIGNED_BYTE"                  },

		{200, 299, IF::YA,   IF::XY8,         IF::UNORM, "GL_LUMINANCE_ALPHA",      "GL_LUMINANCE_ALPHA", "GL_UNSIGNED_BYTE"                  },

		{200, 999, IF::RGB,  IF::XYZ8,        IF::UNORM, "GL_RGB",                  "GL_RGB",             "GL_UNSIGNED_BYTE"                  },
		{200, 999, IF::RGB,  IF::X5Y6Z5,      IF::UNORM, "GL_RGB",                  "GL_RGB" ,            "GL_UNSIGNED_SHORT_5_6_5"           },
																								 
		{200, 999, IF::RGBA, IF::XYZW8,       IF::UNORM, "GL_RGBA",                 "GL_RGBA",            "GL_UNSIGNED_BYTE"                  },
		{200, 999, IF::RGBA, IF::X4Y4Z4W4,    IF::UNORM, "GL_RGBA",                 "GL_RGBA",            "GL_UNSIGNED_SHORT_4_4_4_4"         },
		{200, 999, IF::RGBA, IF::X5Y5Z5W1,    IF::UNORM, "GL_RGBA",                 "GL_RGBA",            "GL_UNSIGNED_SHORT_1_5_5_5"         },
	};

	void printDimension(CodeWriter& w, StringRef dim, int blockSize, int minNumBlocks)
	{
		// some compressed formats such as PVRTC require a minimum number of blocks (e.g. 2)
		if (minNumBlocks > 1)
			w << "max(";
		
		if (blockSize == 1)
		{
			// individual pixels of uncompressed formats
			w << dim;
		}
		else
		{
			// divide by block size and round up
			w << "((" << dim << " + " << (blockSize - 1) << ") / " << blockSize << ")";
		}
		
		if (minNumBlocks > 1)
			w << ", " << minNumBlocks << ")";
		w << " * ";
	}

	void setTexture(CodeWriter& w, const TextureDataInfo& textureInfo, Texture::Type type, const SceneOptions& options)
	{
		const TextureFormat& f = textureInfo.f;
		ImageFormat format = f.getFormat();
		size_t memorySize = format.getMemorySize();
		const ImageFormat::BlockInfo& blockInfo = format.getBlockInfo();
			
		const char* targets[] = {"2D", "3D", "CUBE_MAP"};
		const char* t = targets[textureInfo.type - Image::IMAGE];
		
		w << boost::format("glBindTexture(GL_TEXTURE_%1%, texture);\n") % t;

		if (textureInfo.type <= Image::VOLUME)
		{
			// image or volume
			bool isVolume = textureInfo.type == Image::VOLUME;

			// 2D and 3D textures
						
			// check for mipmaps (only if texture and compressed format like DXT, see TextureDataInfo.cpp)
			if (textureInfo.numMipmaps > 1)
			{
				// variables for dimensions
				w << "int width = " << textureInfo.size.x << ";\n";
				w << "int height = " << textureInfo.size.y << ";\n";
				if (isVolume)
					w << "int depth = " << textureInfo.size.z << ";\n";
				
				// variable for data pointer
				w << "ubyte* textureData = data + " << textureInfo.dataOffset << ";\n";

				// generate for loop for mipmaps
				w << "for (int i = 0; i < " << textureInfo.numMipmaps << "; ++i)\n";
				w.beginScope();

				// calc size of texture
				w << "size_t size = ";
				printDimension(w, "width", blockInfo.blockSize.x, blockInfo.minNumBlocks.x);
				printDimension(w, "height", blockInfo.blockSize.y, blockInfo.minNumBlocks.y);
				if (isVolume)
					printDimension(w, "depth", blockInfo.blockSize.z, blockInfo.minNumBlocks.z);
				w << memorySize << ";\n";

				// set texture data
				if (!format.isCompressed())
				{
					// normal texture
					w << boost::format("glTexImage%1%(GL_TEXTURE_%1%, i, ") % t << f.targetInternalFormat << ", ";
					w << "width, height, ";
					if (isVolume)
						w << "depth, ";
					w << "0, " << f.targetFormat << ", " << f.targetType << ", textureData);\n";
				}
				else
				{
					// compressed texture
					w << boost::format("glCompressedTexImage%1%(GL_TEXTURE_%1%, i, ") % t << f.targetInternalFormat << ", ";
					w << "width, height, ";
					if (isVolume)
						w << "depth, ";
					w << "0, size, textureData);\n";
				}
				
				// increment data pointer
				w << "textureData += size;\n";
						
				// reduce size
				int maxDimension = 1 << (textureInfo.numMipmaps - 1);
				if (textureInfo.size.x < maxDimension)
					w << "width = max(width >> 1, 1);\n";
				else 
					w << "width >>= 1;\n";

				if (textureInfo.size.y < maxDimension)
					w << "height = max(height >> 1, 1);\n";
				else
					w << "height >>= 1;\n";

				if (isVolume)
				{
					if (textureInfo.size.z < maxDimension)
						w << "depth = max(depth >> 1, 1);\n";
					else
						w << "depth >>= 1;\n";
				}			
				w.endScope();
				
				w << boost::format("glTexParameteri(GL_TEXTURE_%1%, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);\n") % t;
			}
			else
			{
				// no mipmaps provided

				// set texture data
				if (!format.isCompressed())
				{
					// normal texture
					w << boost::format("glTexImage%1%(GL_TEXTURE_%1%, 0, ") % t << f.targetInternalFormat << ", ";
					w << textureInfo.size.x << ", " << textureInfo.size.y << ", ";
					if (isVolume)
						w << textureInfo.size.z << ", ";
					w << "0, " << f.targetFormat << ", " << f.targetType << ", data + " << textureInfo.dataOffset << ");\n";
				}
				else
				{
					// compressed texture
					w << boost::format("glCompressedTexImage%1%(GL_TEXTURE_%1%, 0, ") % t << f.targetInternalFormat << ", ";
					w << textureInfo.size.x << ", " << textureInfo.size.y << ", ";
					if (isVolume)
						w << textureInfo.size.z << ", ";
					w << "0, " << Image::calcMemorySize(format, textureInfo.size) << ", data + " << textureInfo.dataOffset << ");\n";
				}

				if (type == Texture::TEXTURE)
				{
					// texture: generate mipmaps, opengl es 2.0: only power of two
					w << boost::format("glTexParameteri(GL_TEXTURE_%1%, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);\n") % t;
					w << boost::format("glGenerateMipmap(GL_TEXTURE_%1%);\n") % t;
				}
				else
				{
					// surface: no mipmaps
					w << boost::format("glTexParameteri(GL_TEXTURE_%1%, GL_TEXTURE_MIN_FILTER, GL_LINEAR);\n") % t;
					
					// if non power of two: use clamp
					if (!options.api.supportsTextureNonPowerOfTwo())
					{
						w << boost::format("glTexParameteri(GL_TEXTURE_%1%, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);\n") % t;
						w << boost::format("glTexParameteri(GL_TEXTURE_%1%, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);\n") % t;
					}
				}
			}
		}

		if (options.api.supportsTextureSwizzle())
		{
			// set texture swizzle
			int swizzle[] = {4, 4, 4, 5};
			bool isNeutral = true;
			for (int i = 0; i < 4; ++i)
			{
				switch (textureInfo.f.getFormat().getMapping(i))
				{
				case ImageFormat::R:
					swizzle[0] = i;
					isNeutral &= i == 0;
					break;
				case ImageFormat::G:
					swizzle[1] = i;
					isNeutral &= i == 1;
					break;
				case ImageFormat::B:
					swizzle[2] = i;
					isNeutral &= i == 2;
					break;
				case ImageFormat::Y:
					swizzle[0] = i;
					swizzle[1] = i;
					swizzle[2] = i;
					isNeutral = false;
					break;
				case ImageFormat::A:
					swizzle[3] = i;
					isNeutral &= i == 3;
					break;
				}
			}
			if (!isNeutral)
			{
				w << "GLint swizzle[] = {";
				for (int i = 0; i < 4; ++i)
				{
					const char* colors[] = {"GL_RED", "GL_GREEN", "GL_BLUE", "GL_ALPHA", "GL_ZERO", "GL_ONE"};
					if (i != 0)
						w << ", ";
					w << colors[swizzle[i]];
				}
				w << "};\n";
				w << boost::format("glTexParameteriv(GL_TEXTURE_%1%, GL_TEXTURE_SWIZZLE_RGBA, swizzle);\n") % t;
			}
		}

		// unbind texture
		w << boost::format("glBindTexture(GL_TEXTURE_%1%, 0);\n") % t;
	}

} // anonymous namespace

void generateTextureOpenGL(Pointer<ImageConverter> imageConverter, Pointer<Image> image, CodeWriter& w, DataWriter& d,
	 Texture::Type type, const SceneOptions& options)
{
	w.beginStruct("Global");
	w << "GLuint texture;\n";
	w.endStruct();
	w.writeLine();
	
	w << "void initGlobal(void* pGlobal, ubyte* data)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();				
		w << "glGenTextures(1, &global.texture);\n";
		w << "GLuint texture = global.texture;\n";

		TextureFormatRange textureFormats = options.api == GraphicsApi::GL
			? TextureFormatRange(textureFormatsGL) : TextureFormatRange(textureFormatsGLES);
		TextureDataInfo textureInfo;
		writeImage(d, image, imageConverter, textureFormats, textureInfo, type, options);
		setTexture(w, textureInfo, type, options);

		w.endScope();
	}
	w.writeLine();
	
	w << "void doneGlobal(void* pGlobal)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();				
		w << "glDeleteTextures(1, &global.texture);\n";
		w.endScope();
	}
	w.writeLine();
	
	w << "void copy(const void* pGlobal, void* pDestination)\n";
	{
		w.beginScope();
		w << "const Global& global = *(const Global*)pGlobal;\n";
		w << "*(GLuint*)pDestination = global.texture;\n";
		w.endScope();
	}	
}

void generateTextureSequenceOpenGL(Pointer<ImageConverter> imageConverter, const ImageSequence& textureSequence,
	CodeWriter& w, DataWriter& d, Texture::Type type, const SceneOptions& options)
{
	// start and length of texture sequence
	int start = textureSequence.begin()->first;
	int length = textureSequence.rbegin()->first + 1 - start;
	
	w.beginStruct("Global");
	w << "GLuint textures[" << length << "];\n";
	w.endStruct();
	w.writeLine();
	
	w << "void initGlobal(void* pGlobal, ubyte* data)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();				
		w << "glGenTextures(" << length << ", &global.textures[0]);\n";

		// set data of all textures
		typedef std::pair<const uint, Pointer<Image> > Pair;
		foreach (const Pair& p, textureSequence)
		{
			w.beginScope();
			w << "GLuint texture = global.textures[" << (p.first - start) << "];\n";

			TextureFormatRange textureFormats = options.api == GraphicsApi::GL
				? TextureFormatRange(textureFormatsGL) : TextureFormatRange(textureFormatsGLES);
			TextureDataInfo textureInfo;
			writeImage(d, p.second, imageConverter, textureFormats, textureInfo, type, options);
			setTexture(w, textureInfo, type, options);

			w.endScope();
		}
		w.endScope();
	}
	w.writeLine();
	
	w << "void doneGlobal(void* pGlobal)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();				
		w << "glDeleteTextures(" << length << ", &global.textures[0]);\n";
		w.endScope();
	}
	w.writeLine();
	
	w << "void copy(const void* pGlobal, void* pDestination)\n";
	{
		w.beginScope();
		w << "const Global& global = *(const Global*)pGlobal;\n";
		w << "GLuint* destination = (GLuint*)pDestination;\n";
		w << "for (int i = 0; i < " << length << "; ++i)\n";
		w << "\tdestination[i] = global.textures[i];\n";
		w.endScope();
	}
}

void generateSymbolMapOpenGL(Pointer<SymbolMap> symbolMap,
	CodeWriter& w, DataWriter& d, Texture::Type type, const SceneOptions& options)
{
	// build symbol data
	std::string strings;
	std::vector<int> indices;
	std::vector<float> symbols;
	foreach (SymbolMap::SymbolPair& p, symbolMap->symbols)
	{
		SymbolMap::Rect& rect = p.second;
		
		indices += int(strings.size());
		
		strings += p.first;
		strings += '\0';
		
		symbols += rect.x,
			rect.y,
			rect.w,
			rect.h;
	}
	
	// write symbol data
	int64_t indicesOffset = d.align(2);
	d.write<ushort>(indices.begin(), indices.size());
	int64_t stringsOffset = d.align(1);
	d.write<uint8_t>(strings.begin(), strings.size());
	int64_t symbolsOffset = d.align(4);
	d.write<float>(symbols.begin(), symbols.size());


	w.beginStruct("Global");
	
	// use predefined structure containing
	// float uv2posX
	// float uv2posY
	// int numSymbols
	// ushort* indices
	// ubyte* strings
	// float* rects
	w << "TextSymbols symbols;\n";

	w.endStruct();
	w.writeLine();
	
	w << "void initGlobal(void* pGlobal, ubyte* data)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();				

		w << "global.symbols.uv2posX = " << symbolMap->uv2posX << ";\n";
		w << "global.symbols.uv2posY = " << symbolMap->uv2posY << ";\n";
		w << "global.symbols.numSymbols = " << symbolMap->symbols.size() << ";\n";
		w << "global.symbols.indices = (ushort*)(data + " << indicesOffset << ");\n";
		w << "global.symbols.strings = data + " << stringsOffset << ";\n";
		w << "global.symbols.rects = (float*)(data + " << symbolsOffset << ");\n";

		w.endScope();
	}
	w.writeLine();
	
	w << "void doneGlobal(void* pGlobal)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();				

		w.endScope();
	}
	w.writeLine();
	
	w << "void copy(const void* pGlobal, void* pDestination)\n";
	{
		w.beginScope();
		w << "const Global& global = *(const Global*)pGlobal;\n";
		w << "TextSymbols* destination = (TextSymbols*)pDestination;\n";
		w << "*destination = global.symbols;\n";
		w.endScope();
	}
}

} // namespace digi

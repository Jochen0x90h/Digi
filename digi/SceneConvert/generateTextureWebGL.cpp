#include <digi/Utility/IntUtility.h>

#include "generateTextureWebGL.h"


namespace digi {

// textures as separate image files

namespace
{
	typedef ImageFormat IF;

	// available texture formats when storing textures into separate image files (.jpg, .png etc.)
	const TextureFormat textureFormatsFile[] =
	{
		// luminance images get loaded as rgb
		{100, 199, IF::Y,    IF::X8,          IF::UNORM, "gl.RGB",       "gl.RGB",      "gl.UNSIGNED_BYTE"},

		{100, 199, IF::YA,   IF::XY8,         IF::UNORM, "gl.RGBA",      "gl.RGBA",     "gl.UNSIGNED_BYTE"},

		{100, 199, IF::RGB,  IF::XYZ8,        IF::UNORM, "gl.RGB",       "gl.RGB",      "gl.UNSIGNED_BYTE"},
																				 
		{100, 199, IF::RGBA, IF::XYZW8,       IF::UNORM, "gl.RGBA",      "gl.RGBA",     "gl.UNSIGNED_BYTE"},
	};

	void onLoad(CodeWriter& w, Pointer<Image> image, Pointer<ImageConverter> imageConverter,
		TextureFormatRange textureFormats, const fs::path& dir, int index, Texture::Type type,
		const std::string& name, const SceneOptions& options)
	{
		TextureFileInfo textureInfo;
		
		// index for texture arrays
		std::string is;
		if (index != -1)
			is = Code() << "s[" << index << "]";

		// onload function
		{
			w.beginScope();
			writeImage(image, imageConverter, TextureFormatRange(textureFormatsFile), dir, name, textureInfo, type,
				options);
			//setTexture(w, textureInfo, index, type);
			
			const TextureFormat& f = textureInfo.f;

			w << "gl.bindTexture(gl.TEXTURE_2D, texture" << is << ");\n";
			w << "gl.texImage2D(gl.TEXTURE_2D, 0, " << f.targetInternalFormat << ", " << f.targetFormat << ", " << f.targetType
				<< ", this);\n";
			
			w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);\n";
			if (type == Texture::TEXTURE)
			{
				// texture: mipmaps, only power of two
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);\n";
				w << "gl.generateMipmap(gl.TEXTURE_2D);\n";
			}
			else
			{
				// surface: no mipmaps
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);\n";		
				
				// if non power of two: use clamp
				//if (!isPowerOfTwo(textureInfo.width) || !isPowerOfTwo(textureInfo.height))
				{
					w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);\n";
					w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);\n";
				}
			}
			w << "gl.bindTexture(gl.TEXTURE_2D, null);\n";
			
			w << "loaded();\n";
			w.endScope(';');
		}
		
		// image url
		/*
		std::string fileName = "'" + name;
		if (countSetBits(textureInfo.formatFlags) == 1)
		{
			fileName += ".";
			if (textureInfo.formatFlags & TextureOptions::PNG)
				fileName += "png";
			else if (textureInfo.formatFlags & TextureOptions::JPEG)
				fileName += "jpg";
			else if (textureInfo.formatFlags & TextureOptions::WEBP)
				fileName += "webp";
			fileName += "'";
		}
		else
		{
			if (textureInfo.jpeg2png)
				w << "if (ext == '.jpg') ext = '.png';\n";
			fileName += "' + ext";
		}*/
		size_t setCount = options.textureOptions.size();
		for (size_t index = 0; index < setCount; ++index)
		{
			const TextureOptions& textureOptions = options.textureOptions[index];
			
			// get output path
			std::string output = replace(textureOptions.output, '\\', '/');
			if (!output.empty() && !endsWith(output, "/"))
				output += '/';
			
			if (setCount > 1)
				w << "if (index == " << index << ")\n\t";
			w << "image" << is << ".src = url + '" << output
				<< name << textureInfo.extensions[index] << "';\n";
		}
	}
} // anonymous namespace

int generateTextureWebGL(Pointer<ImageConverter> imageConverter, Pointer<Image> image,
	CodeWriter& w, const fs::path& dir, const std::string& name, Texture::Type type, const SceneOptions& options)
{
	w << "initGlobal: function (global, url, index, loaded)\n";
	{
		w.beginScope();

		w << "var texture = global.texture = gl.createTexture();\n";
		w << "var image = global.image = new Image();\n";
		w << "image.onload = function ()\n";
		onLoad(w, image, imageConverter, TextureFormatRange(textureFormatsFile), dir, -1, type, name, options);
		w.endScope(',');
	}
	w.writeLine();
	
	w << "doneGlobal: function (global)\n";
	{
		w.beginScope();

		w << "gl.deleteTexture(global.texture);\n";

		w.endScope(',');
	}
	w.writeLine();
	
	w << "copy: function (global, ostate, b)\n";
	{
		w.beginScope();
		
		w << "ostate[b] = global.texture;\n";

		w.endScope(',');
	}
	
	return 1;
}

int generateTextureSequenceWebGL(Pointer<ImageConverter> imageConverter, const ImageSequence& textureSequence,
	CodeWriter& w, const fs::path& dir, const std::string& name, Texture::Type type, const SceneOptions& options)
{
	typedef std::pair<const uint, Pointer<Image> > Pair;
	//int numTextures = !textureSequence.empty() ? textureSequence.rbegin()->first + 1 : 0;
	//int numResources = 0;

	// start and length of texture sequence
	int start = textureSequence.begin()->first;
	int length = textureSequence.rbegin()->first + 1 - start;
	
	w << "initGlobal: function (global, url, index, loaded)\n";
	{
		w.beginScope();

		w << "var textures = global.textures = [];\n";
		w << "var images = global.images = [];\n";

		for (int i = 0; i < length; ++i)
		{
			ImageSequence::const_iterator it = textureSequence.find(start + i);
			if (it != textureSequence.end())
			{
				Pointer<Image> image = it->second;
				std::string n = replace(name, "#", toString(start + i));
				
				w << "textures[" << i << "] = gl.createTexture();\n";
				w << "images[" << i << "] = new Image();\n";
				w << "images[" << i << "].onload = function ()\n";
				onLoad(w, image, imageConverter, TextureFormatRange(textureFormatsFile), dir, i, type, n, options);

				//++numResources;
			}
			else
			{
				w << "textures[" << i << "] = null;\n";			
			}
		}
		
		w.endScope(',');
	}
	w.writeLine();
	
	w << "doneGlobal: function (global)\n";
	{
		w.beginScope();

		foreach (const Pair& p, textureSequence)
		{
			w << "gl.deleteTexture(global.textures[" << (p.first - start) << "]);\n";
		}

		w.endScope(',');
	}
	w.writeLine();
	
	w << "copy: function (global, ostate, b)\n";
	{
		w.beginScope();

		w << "var i;\n";
		w << "for (i = 0; i < " << length << "; ++i)\n";
		w << "\tostate[b + i] = global.textures[i];\n";

		w.endScope(',');
	}
	
	// return number of resources
	return textureSequence.size();
}

// textures embedded in data file

namespace
{
	typedef ImageFormat IF;

	// available texture formats when storing textures into data file
	const TextureFormat textureFormatsData[] =
	{
		// luminance images get loaded as rgb
		{100, 199, IF::Y,    IF::X8,       IF::UNORM, "gl.LUMINANCE",           "gl.LUMINANCE",       "gl.UNSIGNED_BYTE"},

		{100, 199, IF::YA,   IF::XY8,      IF::UNORM, "gl.LUMINANCE_ALPHA",     "gl.LUMINANCE_ALPHA", "gl.UNSIGNED_BYTE"},

		{100, 199, IF::RGB,  IF::XYZ8,     IF::UNORM, "gl.RGB",                 "gl.RGB",             "gl.UNSIGNED_BYTE"},
																								 
		{100, 199, IF::RGBA, IF::XYZW8,    IF::UNORM, "gl.RGBA",                "gl.RGBA",            "gl.UNSIGNED_BYTE"},
	};

	void setTexture(CodeWriter& w, GraphicsApi api, const TextureDataInfo& textureInfo, Texture::Type type)
	{
		const TextureFormat& f = textureInfo.f;
		
		// set texture data
		w << "gl.texImage2D(gl.TEXTURE_2D, 0, " << f.targetInternalFormat << ", "
			<< textureInfo.size.x << ", " << textureInfo.size.y << ", 0, "
			<< f.targetFormat << ", " << f.targetType << ", new Uint8Array(data, "
			<< textureInfo.dataOffset << ", " << textureInfo.dataSize << "));\n";
				
		w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);\n";
		if (type == Texture::TEXTURE)
		{
			// texture: mipmaps, only power of two
			w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);\n";
			w << "gl.generateMipmap(gl.TEXTURE_2D);\n";
		}
		else
		{
			// surface: no mipmaps
			w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);\n";		

			// if non power of two: use clamp
			if (!api.supportsTextureNonPowerOfTwo())
			{
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);\n";
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);\n";
			}
		}
	}
} // anonymous namespace

void generateTextureWebGL(Pointer<ImageConverter> imageConverter, Pointer<Image> image, CodeWriter& w, DataWriter& d,
	 Texture::Type type, const SceneOptions& options)
{
	w << "initGlobal: function (global, data)\n";
	{
		w.beginScope();

		TextureDataInfo textureInfo;
		writeImage(d, image, imageConverter, TextureFormatRange(textureFormatsData), textureInfo, type, options);
		w << "gl.bindTexture(gl.TEXTURE_2D, global.texture = gl.createTexture());\n";
		setTexture(w, options.api, textureInfo, type);
		w << "gl.bindTexture(gl.TEXTURE_2D, null);\n";

		w.endScope(',');
	}
	w.writeLine();
	
	w << "doneGlobal: function (global)\n";
	{
		w.beginScope();

		w << "gl.deleteTexture(global.texture);\n";

		w.endScope(',');
	}
	w.writeLine();
	
	w << "copy: function (global, ostate, b)\n";
	{
		w.beginScope();
		
		w << "ostate[b] = global.texture;\n";

		w.endScope(',');
	}	
}

void generateTextureSequenceWebGL(Pointer<ImageConverter> imageConverter, const ImageSequence& textureSequence,
	CodeWriter& w, DataWriter& d, Texture::Type type, const SceneOptions& options)
{
	typedef std::pair<const uint, Pointer<Image> > Pair;
	
	// start and length of texture sequence
	int start = textureSequence.begin()->first;
	int length = textureSequence.rbegin()->first + 1 - start;

	w << "initGlobal: function (global, data)\n";
	{
		w.beginScope();

		w << "var textures = global.textures = [];\n";

		for (uint i = 0; i < length; ++i)
		{
			ImageSequence::const_iterator it = textureSequence.find(start + i);
			if (it != textureSequence.end())
			{
				TextureDataInfo textureInfo;
				writeImage(d, it->second, imageConverter, TextureFormatRange(textureFormatsData), textureInfo, type, options);
				w << "gl.bindTexture(gl.TEXTURE_2D, textures[" << i << "] = gl.createTexture());\n";
				setTexture(w, options.api, textureInfo, type);
				w << "gl.bindTexture(gl.TEXTURE_2D, null);\n";
			}
			else
			{
				w << "textures[" << i << "] = null;\n";
			}
		}
		
		w.endScope(',');
	}
	w.writeLine();
	
	w << "doneGlobal: function (global)\n";
	{
		w.beginScope();

		foreach (const Pair& p, textureSequence)
		{
			w << "gl.deleteTexture(global.textures[" << (p.first - start) << "]);\n";
		}

		w.endScope(',');
	}
	w.writeLine();
	
	w << "copy: function (global, ostate, b)\n";
	{
		w.beginScope();

		w << "var i;\n";
		w << "for (i = 0; i < " << length << "; ++i)\n";
		w << "\tostate[b + i] = global.textures[i];\n";

		w.endScope(',');
	}
}

} // namespace digi

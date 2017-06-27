#ifndef digi_SceneConvert_SceneOptions_h
#define digi_SceneConvert_SceneOptions_h

#include <vector>

#include "ShaderOptions.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

struct TextureOptions
{
	enum TextureFormat
	{
		// use png
		PNG,
		
		// use jpeg or png if image has alpha channel or if png is smaller
		JPEG_PNG,
		
		// use webp
		WEBP,

		// use uncompressed format that is closest to input format
		RAW,

		// uses s3tc dxt1 or dxt5 dependent on input format
		S3TC,

		// uses pvrtc with 2 bit per pixel
		PVRTC2,

		// uses pvrtc with 4 bit per pixel
		PVRTC4,
	};

	// compression quality
	enum Quality
	{
		LOSSLESS,
		HIGH,
		MEDIUM,
		LOW
	};

	// texture compression format
	TextureFormat format;
	
	// compression quality
	Quality quality;
	
	// divide size by given number
	int divisor;

	// minimum texture size
	int preserveSize;

	// maximum texture size
	int maxSize;

	// texture output path if textures are in separate files
	std::string output;

	TextureOptions()
		: format(RAW), quality(MEDIUM), divisor(1), preserveSize(1), maxSize(8192) {}
};

struct GraphicsApi
{
	enum Type
	{
		// OpenGL (2.1, 3.2: vertex array objects, 3.3: texture swizzle)
		GL,

		// OpenGL ES (2.0)
		GLES,

		// WebGL (1.0)
		WEBGL
	};
	
	Type type;
	
	// version major, minor, patch as decimal number, e.g. 330 for 3.3.0
	int version;
	
	GraphicsApi()
		: type(GL), version(210) {}

	GraphicsApi(Type type, int version)
		: type(type), version(version) {}
	
	bool operator == (Type type) const
	{
		return this->type == type;
	}
	
	// supports non power of two for mipmap textures (non power of two works for surface textures if clamp is used)
	bool supportsTextureNonPowerOfTwo() const
	{
		return this->type == GL;
	}

	// supports texture swizzle
	bool supportsTextureSwizzle() const
	{
		return (this->type == GL && this->version >= 330)
			|| (this->type == GLES && this->version >= 300);
	}
};

/// options for scene conversion
struct SceneOptions
{
	// graphics api and version
	GraphicsApi api;

// texture options
/*
	enum TextureCompression
	{
		// use uncompressed format that is closest to input format
		NO_TEXTURE_COMPRESSION,

		// uses s3tc dxt1 or dxt5 dependent on input format
		S3TC,

		// uses pvrtc with 2 bit per pixel
		PVRTC2,

		// uses pvrtc with 4 bit per pixel
		PVRTC4,
	};

	// hardware texture compression mode
	TextureCompression compression;

	// divide size by given number
	int divisor;

	// minimum texture size
	int preserveSize;

	// maximum texture size
	int maxSize;
*/
	// texture options for output to image files
	std::vector<TextureOptions> textureOptions;

	// indicates if textures are placed in separate image files or data
	bool texturesInFiles;

// scene options
		
	// indicate if glMapBuffer can be used
	bool mapBuffer;
	
	// execute deformer in the vertex shader
	bool deformersInShaders;
	
	enum DataMode
	{
		// vertex buffers are used as they are
		KEEP = 0,
		
		// mask for reduce modes
		REDUCE_MASK = 3,
		
		// reduce resolution of positions and uvs to 24 bit and of normals, tangents and colors to 12 bit
		REDUCE1 = 1,

		// reduce resolution of positions and uvs to 16 bit and of normals, tangents and colors to 8 bit
		REDUCE2 = 2,

				
		// compress using differential lzss
		COMPRESS = 4,

		// reduce with level 1 and compress
		COMPRESS1 = REDUCE1 | COMPRESS,

		// reduce with level 2 and compress
		COMPRESS2 = REDUCE2 | COMPRESS,


		// extend vertex data to float on target side (IE11)
		USE_FLOAT = 8,
	};
	
	int dataMode;
	
	// pack positions and uvs to 16 bit, normals and tangents to 8 bit
	//bool packVertexBuffers;

	std::vector<ShaderOptions> shaderOptions;
	
	// indicates if shaders are placed in code or data
	bool shadersInCode;
		
	// render bounding boxes
	bool renderBoundingBoxes;

	SceneOptions()
		: texturesInFiles(false),
		mapBuffer(true), deformersInShaders(false), dataMode(KEEP),
		shaderOptions(1), shadersInCode(false), renderBoundingBoxes(false) {}
};

struct SceneStatistics
{
	// number of textures used in vertex and pixel shaders
	int2 numTextures;
	
	// number of render jobs that are needed
	int numRenderJobs;
	
	SceneStatistics()
		: numTextures(), numRenderJobs() {}
};

/// @}

} // namespace digi

#endif

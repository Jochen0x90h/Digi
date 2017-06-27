#ifndef digi_Scene_Texture_h
#define digi_Scene_Texture_h

#include <digi/Image/Image.h>

#include "ObjectReader.h"
#include "ObjectWriter.h"


namespace digi {

/// @addtogroup Scene
/// @{

/// base class for texture in a scene
class Texture : public Object
{
public:

	enum Type
	{
		// standard texture, may have mipmaps
		TEXTURE,
		
		// surface, no mipmaps and clamped. on opengl es 2.0 non-power-of-two is possible
		SURFACE
	};

	Texture()
		: type(TEXTURE) {}
	
	Texture(const std::string& name, Type type)
		: name(name), type(type) {}

	virtual ~Texture();
		
	static Pointer<Texture> load(ObjectReader& r);
	virtual void save(ObjectWriter& w) = 0;		
	
	// texture name
	std::string name;
	
	// texture type (texture or surface)
	Type type;
};

/// texture that is described by an image
class TextureImage : public Texture
{
public:
	
	TextureImage() {}
	
	TextureImage(const std::string& name, Type type, Pointer<Image> image)
		: Texture(name, type), image(image) {}

	virtual ~TextureImage();

	// image for the texture
	Pointer<Image> image;

	virtual void save(ObjectWriter& w);		
};


// image sequence
typedef std::map<uint, Pointer<Image> > ImageSequence;

/// texture that is described by a sequence of images
class TextureSequence : public Texture
{
public:

	TextureSequence() {}

	TextureSequence(const std::string& name, Type type)
		: Texture(name, type) {}

	virtual ~TextureSequence();

	// sequence of images for the texture sequence
	ImageSequence imageSequence;


	virtual void save(ObjectWriter& w);		
};


/// font symbol map
class SymbolMap : public Texture
{
public:

	SymbolMap() {}

	SymbolMap(const std::string& name)
		: Texture(name, TEXTURE), uv2posX(1.0f), uv2posY(1.0f) {}

	virtual ~SymbolMap();


	float uv2posX;
	float uv2posY;

	// rectangle for symbol on font texture (glyph or ligature)
	struct Rect
	{
		float x;
		float y;
		float w;
		float h;
		
		Rect()
			: x(), y(), w(), h()
		{}

		Rect(float x, float y, float w, float h)
			: x(x), y(y), w(w), h(h)
		{}
	};
	
	typedef std::map<std::string, Rect>::iterator SymbolIterator;
	typedef std::pair<const std::string, Rect> SymbolPair;
	std::map<std::string, Rect> symbols;

	virtual void save(ObjectWriter& w);		
};

/// @}

} // namespace digi

#endif

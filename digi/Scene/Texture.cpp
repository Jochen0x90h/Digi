#include "Texture.h"


namespace digi {

// Texture

Texture::~Texture()
{
}

// TextureImage
template <typename Serializer>
void serialize(Serializer& s, TextureImage& textureImage)
{
	s & textureImage.name;
	s & wrapUInt(textureImage.type);
	s & textureImage.image;
}

// TextureSequence
template <typename Serializer>
void serialize(Serializer& s, TextureSequence& textureSequence)
{
	s & textureSequence.name;
	s & wrapUInt(textureSequence.type);
	s & textureSequence.imageSequence;
}

// SymbolMap::Symbol
template <typename Serializer>
Serializer& operator &(Serializer& s, SymbolMap::Rect& rect)
{
	s & rect.x;
	s & rect.y;
	s & rect.w;
	s & rect.h;
	return s;
}

// SymbolMap
template <typename Serializer>
void serialize(Serializer& s, SymbolMap& symbolMap)
{
	s & symbolMap.name;
	s & symbolMap.uv2posX;
	s & symbolMap.uv2posY;
	s & symbolMap.symbols;
}


Pointer<Texture> Texture::load(ObjectReader& r)
{
	// read type id
	uint type;
	r & type;
	
	switch (type)
	{
	case 0:
		{
			TextureImage* textureImage = new TextureImage();
			VersionReader v(r);
			serialize(r, *textureImage);
			return textureImage;
		}
	case 1:
		{
			TextureSequence* textureSequence = new TextureSequence();
			VersionReader v(r);
			serialize(r, *textureSequence);
			return textureSequence;
		}
	case 2:
		{
			SymbolMap* symbolMap = new SymbolMap();
			VersionReader v(r);
			serialize(r, *symbolMap);
			return symbolMap;
		}
	}
	return null;
}


// TextureImage

TextureImage::~TextureImage()
{
}

void TextureImage::save(ObjectWriter& w)
{
	// write type id
	w & uint(0);

	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}


// TextureSequence

TextureSequence::~TextureSequence()
{
}

void TextureSequence::save(ObjectWriter& w)
{
	// write type id
	w & uint(1);

	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}


// SymbolMap

SymbolMap::~SymbolMap()
{
}

void SymbolMap::save(ObjectWriter& w)
{
	// write type id
	w & uint(2);

	// write version and members	
	VersionWriter v(w, 2);
	serialize(w, *this);
}

} // namespace digi

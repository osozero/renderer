#include "Texture.h"


TGAColor Texture::getColorFromTexCoord(Vec3f coords)
{
	return image.get(coords.x*width, coords.y*height);
}

Texture::~Texture()
{
}

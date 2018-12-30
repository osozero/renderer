#include "Texture.h"


TGAColor Texture::getColorFromTexCoord(Vec3f coords)
{
	return image.get(coords.x*width, coords.y*height);
}

Vec3f Texture::getNormalFromTexCoord(Vec3f coords)
{
	Vec2i uv(coords.x*width, coords.y*height);

	TGAColor color = image.get(uv.x, uv.y);

	Vec3f normal;

	for(int i=0;i<3;i++)
	{
		normal[2 - i] = (float)color[i] / 255.0f * 2.0f - 1.0f;
	}

	return normal;
}

float Texture::getSpecularFromTexCoords(Vec3f coords)
{
	return image.get(coords.x*width, coords.y*height)[0];
}

Texture::~Texture()
{
}

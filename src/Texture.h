#pragma once
#include "TGAImage.h"
#include "Model.h"

class Model;

class Texture
{
public:
	TGAImage image;
	Model model;
	int width;
	int height;

	Texture(TGAImage &_image, Model &_model)
		:image(_image),
		model(_model),
		width(image.getWidth()),
		height(image.getHeight())
	{}

	TGAColor getColorFromTexCoord(Vec3f coords);
	~Texture();
};


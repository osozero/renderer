#pragma once
#include "IShader.h"
#include "Model.h"
#include "Matrix.h"
#include "Texture.h"

class GouraudShader :
	public IShader
{
public:
	Vec3f varyingIntensity;
	Vec3f textureCoords[3];

	Model model;
	Texture texture;

	Matrix viewport;
	Matrix proj;
	Matrix modelview;

	Vec3f lightDirection;

	GouraudShader(Model &_model,Texture &_texture, Matrix &_viewport, Matrix &_proj, Matrix &_modelview, Vec3f &_lightDir)
		:model(_model),
		texture(_texture),
		viewport(_viewport),
		proj(_proj),
		modelview(_modelview),
		lightDirection(_lightDir)
	{
	}
	
	~GouraudShader(){};
	
	Vec4f vertex(int iface, int nthVert) override;
	
	bool fragment(Vec3f bar, TGAColor& color) override;
};


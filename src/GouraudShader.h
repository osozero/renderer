#pragma once
#include "IShader.h"
#include "Model.h"
#include "Matrix.h"

class GouraudShader :
	public IShader
{
public:
	Vec3f varyingIntensity;
	Model model;
	Matrix viewport;
	Matrix proj;
	Matrix modelview;

	Vec3f lightDirection;

	GouraudShader(Model &_model, Matrix &_viewport, Matrix &_proj, Matrix &_modelview, Vec3f &_lightDir)
		:model(_model),
		viewport(_viewport),
		proj(_proj),
		modelview(_modelview),
		lightDirection(_lightDir){}
	
	~GouraudShader(){};
	
	Vec4f vertex(int iface, int nthVert) override;
	
	bool fragment(Vec3f bar, TGAColor& color) override;
};


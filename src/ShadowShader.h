#pragma once
#include "IShader.h"
#include <glm/mat4x2.hpp>
#include "Model.h"
#include "Texture.h"

class ShadowShader :
	public IShader
{
public:

	glm::mat4 uniform_M;//projection*modelview;
	glm::mat4 uniform_MT;//uniform_M.inverse.transpose
	glm::mat4 uniform_MShadow;
	
	Vec3f varying_uv[3];//texture color mapping
	glm::mat3x3 varying_tri;

	Model model;
	Texture diffuseTexture;
	Texture normalTexture;
	Texture specularTexture;

	float *shadowbuffer;

	glm::vec4 lightDir;

	glm::mat4 viewport;
	glm::mat4 proj;
	glm::mat4 modelview;


	ShadowShader(Model &_model, Texture &_dTexture, Texture &_nTexture, Texture &_sTexture, glm::mat4 &_viewport, glm::mat4 &_proj, glm::mat4 &_modelview, glm::mat4 &M, glm::mat4 &MIT, glm::mat4 &MS,float * shadownuffer_,glm::vec4 lightDir_)
		:model(_model),
		diffuseTexture(_dTexture),
		normalTexture(_nTexture),
		specularTexture(_sTexture),
		viewport(_viewport),
		proj(_proj),
		modelview(_modelview),
		uniform_M(M),
		uniform_MT(MIT),
		uniform_MShadow(MS),
		shadowbuffer(shadownuffer_),
		lightDir(lightDir_)
	{};


	~ShadowShader(){};
	Vec4f vertex(int iface, int nthVert) override;
	bool fragment(Vec3f bar, TGAColor& color) override;
};


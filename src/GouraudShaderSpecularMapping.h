#pragma once
#include "IShader.h"
#include "Model.h"
#include "Texture.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class GouraudShaderSpecularMapping :
	public IShader
{
public:
	Vec3f textureCoords[3];


	glm::mat4 viewport;
	glm::mat4 proj;
	glm::mat4 modelview;

	glm::mat4 uniformProjectionModelview;// projection*modelview;
	glm::mat4 uniformProjModelviewInvertTranspose;// (projection*modelview) * invert_transpose;

	glm::vec4 lightDirection;

	Texture diffuseTexture;
	Texture normalTexture;
	Texture specularTexture;
	Model model;

	GouraudShaderSpecularMapping(Model &_model, Texture &_dTexture, Texture &_nTexture,Texture &_sTexture, glm::mat4 &_viewport, glm::mat4 &_proj, glm::mat4 &_modelview, glm::vec4 &_lightDir)
		:model(_model),
		diffuseTexture(_dTexture),
		normalTexture(_nTexture),
		specularTexture(_sTexture),
		viewport(_viewport),
		proj(_proj),
		modelview(_modelview),
		lightDirection(_lightDir)
	{
	};

	~GouraudShaderSpecularMapping(){};
	Vec4f vertex(int iface, int nthVert) override;
	bool fragment(Vec3f bar, TGAColor& color) override;
};


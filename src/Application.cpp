
#include <memory>
#include "TGAImage.h"
#include "Model.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "Matrix.h"
#include "GouraudShader.h"
#include "Renderer.h"
#include "GouraudShaderNormalMapping.h"


Vec3f eye(1,1,3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

Vec3f lightDirection(1, 1, 1);

const int width = 800;
const int height = 800;
const int depth = 255;


int main(int argc, char** argv)
{
	lightDirection.normalize();

	Renderer renderer;

	renderer.lookAt(eye, center,up);
	renderer.setProjection(-1.0f / (eye - center).length());
	renderer.setViewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

	std::string modelPath("resource/model/head.obj");
	std::string diffuseTexturePath("resource/texture/head_diffuse.tga");
	std::string normalTexturePath("resource/texture/head_nm.tga");

	auto model = std::make_unique<Model>(modelPath.c_str());

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

	TGAImage diffuseTexture;
	TGAImage normalTexture;

	if (!diffuseTexture.readTGAFile(diffuseTexturePath.c_str()))
	{
		std::cerr << "texture image could not be loaded: " << diffuseTexturePath << std::endl;

		return -1;
	}

	diffuseTexture.flipVertically();

	if (!normalTexture.readTGAFile(normalTexturePath.c_str()))
	{
		std::cerr << "texture image could not be loaded: " << normalTexturePath<< std::endl;

		return -1;
	}

	normalTexture.flipVertically();



	Texture diffuseTex(diffuseTexture, *model);
	Texture normTexture(normalTexture, *model);

	GouraudShader gouraudShader(*model, diffuseTex,renderer.viewport, renderer.projection, renderer.modelview, lightDirection);

	glm::vec4 lightDir(1, 1, 1, 1);
	GouraudShaderNormalMapping shaderNormalMapping(*model, diffuseTex,normTexture, renderer.vPort, renderer.proj, renderer.mView, lightDir);

	shaderNormalMapping.uniformProjectionModelview = renderer.mView*renderer.proj;
	shaderNormalMapping.uniformProjModelviewInvertTranspose = glm::transpose(glm::inverse(shaderNormalMapping.uniformProjectionModelview));



	for (int i = 0; i < model->numberOfFaces(); i++)
	{
		Vec4f screenCords[3];
		for (int j = 0; j < 3; j++)
		{
			//screenCords[j] = gouraudShader.vertex(i, j);
			screenCords[j] = shaderNormalMapping.vertex(i, j);
		}

		renderer.triangle(screenCords, shaderNormalMapping, image, zbuffer);
	}

	image.flipVertically();

	image.writeTGAFile("GouraudShaderNormalMappingTextured.tga");
	return 0;
}


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
#include "GouraudShaderSpecularMapping.h"
#include "PhongShaderTangentSpace.h"
#include "DepthShader.h"
#include "ShadowShader.h"


Vec3f eye(1,1,4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

Vec3f lightDirection(-1, -1, 1);

const int width = 800;
const int height = 800;
const int depth = 255;


int doShadowShading()
{
	float *zbufferFloat = new float[width*height];
	float *shadowBuffer = new float[width*height];

	glm::vec4 lightDir(1.0, 1.0, 0.0,0.0f);

	
	lightDir = glm::normalize(lightDir);
	
	for(int i=0;i<width*height;i++)
	{
		shadowBuffer[i] = zbufferFloat[i] = -std::numeric_limits<float>::max();
	}

	TGAImage depth(width, height, TGAImage::RGB);	

	Renderer renderer;


	//prepared for depth shader
	renderer.lookAt(Vec3f(lightDir.x,lightDir.y,lightDir.z), center, up);
	renderer.setViewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	renderer.setProjection(0);

	std::string modelPath("resource/model/diablo3_pose.obj");
	auto model = std::make_unique<Model>(modelPath.c_str());



	DepthShader depthShader(*model, renderer.vPort, renderer.proj, renderer.mView);

	Vec4f screenCoords[3];

	for(int i =0;i<model->numberOfFaces();i++)
	{
		for(int j=0;j<3;j++)
		{
			screenCoords[j] = depthShader.vertex(i, j);
		}

		renderer.triangle(screenCoords, depthShader, depth, shadowBuffer);	
	}
	
	depth.flipVertically();
	depth.writeTGAFile("depth.tga");

	glm::mat4 M = renderer.mView*renderer.proj*renderer.vPort;

	//preparing for frame buffer
	TGAImage image(width, height, TGAImage::RGB);


	std::string diffuseTexturePath("resource/texture/diablo3_pose_diffuse.tga");
	std::string normalTexturePath("resource/texture/diablo3_pose_nm.tga");
	std::string specularTexturePath("resource/texture/diablo3_pose_spec.tga");

	TGAImage diffuseTexture;
	TGAImage normalTexture;
	TGAImage specularTexture;

	if (!diffuseTexture.readTGAFile(diffuseTexturePath.c_str()))
	{
		std::cerr << "texture image could not be loaded: " << diffuseTexturePath << std::endl;

		return -1;
	}

	diffuseTexture.flipVertically();

	if (!normalTexture.readTGAFile(normalTexturePath.c_str()))
	{
		std::cerr << "texture image could not be loaded: " << normalTexturePath << std::endl;

		return -1;
	}

	normalTexture.flipVertically();

	if (!specularTexture.readTGAFile(specularTexturePath.c_str()))
	{
		std::cerr << "texture image could not be loaded: " << specularTexturePath << std::endl;

		return -1;
	}

	specularTexture.flipVertically();



	Texture diffuseTex(diffuseTexture, *model);
	Texture normTexture(normalTexture, *model);
	Texture specTexture(specularTexture, *model);

	renderer.lookAt(eye, center, up);
	renderer.setViewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	renderer.setProjection(-1.f / (eye - center).length());

	auto invertTranspose = glm::transpose(glm::inverse(renderer.proj*renderer.mView));


	auto mshader = M * glm::inverse(renderer.mView * renderer.proj *renderer.vPort);

	ShadowShader shadowShader(*model, diffuseTex, normTexture, specTexture, renderer.vPort, renderer.proj, renderer.mView, renderer.mView, invertTranspose, mshader, shadowBuffer, lightDir);

	Vec4f screenCoords2[3];

	for(int i=0;i<model->numberOfFaces();i++)
	{
		for(int j=0;j<3;j++)
		{
			screenCoords2[j] = shadowShader.vertex(i, j);
		}

		renderer.triangle(screenCoords2, shadowShader, image, zbufferFloat);
	}

	image.flipVertically();

	image.writeTGAFile("shadowMapping3.tga");

	delete[] zbufferFloat;
	delete[] shadowBuffer;

	return 0;




	

}

int main(int argc, char** argv)
{
	doShadowShading();

	return -1;

	float *zbufferFloat = new float[width*height];
	for (int i = width * height; i--; zbufferFloat[i] = -std::numeric_limits<float>::max());

	lightDirection.normalize();

	Renderer renderer;

	renderer.lookAt(eye, center,up);
	renderer.setProjection(-1.0f / (eye - center).length());
	renderer.setViewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

	std::string modelPath("resource/model/head.obj");
	std::string diffuseTexturePath("resource/texture/head_diffuse.tga");
	std::string normalTexturePath("resource/texture/head_nm_tangent.tga");
	std::string specularTexturePath("resource/texture/head_spec.tga");
	auto model = std::make_unique<Model>(modelPath.c_str());

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

	TGAImage diffuseTexture;
	TGAImage normalTexture;
	TGAImage specularTexture;

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

	if (!specularTexture.readTGAFile(specularTexturePath.c_str()))
	{
		std::cerr << "texture image could not be loaded: " << specularTexturePath<< std::endl;

		return -1;
	}

	specularTexture.flipVertically();



	Texture diffuseTex(diffuseTexture, *model);
	Texture normTexture(normalTexture, *model);
	Texture specTexture(specularTexture, *model);

	GouraudShader gouraudShader(*model, diffuseTex,renderer.viewport, renderer.projection, renderer.modelview, lightDirection);

	glm::vec4 lightDir(1, 3, 2, 1);
	GouraudShaderNormalMapping shaderNormalMapping(*model, diffuseTex,normTexture, renderer.vPort, renderer.proj, renderer.mView, lightDir);

	GouraudShaderSpecularMapping shaderSpecularMapping(*model, diffuseTex, normTexture,specTexture, renderer.vPort, renderer.proj, renderer.mView, lightDir);



	shaderNormalMapping.uniformProjectionModelview = renderer.mView*renderer.proj;
	shaderNormalMapping.uniformProjModelviewInvertTranspose = glm::transpose(glm::inverse(shaderNormalMapping.uniformProjectionModelview));


	shaderSpecularMapping.uniformProjectionModelview = renderer.mView*renderer.proj;
	shaderSpecularMapping.uniformProjModelviewInvertTranspose = glm::transpose(glm::inverse(shaderSpecularMapping.uniformProjectionModelview));




	lightDir = glm::normalize(renderer.proj*renderer.mView*lightDir);
	PhongShaderTangentSpace phongShaderTangentSpace(*model, diffuseTex, normTexture, specTexture, renderer.vPort, renderer.proj, renderer.mView, lightDir);

	



	for (int i = 0; i < model->numberOfFaces(); i++)
	{
		Vec4f screenCords[3];
		for (int j = 0; j < 3; j++)
		{
			//screenCords[j] = gouraudShader.vertex(i, j);
			//screenCords[j] = shaderNormalMapping.vertex(i, j);
			//screenCords[j] = shaderSpecularMapping.vertex(i, j);

			phongShaderTangentSpace.vertex(i, j);
			
		}

		renderer.triangle(phongShaderTangentSpace.varying_tri, phongShaderTangentSpace, image, zbufferFloat);
	}

	image.flipVertically();

	image.writeTGAFile("PhongShaderTangentSpace.tga");
	return 0;
}

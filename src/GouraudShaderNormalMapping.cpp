#include "GouraudShaderNormalMapping.h"
#include "glm/matrix.hpp"
#include <algorithm>

Vec4f GouraudShaderNormalMapping::vertex(int iface, int nthVert)
{
	Vec4f glVertex = Vec4f(model.vert(iface, nthVert), 1.0f);
	textureCoords[nthVert] = model.texture(iface, nthVert);

	glm::vec4 glVertexM4;

	for(int i=0;i<4;i++)
	{
		glVertexM4[i] = glVertex[i];
	}

	auto multiplyResult = glVertexM4 * modelview * proj*viewport;

	for(int i=0;i<4;i++)
	{
		glVertex[i] = multiplyResult[i];
	}

	return glVertex;
}

bool GouraudShaderNormalMapping::fragment(Vec3f bar, TGAColor& color)
{
	Vec3f interpolatedTexCoords(0, 0, 0);
	float temp;
	for (int i = 0; i < 3; i++)
	{
		temp = 0;
		for (int j = 0; j < 3; j++)
		{
			temp += textureCoords[j][i] * bar[j];
		}

		interpolatedTexCoords[i] = temp;
	}

	Vec3f normal = normalTexture.getNormalFromTexCoord(interpolatedTexCoords);

	glm::vec4 gV4;

	for(int i=0;i<3;i++)
	{
		gV4[i] = normal[i];
	}
	gV4[3] = 1.0;

	glm::vec3 translatedNormal = gV4*uniformProjModelviewInvertTranspose;

	glm::vec3 translatedLight = lightDirection*uniformProjectionModelview;

	translatedLight = glm::normalize(translatedLight);

	float intensity = std::max(0.0f, glm::dot(translatedNormal, translatedLight));

	color = diffuseTexture.getColorFromTexCoord(interpolatedTexCoords)*intensity;

	return false;


	return false;
}

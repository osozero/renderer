#include "GouraudShaderSpecularMapping.h"
#include "Matrix.h"
#include <algorithm>
#include <iostream>


Vec4f GouraudShaderSpecularMapping::vertex(int iface, int nthVert)
{
	Vec4f glVertex = Vec4f(model.vert(iface, nthVert),1.0f);

	textureCoords[nthVert] = model.texture(iface, nthVert);

	glm::vec4 glVertexV4;
	for(int i =0;i<4;i++)
	{
		glVertexV4[i] = glVertex[i];
	}

	auto multiplyResult = glVertexV4 * modelview * proj*viewport;

	for (int i = 0; i < 4; i++)
	{
		glVertex[i] = multiplyResult[i];
	}

	return glVertex;
}

bool GouraudShaderSpecularMapping::fragment(Vec3f bar, TGAColor& color)
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

	for (int i = 0; i < 3; i++)
	{
		gV4[i] = normal[i];
	}
	gV4[3] = 1.0;

	glm::vec3 n = glm::normalize(gV4 * uniformProjModelviewInvertTranspose);

	glm::vec3 l= glm::normalize(lightDirection * uniformProjectionModelview);

	glm::vec3 reflected = glm::normalize(n * (n*l*2.f) -l);

	float specular = specularTexture.getSpecularFromTexCoords(interpolatedTexCoords);

	float spec = pow(std::max(reflected.z, 0.0f), specular);

	float diffuse = std::max(0.0f, glm::dot(n,l));

	color = diffuseTexture.getColorFromTexCoord(interpolatedTexCoords);

	for(int i=0;i<3;i++)
	{
		color[i]= std::min<float>(5 + color[i] * (diffuse + 0.6*spec), 255.0f);
	}

	return false;
	
}

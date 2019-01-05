#include "PhongShaderTangentSpace.h"
#include "glm/gtc/matrix_access.hpp"
#include <algorithm>

Vec4f PhongShaderTangentSpace::vertex(int iface, int nthVert)
{
	textureCoords[nthVert] = model.texture(iface, nthVert);

	auto m = proj * modelview;
	auto n = modelview * proj;

	glm::vec4 v4Normal;
	Vec3f ve3Normal = model.normal(iface, nthVert);
	for(int i=0;i<3;i++)
	{
		v4Normal[i] = ve3Normal[i];
	}

	v4Normal[3] = 1.0;
	varying_nrm[nthVert] = glm::transpose(glm::inverse(proj*modelview)) * v4Normal;

	
	Vec3f v3vert = model.vert(iface, nthVert);

	glm::vec4 vert(v3vert.x, v3vert.y, v3vert.z, 1.0);


	glm::vec4 glVertexV4 = proj * modelview*vert;

	varying_tri = glm::column(varying_tri, nthVert, glVertexV4);
	ndc_tri = glm::column(ndc_tri, nthVert, glVertexV4 / glVertexV4[3]);

	Vec4f glVertex(glVertexV4.x, glVertexV4.y, glVertexV4.z, glVertexV4.w);

	return glVertex;
}

bool PhongShaderTangentSpace::fragment(Vec3f bar, TGAColor& color)
{
	Vec3f bn(0,0,0);
	float temp;

	for(int i =0;i<3;i++)
	{
		temp= 0.0f;
		for(int j=0;j<3;j++)
		{
			temp += varying_nrm[i][j] * bar[j];
		}

		bn[i] = temp;
	}

	Vec3f interpolatedTexCoords(0, 0, 0);
	
	for (int i = 0; i < 3; i++)
	{
		temp = 0;
		for (int j = 0; j < 3; j++)
		{
			temp += textureCoords[j][i] * bar[j];
		}

		interpolatedTexCoords[i] = temp;
	}

	glm::mat3x3 A;

	glm::column(A, 0, glm::column(ndc_tri, 1) - glm::column(ndc_tri, 0));

	glm::column(A, 1, glm::column(ndc_tri, 2) - glm::column(ndc_tri, 0));

	glm::column(A, 2,glm::vec3(bn.x,bn.y,bn.z));


	glm::mat3x3 AI = glm::inverse(A);

	glm::vec3 i = AI * glm::vec3(textureCoords[0][1] - textureCoords[0][0], textureCoords[0][2] - textureCoords[0][0], 0);

	glm::vec3 j = AI * glm::vec3(textureCoords[1][1] - textureCoords[1][0], textureCoords[1][2] - textureCoords[1][0], 0);

	glm::mat3x3 B;

	glm::column(B, 0, glm::normalize(i));

	glm::column(A, 1, glm::normalize(j));

	glm::column(A, 2, glm::vec3(bn.x, bn.y, bn.z));

	Vec3f nrml = normalTexture.getNormalFromTexCoord(interpolatedTexCoords);

	glm::vec3 n = glm::normalize(B* glm::vec3(nrml.x, nrml.y, nrml.z));

	float diff = std::max(0.0f, glm::dot(n, lightDirection));

	color = diffuseTexture.getColorFromTexCoord(interpolatedTexCoords)*diff;

	return false;
}

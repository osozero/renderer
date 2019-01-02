#include "PhongShaderTangentSpace.h"
#include "glm/gtc/matrix_access.hpp"

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

	glm::column(varying_tri, nthVert, glVertexV4);
	glm::column(varying_tri, nthVert, glVertexV4 / glVertexV4[3]);

	Vec4f glVertex(glVertexV4.x, glVertexV4.y, glVertexV4.z, glVertexV4.w);

	return glVertex;
}

bool PhongShaderTangentSpace::fragment(Vec3f bar, TGAColor& color)
{
	//todo
	return false;
}

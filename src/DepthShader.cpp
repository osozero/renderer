#include "DepthShader.h"
#include <glm/gtc/matrix_access.inl>


Vec4f DepthShader::vertex(int iface, int nthVert)
{
	Vec3f glVertex = model.vert(iface, nthVert);

	glm::vec4 v4glVertex(glVertex.x, glVertex.y, glVertex.z, 1.0);
	v4glVertex = v4glVertex* modelview*proj*viewport;

	varying_tri = glm::column(varying_tri, nthVert, v4glVertex / v4glVertex[3]);

	return Vec4f(v4glVertex.x, v4glVertex.y, v4glVertex.z, v4glVertex.w);
}

bool DepthShader::fragment(Vec3f bar, TGAColor& color)
{
	float depth = 255.0f;

	glm::vec3 p =glm::vec3(bar.x,bar.y,bar.z) * varying_tri;

	color = TGAColor(255, 255, 255)*(p.z / depth);

	return false;
}

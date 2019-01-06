#include "ShadowShader.h"
#include <glm/gtc/matrix_access.inl>
#include <algorithm>
#include <iostream>


Vec4f ShadowShader::vertex(int iface, int nthVert)
{
	varying_uv[nthVert] = model.texture(iface, nthVert);
	Vec4f glVertex = Vec4f(model.vert(iface, nthVert),1.0f);

	glm::vec4 glmV4glVertex(glVertex.x, glVertex.y, glVertex.z, glVertex.w);

	glmV4glVertex = glmV4glVertex * modelview*proj*viewport;

	varying_tri = glm::column(varying_tri, nthVert, glmV4glVertex / glmV4glVertex[3]);

	return Vec4f(glmV4glVertex.x, glmV4glVertex.y, glmV4glVertex.z, glmV4glVertex.w);
}

bool ShadowShader::fragment(Vec3f bar, TGAColor& color)
{
	int width = 800;
	glm::vec3 glmBar(bar.x, bar.y, bar.z);

	glm::vec4 shadowbufferPoint = glm::vec4(glmBar * varying_tri,1.0f)*uniform_MShadow;
	
	shadowbufferPoint = shadowbufferPoint / shadowbufferPoint[3];

	int index = int(shadowbufferPoint[0]) + shadowbufferPoint[1] * width;
	
	float shadow = 0.3 + 0.7 * (shadowbuffer[index] < shadowbufferPoint[2]+ 43.34);

	Vec3f interpolatedTexCoords(0, 0, 0);
	float temp;
	for (int i = 0; i < 3; i++)
	{
		temp = 0;
		for (int j = 0; j < 3; j++)
		{
			temp += varying_uv[j][i] * bar[j];
		}

		interpolatedTexCoords[i] = temp;
	}

	Vec3f normal = normalTexture.getNormalFromTexCoord(interpolatedTexCoords);

	glm::vec3 n = glm::normalize(glm::vec4(normal.x, normal.y, normal.z,1.0f) * uniform_MT);

	glm::vec3 l = glm::normalize(lightDir*uniform_M);

	glm::vec3 r = glm::normalize(n*(n*l*2.0f) - l);

	float spec = pow(std::max(r.z, 0.0f), specularTexture.getSpecularFromTexCoords(interpolatedTexCoords));

	float diff = std::max(0.0f, glm::dot(n, l));

	TGAColor c = diffuseTexture.getColorFromTexCoord(interpolatedTexCoords);

	for(int i=0;i<3;i++)
	{
		color[i] = std::min<float>(20 + c[i] * shadow*(1.2*diff + 0*spec), 255);
	}

	return false;


}

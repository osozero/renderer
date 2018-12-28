#include "GouraudShader.h"
#include <algorithm>
#include "Geometry.h"
#include "TGAImage.h"


Vec4f GouraudShader::vertex(int iface, int nthVert)
{
	Vec4f glVertex = Vec4f(model.vert(iface, nthVert),1.0f);
	Matrix glVertexM4(4, 1);

	for(int i=0;i<4;i++)
	{
		glVertexM4[i][0] = glVertex[i];
	}
	
	auto transResult = viewport * proj*modelview*glVertexM4;
	for(int i=0;i<4;i++)
	{
		glVertex[i] = transResult[i][0];
	}

	auto norm = model.normal(iface, nthVert);
	varyingIntensity[nthVert] = std::max(0.0f, model.normal(iface, nthVert).dot(lightDirection));

	return glVertex;
}

bool GouraudShader::fragment(Vec3f bar, TGAColor& color)
{
	float intensity = varyingIntensity.dot(bar);

	color = TGAColor(255, 255, 255)*intensity;

	return false;
}

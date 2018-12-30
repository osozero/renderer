#include "GouraudShader.h"
#include <algorithm>
#include "Geometry.h"
#include "TGAImage.h"


Vec4f GouraudShader::vertex(int iface, int nthVert)
{
	Vec4f glVertex = Vec4f(model.vert(iface, nthVert),1.0f);
	textureCoords[nthVert] = model.texture(iface, nthVert);
	
	Matrix glVertexM4(4, 1);

	for(int i=0;i<4;i++)
	{
		glVertexM4[i][0] = glVertex[i];
	}

	auto mresult = viewport * proj*modelview;
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

	Vec3f interpolatedTexCoords(0,0,0);
	float temp;
	for(int i=0;i<3;i++)
	{
		temp = 0;
		for(int j=0;j<3;j++)
		{
			temp += textureCoords[j][i] * bar[j];
		}

		interpolatedTexCoords[i] = temp;
	}

		
	color = texture.getColorFromTexCoord(interpolatedTexCoords);
	color = color*intensity;

	return false;
}

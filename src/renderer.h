#pragma once
#include "Matrix.h"

class renderer
{
public:
	Matrix  modelview;
	Matrix viewport;
	Matrix projection;
	
	renderer();

	void setViewport(int x, int y, int w, int h_i);

	//coeff = -1/c
	void setProjection(float coeff = 0);

	void lookAt(Vec3f eye, Vec3f center, Vec3f up);

	void triangle(Vec4f *pts,)


	~renderer();
};


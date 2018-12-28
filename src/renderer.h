#pragma once
#include "Matrix.h"
#include "IShader.h"
#include "TGAImage.h"


class TGAImage;

class renderer
	: public IShader
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

	void triangle(Vec4f *pts, TGAImage &image, TGAImage &zbuffer);

	Vec4f vertex(int iface, int nthVert) override;
	bool fragment(Vec3f bar, TGAColor& color) override;

	~renderer();
	
};


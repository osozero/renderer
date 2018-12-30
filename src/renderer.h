#pragma once
#include "Matrix.h"
#include "IShader.h"
#include "TGAImage.h"
#include <glm/mat4x4.hpp>


class TGAImage;

class Renderer
{
public:
	Matrix  modelview;
	Matrix viewport;
	Matrix projection;

	glm::mat4 mView;
	glm::mat4 vPort;
	glm::mat4 proj;

	Renderer();

	void setViewport(int x, int y, int w, int h_i);
	//coeff = -1/c
	void setProjection(float coeff = 0);

	void lookAt(Vec3f eye, Vec3f center, Vec3f up);

	void triangle(Vec4f *pts, IShader &shader,TGAImage &image, TGAImage &zbuffer);

	~Renderer();

private:
	Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
	
};


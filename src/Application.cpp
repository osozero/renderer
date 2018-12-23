
#include <memory>
#include <iostream>
#include "TGAImage.h"
#include "Model.h"
#include <algorithm>

const TGAColor white(255, 255, 255,255);
const TGAColor red(255, 0, 0, 255);
const TGAColor green(0, 255, 0, 255);

const int width = 800;
const int height = 800;


Vec3f barycentric2(Vec2i *pts, Vec2i P) {
	Vec3f u = cross(Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]), Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u[2]) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

Vec3f barycentric(Vec2i *pts, Vec2i P)
{


	float u;
	float v;
	float w;

	Vec3i AB(pts[1] - pts[0], 0.0);
	Vec3i AP(P - pts[0], 0.0);

	Vec3i BC(pts[2] - pts[1], 0.0);
	Vec3i BP(P - pts[1], 0.0);

	Vec3i CA(pts[0] - pts[2], 0.0);
	Vec3i CP(P - pts[2], 0.0);


	//for plane normal calculation
	Vec3i AC(pts[2] - pts[0], 0.0);

	Vec3i normal = cross(AB, AC);

	float denom = normal.dot(normal);

	Vec3i crossABAP = cross(AB, AP);

	if(normal.dot(crossABAP)<0)
	{
		return Vec3f(-1, -1, -1);
	}

	Vec3i crossBCBP = cross(BC, BP);

	if((u=normal.dot(crossBCBP))<0)
	{
		return Vec3f(-1, -1, -1);
	}

	Vec3i crossCACP = cross(CA, CP);

	if((v=normal.dot(crossCACP)<0))
	{
		return Vec3f(-1, -1, -1);
	}

	u /= denom;
	v /= denom;
	w = 1 - u - v;

	return Vec3f(w, u, v);
}

void line(Vec2i v1, Vec2i v2,TGAImage &image,const TGAColor &color)
{
	bool steep = false;

	if(std::abs(v1.x-v2.x)<std::abs(v1.y-v2.y))
	{
		std::swap(v1.x, v1.y);
		std::swap(v2.x, v2.y);
		steep = true;
	}

	if(v1.x>v2.x)
	{
		std::swap(v1.x, v2.x);
		std::swap(v1.y, v2.y);
	}

	int dx = v2.x - v1.x;
	int dy = v2.y - v1.y;

	int derror = std::abs(dy) * 2;
	int error = 0;

	int y = v1.y;

	for(int x=v1.x;x<=v2.x;x++)
	{
		if(steep)
		{
			image.set(y, x, color);
		}else
		{
			image.set(x, y, color);
		}
		error += derror;
		if(error>dx)
		{
			y += (v2.y > v1.y? 1 : -1);
			error -= dx*2;
		}
	}
}

void triangle(Vec2i *pts,TGAImage &image, const TGAColor &color)
{
	Vec2i bboxmin(image.getWidth() - 1, image.getHeight() - 1);

	Vec2i bboxmax(0, 0);

	Vec2i clamp(image.getWidth() - 1, image.getHeight() - 1);

	for(int i=0;i<3;i++)
	{
		for(int j=0;j<2;j++)
		{
			bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}

	Vec2i P;

	for(P.x=bboxmin.x;P.x<=bboxmax.x;P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(pts, P);

			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
			{
				continue;
			}

			image.set(P.x, P.y, color);
		}
	}
}

void loadModelAndDrawWireframe()
{
	std::string arg = "resource/model/head.obj";

	auto model = std::make_unique<Model>(Model(arg.c_str()));

	TGAImage image(width, height, TGAImage::RGB);

	for (int i = 0; i < model->numberOfFaces(); i++)
	{
		std::vector<int>  face = model->face(i);

		Vec2i scrCoords[3];
		for (int j = 0; j < 3; j++)
		{
			auto wordCoords = model->vert(face[j]);
			scrCoords[j] = Vec2i((wordCoords.x + 1)*width/2, (wordCoords.y + 1)*height/2);			
		}

		triangle(scrCoords, image, TGAColor(rand() % 255, rand() % 255, rand() % 255));
	}

	image.flipVertically();
	image.writeTGAFile("rasterized-random-colored-face.tga");
}

void rasterizeOneTriangle()
{
	TGAImage frame(200, 200, TGAImage::RGB);
	Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };


	triangle(pts, frame, TGAColor(255, 0, 0));

	frame.flipVertically();

	frame.writeTGAFile("rasterized2.tga");
}

int main(int argc, char**argv)
{
	loadModelAndDrawWireframe();
	
	return 0;
}

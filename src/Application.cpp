
#include <memory>
#include <iostream>
#include "TGAImage.h"
#include "Model.h"

const TGAColor white(255, 255, 255,255);
const TGAColor red(255, 0, 0, 255);

const int width = 800;
const int height = 800;



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

void triangle(Vec2i v1,Vec2i v2,Vec2i v3,TGAImage &image, const TGAColor &color)
{
	line(v1, v2, image, color);
	line(v2, v3, image, color);
	line(v3, v1, image, color);
}



void loadModelAndDrawWireframe()
{
	std::string arg = "resource/model/head.obj";

	auto model = std::make_unique<Model>(Model(arg.c_str()));

	TGAImage image(width, height, TGAImage::RGB);

	for (int i = 0; i < model->numberOfFaces(); i++)
	{
		std::vector<int>  face = model->face(i);

		for (int j = 0; j < 3; j++)
		{
			auto v0 = model->vert(face[j]);
			auto v1 = model->vert(face[(j + 1) % 3]);

			int x0 = (v0.x + 1.0)*width / 2.0;
			int y0 = (v0.y + 1.0)*height / 2.0;

			int x1 = (v1.x + 1.0)*width / 2.0;
			int y1 = (v1.y + 1.0)*height / 2.0;

			line(Vec2i(x0, y0), Vec2i(x1, y1), image, white);
		}
	}

	image.flipVertically();
	image.writeTGAFile("out.tga");
}
int main(int argc,char**argv)
{
	TGAImage image(width, height, TGAImage::RGB);

	triangle(Vec2i(10, 90), Vec2i(80, 90), Vec2i(200, 250), image, white);

	image.flipVertically();
	image.writeTGAFile("out2.tga");
	
	return 0;
}

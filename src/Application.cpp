
#include <memory>
#include <iostream>
#include "TGAImage.h"
#include "Model.h"

const TGAColor white(255, 255, 255,255);
const TGAColor red(255, 0, 0, 255);

const int width = 800;
const int height = 800;



void line(int x0,int y0,int x1,int y1,TGAImage &image,const TGAColor &color)
{
	bool steep = false;

	if(std::abs(x0-x1)<std::abs(y0-y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	if(x0>x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	int derror = std::abs(dy) * 2;
	int error = 0;

	int y = y0;

	for(int x=x0;x<=x1;x++)
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
			y += (y1 > y0 ? 1 : -1);
			error -= dx*2;
		}
	}
}


int main(int argc,char**argv)
{
	std::string arg;
	
	if(argc==2)
	{
		arg = argv[1];
	}else
	{
		arg = "resource/model/head.obj";
	}

	auto model = std::make_unique<Model>(Model(arg.c_str()));

	TGAImage image(width, height, TGAImage::RGB);

	for(int i=0;i<model->numberOfFaces();i++)
	{
		std::vector<int>  face = model->face(i);

		for(int j=0;j<3;j++)
		{
			auto v0 = model->vert(face[j]);
			auto v1 = model->vert(face[(j + 1) % 3]);

			int x0 = (v0.x + 1.0)*width / 2.0;
			int y0 = (v0.y + 1.0)*height / 2.0;

			int x1 = (v1.x + 1.0)*width / 2.0;
			int y1 = (v1.y + 1.0)*height / 2.0;

			line(x0, y0, x1, y1, image, white);
		}
	}

	image.flipVertically();
	image.writeTGAFile("out.tga");
	return 0;
}

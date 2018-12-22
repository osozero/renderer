
#include <memory>
#include <iostream>
#include "TGAImage.h"
#include "Model.h"

const TGAColor white(255, 255, 255,255);
const TGAColor red(255, 0, 0, 255);
const TGAColor green(0, 255, 0, 255);

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
	//sort v1,v2,v3 asc by their y values
	if(v1.y>v2.y)
	{
		std::swap(v1, v2);
	}

	if(v1.y>v3.y)
	{
		std::swap(v1, v3);
	}

	if(v2.y>v3.y)
	{
		std::swap(v2, v3);
	}


	int totalHeight = v3.y - v1.y;

	

	for(int i = v1.y; i<=v2.y;i++)
	{
		int segmentHeight = v2.y - v1.y + 1;
		float alpha = static_cast<float>(i - v1.y) / totalHeight;
		float beta = static_cast<float>(i - v1.y) / segmentHeight;

		Vec2i a = v1 + (v3 - v1)*alpha;
		Vec2i b = v1 + (v2 - v1)*beta;

		if(a.x>b.x)
		{
			std::swap(a, b);
		}

		for(int j = a.x;j<=b.x;j++)
		{
			image.set(j, i, color);
			//std::cout << j << "," << i << "--";
		}

		//std::cout<<std::endl;
	}

	for(int y=v2.y;y<=v3.y;y++)
	{
		int segmentHeight = v3.y - v2.y+1;
		float alpha = static_cast<float>(y - v1.y) / totalHeight;
		float beta = static_cast<float>(y - v2.y) / segmentHeight;

		Vec2i a = v1 + (v3 - v1)*alpha;
		Vec2i b = v2 + (v3 - v2)*beta;

		if(a.x>b.x)
		{
			std::swap(a, b);
		}

		for(int j = a.x;j<=b.x;j++)
		{
			image.set(j, y, color);
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

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 120 )};
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	triangle(t0[0], t0[1], t0[2], image, red);
	triangle(t1[0], t1[1], t1[2], image, white);
	triangle(t2[0], t2[1], t2[2], image, green);

	image.flipVertically();
	image.writeTGAFile("out3.tga");
	
	return 0;
}

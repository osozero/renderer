
#include <memory>
#include <iostream>
#include "TGAImage.h"

const TGAColor white(255, 255, 255,255);
const TGAColor red(255, 0, 0, 255);

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


int main()
{

	TGAImage image(100, 100, TGAImage::RGB);

	line(80, 40, 13, 20, image, red);

	line(13, 20, 85, 40, image, white);
	
	image.flipVertically();
	image.writeTGAFile("out.tga");
	return 0;
}

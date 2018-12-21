
#include <memory>
#include <iostream>
#include "TGAImage.h"

const TGAColor white(255, 255, 255,255);
const TGAColor red(255, 0, 0, 255);

void line(int x0,int y0,int x1,int y1,TGAImage &image,TGAColor color)
{
	for (float f = 0.0; f < 1.0; f += 0.01f)
	{
		int x = x0 * (1.0 - f) + x1 * f;
		int y = y0 * (1.0 - f) + y1 * f;

		image.set(x, y, color);
	}
}


int main()
{

	TGAImage image(100, 100, TGAImage::RGB);

	line(13, 20, 80, 40, image, white);
	image.flipVertically();
	image.writeTGAFile("out.tga");
	return 0;
}

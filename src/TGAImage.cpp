#include "TGAImage.h"
#include <algorithm>


bool TGAImage::loadRLEData(std::ifstream& in)
{
}

bool TGAImage::unloadRLEData(std::ofstream& out)
{
}

bool TGAImage::readTGAFile(const char* filename)
{

}

bool TGAImage::writeTGAFile(const char* filename)
{
}

TGAImage::TGAImage()
	:width(0),height(0),bytesPerPixel(0),data(nullptr)
{}

TGAImage::TGAImage(int w, int h, int bpp)
	:width(w),height(h),bytesPerPixel(bpp)
{
	auto size = width * height * bytesPerPixel;
	data = new unsigned char[size]{};
}

TGAImage::TGAImage(const TGAImage& t)
	:width(t.width),height(t.height),bytesPerPixel(t.bytesPerPixel)
{
	auto size = width * height * bytesPerPixel;
	data = new unsigned char[size];
	std::copy(t.data, t.data + size, data);
}


TGAImage::~TGAImage()
{
	delete[] data;
}

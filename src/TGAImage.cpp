#include "TGAImage.h"
#include <algorithm>
#include <iostream>


bool TGAImage::loadRLEData(std::ifstream& in)
{
	return false;
}

bool TGAImage::unloadRLEData(std::ofstream& out)
{
	return false;
}

bool TGAImage::flipVertically()
{
	return false;
}

bool TGAImage::flipHorizontally()
{
	return false;
}

bool TGAImage::readTGAFile(const char* filename)
{
	delete[] data;

	std::ifstream is(filename, std::ifstream::binary);
	if (!is)
	{
		is.close();
		std::cerr << "could not open file: " << filename << std::endl;
		return false;
	}

	TGAHeader header;

	is.read(reinterpret_cast<char*>(&header), sizeof(header));
	if (!is.good())
	{
		is.close();
		std::cerr << "error occured when reading the header of file: " << filename << std::endl;
		return false;
	}

	this->width = header.width;
	this->height = header.height;
	this->bytesPerPixel = header.bitsPerPixel >> 3;

	if ((this->width <= 0)||(this->height<=0)||(this->bytesPerPixel!=GRAYSCALE && this->bytesPerPixel!=RGB && this->bytesPerPixel!=RGBA))
	{
		is.close();
		std::cerr << "bpp, width or height value is bad\n";
		return false;
	}

	unsigned long nbytes = this->bytesPerPixel * this->width * this->height;
	data = new unsigned char[nbytes];

	if((header.dataTypeCode==3)||(header.dataTypeCode==2))
	{
		is.read(reinterpret_cast<char*>(data), nbytes);
		if(!is.good())
		{
			is.close();
			std::cerr << "an error occured when reading file data\n";
			return false;
		}
	}
	else if((header.dataTypeCode==10)||(header.dataTypeCode==11))
	{
		if(!loadRLEData(is))
		{
			is.close();
			std::cerr << "an error occured when reading file data\n";
			return false;
		}
	}
	else
	{
		is.close();
		std::cerr << "unknown file format: " << static_cast<int>(header.dataTypeCode) << std::endl;
		return false;
	}

	if(!(header.imageDescriptor & 0x20))
	{
		flipVertically();
	}

	if(header.imageDescriptor&0x10)
	{
		flipHorizontally();
	}

	is.close();
	std::cerr << width << "x" << height << "/" << bytesPerPixel * 8 << "\n";
	
	return true;
}

bool TGAImage::writeTGAFile(const char* filename)
{
	return false;
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

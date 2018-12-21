#include "TGAImage.h"
#include <algorithm>
#include <iostream>


bool TGAImage::loadRLEData(std::ifstream& is)
{
	const unsigned long pixelCount = this->width * this->height;
	unsigned long currentPixel = 0;
	unsigned long currentByte = 0;

	TGAColor colorBuf;

	do
	{
		unsigned char chunkHeader = 0;
		chunkHeader = is.get();

		if(!is.good())
		{
			std::cerr << "an error occured while reading the data\n";
			return false;
		}

		if(chunkHeader<128)
		{
			chunkHeader++;
			for(int i=0;i<chunkHeader;i++)
			{
				is.read(reinterpret_cast<char*>(colorBuf.raw), this->bytesPerPixel);
				if(!is.good())
				{
					std::cerr << "an error occured while reading the header\n";
					return false;
				}

				for(int j=0; j<this->bytesPerPixel;j++)
				{
					data[currentByte++] = colorBuf.raw[j];
				}

				currentPixel++;

				if(currentPixel>pixelCount)
				{
					std::cerr << "too many pixels read\n";
					return false;
				}
			}
		}
		else
		{
			chunkHeader -= 127;
			is.read(reinterpret_cast<char*>(colorBuf.raw), this->bytesPerPixel);
			if(!is.good())
			{
				std::cerr << "an error occured while reading the header\n";
				return false;
			}

			for(int i=0;i<chunkHeader;i++)
			{
				for(int j=0;j<this->bytesPerPixel;j++)
				{
					data[currentByte++] = colorBuf.raw[j];
				}
				currentPixel++;
				if(currentPixel>pixelCount)
				{
					std::cerr << "too many pixels read\n";
					return false;
				}
			}
		}
	} while (currentPixel < pixelCount);
	return true;
}

bool TGAImage::unloadRLEData(std::ofstream& os)
{
	unsigned char maxChunkLenght = 128;
	unsigned long numberOfPixels = this->width * this->height;
	unsigned long currentPixel = 0;

	while(currentPixel<numberOfPixels)
	{
		unsigned long chunkStart = currentPixel * this->bytesPerPixel;
		unsigned long currentByte = currentPixel * this->bytesPerPixel;
		unsigned char runLength = 1;

		bool raw = true;

		while(currentPixel+runLength<numberOfPixels && runLength<maxChunkLenght)
		{
			bool succEq = true;

			for(int i = 0;succEq&&i<this->bytesPerPixel;i++)
			{
				succEq = this->data[currentByte + i] == this->data[currentByte + this->bytesPerPixel];
			}

			currentByte += this->bytesPerPixel;

			if(runLength==1)
			{
				raw = !succEq;
			}

			if(raw&&succEq)
			{
				runLength--;
				break;
			}

			if(!raw && !succEq)
			{
				break;
			}

			runLength++;
		}

		currentPixel += runLength;
		os.put(raw ? runLength - 1 : runLength + 127);
		if(!os.good())
		{
			std::cerr << "could not write to tga file\n";
			return false;
		}

		os.write(reinterpret_cast<char*>(data + chunkStart), (raw ? runLength * bytesPerPixel : bytesPerPixel));
		if(!os.good())
		{
			std::cerr << "could not write to tga file\n";
			return false;
		}
	}

	return true;
}

bool TGAImage::flipVertically()
{
	if(data==nullptr)
	{
		return false;
	}

	unsigned long byteCountPerLine = this->width * this->bytesPerPixel;

	auto line = std::make_unique<unsigned char*>(new unsigned char[byteCountPerLine]);

	int half = height >> 1;

	for(int i = 0; i<half;i++)
	{
		unsigned long l1 = i * byteCountPerLine;
		unsigned long l2 = (this->height - 1 - i)*byteCountPerLine;

		memmove(static_cast<void*>(*line), static_cast<void*>(data + l1), byteCountPerLine);
		memmove(static_cast<void*>(data + l1), static_cast<void*>(data + l2), byteCountPerLine);
		memmove(static_cast<void*>(data + l2), static_cast<void*>(*line), byteCountPerLine);
	}

	return true;
}

bool TGAImage::flipHorizontally()
{
	if(this->data==nullptr)
	{
		return false;
	}

	int widthHalf = this->width >> 1;

	for(int i=0;i<widthHalf;i++)
	{
		for(int j=0;j<this->height;j++)
		{
			auto c1 = get(i, j);
			auto c2 = get(this->width - 1-i, j);

			set(i, j, c2);
			set(width - 1 - i, j, c1);
		}
	}

	return true;
}

TGAColor TGAImage::get(int x, int y) const
{
	if((this->data==nullptr)||(x<0)||(y<0)||(y>=this->height))
	{
		return TGAColor();
	}

	return TGAColor(this->data + (x + y * this->width)*this->bytesPerPixel, this->bytesPerPixel);
}

bool TGAImage::set(int x, int y, TGAColor c)
{
	if((this->data==nullptr)||(x<0)||(y<0)||(y>=this->height))
	{
		return false;
	}

	memcpy(this->data + (x + y * this->width)*this->bytesPerPixel, c.raw, this->bytesPerPixel);
	return true;
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

bool TGAImage::writeTGAFile(const char* filename, bool rle)
{
	unsigned char devAreaRef[4]{};
	unsigned char extAreaRef[4]{};
	unsigned char footer[18]= { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };

	std::ofstream os(filename, std::ofstream::binary);
	if(!os)
	{
		os.close();
		std::cerr << "could not open file: " << filename << std::endl;
		return false;
	}

	TGAHeader header;
	memset(static_cast<void*>(&header), 0, sizeof(header));

	header.bitsPerPixel = this->bytesPerPixel << 3;
	header.width = this->width;
	header.height = this->height;
	header.dataTypeCode = (this->bytesPerPixel == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));

	//origin is top-left
	header.imageDescriptor = 0x20;

	os.write(reinterpret_cast<char*>(&header), sizeof(header));
	if(!os.good())
	{
		os.close();
		std::cerr << "coul not write header to tga file\n";
		return false;
	}

	if(!rle)
	{

		os.write(reinterpret_cast<char*>(data), width*height*bytesPerPixel);
		if(!os.good())
		{
			os.close();
			std::cerr << "could not write raw data to tga file\n";
			return false;
		}
	}
	else
	{
		if(!unloadRLEData(os))
		{
			os.close();
			std::cerr << "could not unload rle data\n";
			return false;
		}
	}

	os.write(reinterpret_cast<char*>(devAreaRef), sizeof(devAreaRef));
	if(!os.good())
	{
		os.close();
		std::cerr << "could not write dev area ref to tga file\n";
		return false;
	}

	os.write(reinterpret_cast<char*>(extAreaRef), sizeof(extAreaRef));
	if(!os.good())
	{
		os.close();
		std::cerr<< "could not write ext area ref to tga file\n";
		return false;
	}

	os.write(reinterpret_cast<char*>(footer), sizeof(footer));
	if (!os.good())
	{
		os.close();
		std::cerr << "could not write footer data to tga file\n";
		return false;
	}

	os.close();
	return true;
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

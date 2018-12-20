#pragma once


#pragma pack(push,1)
#include <fstream>

struct TGAHeader
{
	char idLength;
	char colorMapType;
	char dataTypeCode;
	short colorMapOrigin;
	short colorMapLength;
	char colorMapDepth;
	short xOrigin;
	short yOrigin;
	short width;
	short height;
	char bitsPerPixel;
	char imageDescriptor;
};
#pragma pack(pop)

class TGAImage
{
protected:
	unsigned char *data;
	int width;
	int height;
	int bytesPerPixel;

	bool loadRLEData(std::ifstream &in);
	bool unloadRLEData(std::ofstream &out);

	bool flipVertically();
	bool flipHorizontally();

public:
	enum Format
	{
		GRAYSCALE=1,
		RGB=3,
		RGBA=4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage& t);

	bool readTGAFile(const char *filename);
	bool writeTGAFile(const char *filename);

	~TGAImage();
};


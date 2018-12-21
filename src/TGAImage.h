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

struct TGAColor
{
	union
	{
		struct
		{
			unsigned char r, g, b, a;
		};
		unsigned char raw[4];
		unsigned val;
	};

	int bytesPp;

	TGAColor()
		:val(0),bytesPp(1){}

	TGAColor(unsigned char R,unsigned char G, unsigned char B, unsigned char A)
		:r(R),g(G),b(B),a(A){}

	TGAColor(int v, int bpp)
		:val(v),bytesPp(bpp){}

	TGAColor(const TGAColor &tc)
		:val(tc.val),bytesPp(tc.bytesPp){}

	TGAColor(const unsigned char* p,int bpp)
		:val(0),bytesPp(bpp)
	{
		std::copy(p, p + bpp, raw);
	}

	TGAColor& operator=(const TGAColor &tc)
	{
		if(this!=&tc)
		{
			val = tc.val;
			bytesPp = tc.bytesPp;
		}

		return *this;
	}
};

class TGAImage
{
protected:
	unsigned char *data;
	int width;
	int height;
	int bytesPerPixel;

	bool loadRLEData(std::ifstream &is);
	bool unloadRLEData(std::ofstream &os);
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
	bool writeTGAFile(const char *filename,bool rle=true);


	bool flipVertically();
	bool flipHorizontally();

	TGAColor get(int x, int y) const;
	bool set(int x, int y, TGAColor c);

	~TGAImage();
};


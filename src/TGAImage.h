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
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};

	int bytesPp;

	TGAColor()
		:val(0),bytesPp(1){}

	TGAColor(unsigned char R,unsigned char G, unsigned char B, unsigned char A=255)
		:r(R),g(G),b(B),a(A){}

	TGAColor(int v, int bpp=3)
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

	TGAColor operator*(float f)
	{
		return TGAColor(this->r*f, this->g*f, this->b*f);
	}

	


	TGAColor operator+(const TGAColor &tc)
	{
		this->r += tc.r;
		this->g += tc.g;
		this->b += tc.b;

		return *this;
	}

	TGAColor operator+=(const TGAColor &tc)
	{
		this->r += tc.r;
		this->g += tc.g;
		this->b += tc.b;

		return *this;
	}

	TGAColor operator*=(const float f)
	{
		this->r *=f;
		this->g *= f;
		this->b *= f;

		return *this;
	}

	int operator[](int i) const
	{
		return raw[i];
	}

	unsigned char& operator[](int i)
	{
		return raw[i];
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

	inline int getWidth() const
	{
		return this->width;
	}

	inline int getHeight() const
	{
		return this->height;
	}

	~TGAImage();
};


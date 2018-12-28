#pragma once
#include "Geometry.h"

struct TGAColor;

class IShader
{
public:
	virtual ~IShader();
	virtual Vec4f vertex(int iface, int nthVert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

#pragma once
#include "Geometry.h"
#include <vector>

class Model
{
public:
	Model(const char *filename);
	
	int numberOfVerts();
	int numberOfFaces();

	Vec3f vert(int);
	std::vector<int> face(int index);
private:
	std::vector<Vec3f> _verts;
	std::vector<std::vector<int>> _faces;
};


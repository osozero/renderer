#pragma once
#include "Geometry.h"
#include <vector>
#include "TGAImage.h"

class Model
{
public:
	Model(const char *filename);
	
	int numberOfVerts();
	int numberOfFaces();
	int numberOfTextures();
	int numberOfNormals();

	Vec3f vert(int);

	Vec3f vert(int face, int nthVert);

	//keeps face and texture index: Vec2f(f,tIndex)
	std::vector<Vec3i> face(int index);
	Vec3f texture(int index);
	Vec3f texture(int face, int nthVert);

	Vec3f normal(int index);
	Vec3f normal(int face, int nthVert);
private:
	std::vector<Vec3f> _verts;
	std::vector<std::vector<Vec3i>> _faces;
	std::vector<Vec3f> _textures;
	std::vector<Vec3f> _normals;
};


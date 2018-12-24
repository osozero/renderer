#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char* filename)
	:_verts(),_faces(),_textures(),_normals()
{
	std::ifstream is;

	is.open(filename, std::ifstream::in);
	if(!is.good())
	{
		is.close();
		std::cerr << "model file could not opened: " << filename << std::endl;
		return;
	}

	
	std::string line;
	std::istringstream isstream;
	while(!is.eof())
	{
		std::getline(is,line);

		isstream.str(line);
		isstream.clear();

		char trash;

		if(!line.compare(0,2,"v "))
		{
			isstream >> trash;
			Vec3f v;

			for (auto& i : v.raw)
			{
				isstream >> i;
			}

			_verts.push_back(v);
		}else if(!line.compare(0,2,"f "))
		{
			std::vector<Vec3i>v;
			int iTrash, idx;

			Vec3i v2;
			isstream >> trash;

			

			while(isstream>>v2.x>>trash>>v2.y>>trash>>v2.z)
			{
				v2.x -= 1;
				v2.y -= 1;
				v2.z -= 1;
				v.push_back(v2);
			}

			_faces.push_back(v);
		}
		else if(!line.compare(0,2,"vt"))
		{
			Vec3f t;
			
			isstream >> trash;
			isstream >> trash;

			isstream >> t.x >> t.y >> t.z;

			_textures.push_back(t);

		}
		else if(!line.compare(0, 2, "vn"))
		{
			Vec3f n;

			isstream >> trash;
			isstream >> trash;

			isstream >> n.x >> n.y >> n.z;

			_normals.push_back(n);
		}
	}

	std::cout << "# v# " << _verts.size() << " f# " << _faces.size() << std::endl;
}

int Model::numberOfVerts()
{
	return static_cast<int>(_verts.size());
}

int Model::numberOfFaces()
{
	return static_cast<int>(_faces.size());
}

int Model::numberOfTextures()
{
	return static_cast<int>(_textures.size());
}

int Model::numberOfNormals()
{
	return static_cast<int>(_normals.size());
}

Vec3f Model::vert(int i)
{
	return _verts.at(i);
}

std::vector<Vec3i> Model::face(int index)
{
	return _faces.at(index);
}

Vec3f Model::texture(int index)
{
	return _textures.at(index);
}

Vec3f Model::normal(int index)
{
	return _normals.at(index);
}

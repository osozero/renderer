#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char* filename)
	:_verts(),_faces()
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
			std::vector<int>f;
			int iTrash, idx;

			isstream >> trash;

			while(isstream>>idx>>trash>>iTrash>>trash>>iTrash)
			{
				idx--;
				f.push_back(idx);
			}

			_faces.push_back(f);
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

Vec3f Model::vert(int i)
{
	return _verts.at(i);
}

std::vector<int> Model::face(int index)
{
	return _faces.at(index);
}

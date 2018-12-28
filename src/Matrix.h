#pragma once
#include <vector>
#include "Geometry.h"

constexpr int DEFAULT_ALLOC = 4;

class Matrix
{
private:
	std::vector < std::vector<float>> m;
	int rows;
	int cols;
public:
	Matrix(int r=DEFAULT_ALLOC,int c=DEFAULT_ALLOC);


	 Matrix(Vec3f v)
		 :m(std::vector<std::vector<float>>(4, std::vector<float>(1)))
	,rows(4), cols(1)
	{

		 m[0].at(0) = v.x;
		m[1].at(0)=v.y;
		m[2].at(0)=(v.z);
		m[3].at(0)=(1.0);
	}



	inline int getRowSize() const
	{
		return rows;
	}

	inline int getColumnSize() const
	{
		return cols;
	}

	static Matrix identity(int dimensions);

	std::vector<float>& operator[](const int i);
	std::vector<float> operator[](const int i) const;

	Matrix operator*(const Matrix &m);

	Matrix transpose();

	Matrix inverse();

	friend std::ostream& operator<<(std::ostream &os, Matrix &m);
};

	

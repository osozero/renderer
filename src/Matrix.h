#pragma once
#include <vector>

constexpr int DEFAULT_ALLOC = 4;

class Matrix
{
private:
	std::vector < std::vector<float>> m;
	int rows;
	int cols;
public:
	Matrix(int r=DEFAULT_ALLOC,int c=DEFAULT_ALLOC);

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


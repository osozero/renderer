#include "Matrix.h"
#include <iostream>

Matrix::Matrix(int r, int c)
	:m(std::vector<std::vector<float>>(r,std::vector<float>(c,0.0f))),
	rows(r),
	cols(c)
{}

Matrix Matrix::identity(int dimensions)
{
	Matrix i(dimensions, dimensions);

	for(int j=0;j<dimensions;j++)
	{
		i[j][j] = 1;
	}

	return i;
}

std::vector<float>& Matrix::operator[](const int i)
{
	return m[i];
}

std::vector<float> Matrix::operator[](const int i) const
{
	return m[i];
}

Matrix Matrix::operator*(const Matrix& m)
{
	if(this->cols!=m.rows)
	{
		return *this;
	}

	Matrix r(this->rows, m.cols);
	float sum;
	for(int i=0;i<this->rows;i++)
	{
		sum = 0.0f;
		for(int j=0;j<m.cols;j++)
		{
			for(auto c=0;c<((*this)[i].size());c++)
			{
				sum += (*this)[i][c] * m[j][c];
			}

			r[i][j] = sum;
		}
	}

	return r;
}

Matrix Matrix::transpose()
{
	Matrix r(this->cols, this->rows);
;
	for(int i=0;i<this->rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			r[j][i] = m[i][j];
		}
	}
	return r;
}

Matrix Matrix::inverse()
{
	Matrix result(rows, cols * 2);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			result[i][j] = m[i][j];
	for (int i = 0; i < rows; i++)
		result[i][i + cols] = 1;
	// first pass
	for (int i = 0; i < rows - 1; i++) {
		// normalize the first row
		for (int j = result.cols - 1; j >= 0; j--)
			result[i][j] /= result[i][i];
		for (int k = i + 1; k < rows; k++) {
			float coeff = result[k][i];
			for (int j = 0; j < result.cols; j++) {
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}
	// normalize the last row
	for (int j = result.cols - 1; j >= rows - 1; j--)
		result[rows - 1][j] /= result[rows - 1][rows - 1];
	// second pass
	for (int i = rows - 1; i > 0; i--) {
		for (int k = i - 1; k >= 0; k--) {
			float coeff = result[k][i];
			for (int j = 0; j < result.cols; j++) {
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}
	// cut the identity matrix back
	Matrix truncate(rows, cols);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			truncate[i][j] = result[i][j + cols];
	return truncate;
}


std::ostream& operator<<(std::ostream& os, Matrix& m)
{
	for (int i = 0; i < m.getRowSize(); i++) {
		for (int j = 0; j < m.getColumnSize(); j++) {
			os << m[i][j];
			if (j < m.getColumnSize() - 1) os << "\t";
		}
		os << "\n";
	}
	return os;
}

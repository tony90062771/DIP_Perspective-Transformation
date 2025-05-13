#include <iostream>
#include <iomanip>
#include <cmath>
#include "matrix.h"

using namespace std;


void error()
{ 
	cout << "GG" << endl;
}

//將指定行的元素除以該行首個元素，使得該行首個元素變為 1
void first(int i, int m, double **arr, double **arr_1)
{ 
	double tmp;
	int j;
	tmp = arr[i][i];
	for (j = 0; j < m; j++)
	{
		arr[i][j] = arr[i][j] / tmp;
		arr_1[i][j] = arr_1[i][j] / tmp;
	}
	return;
}

// 將指定行以外的元素調整為 0，使得該行首個元素為 1 時其他行首個元素為 0
void zero(int i, int m, double **arr, double **arr_1)
{
	int j, k;
	double tmp;
	for (j = 0; j < m; j++)
	{
		if (j == i)
			continue;
		tmp = -1 * arr[j][i];
		for (k = 0; k < m; k++)
		{
			arr[j][k] = arr[j][k] + (tmp * arr[i][k]);
			arr_1[j][k] = arr_1[j][k] + (tmp * arr_1[i][k]);
		}
	}
	return;
}

// 進行高斯消去法
void gauss_jordan(int m, double **arr, double **arr_1)
{ 
	int i, j;
	double tmp;
	for (i = 0; i < m; i++)
	{
		if (arr[i][i] == 0)
		{
			if (i == (m - 1))
			{
				break;
			}
			else
			{
				for (j = 0; j < m; j++)
				{
					arr[i][j] = arr[i][j] + arr[i + 1][j];
					arr_1[i][j] = arr_1[i][j] + arr_1[i + 1][j];
				}
				first(i, m, arr, arr_1);
				zero(i, m, arr, arr_1);
			}
		}
		else
		{
			first(i, m, arr, arr_1);
			zero(i, m, arr, arr_1);
		}
	}
	return;
}

//計算逆矩陣
void matrix_inverse(int matrix_size, double **matrix_name, double **out_matrix)
{
	int i, j;
	gauss_jordan(matrix_size, matrix_name, out_matrix);
	if (matrix_name[matrix_size - 1][matrix_size - 1] == 0)
	{
		error();
		return;
	}
	//cout << "------------OK-----------" << endl;
	for (i = 0; i < matrix_size; i++)
	{
		for (j = 0; j < matrix_size; j++)
		{
			if (out_matrix[i][j] == -0) 
				out_matrix[i][j] = fabs(out_matrix[i][j]);
			//cout << fixed << setprecision(3) << out_matrix[i][j] << "\t";
		}
		cout << endl;
	}
	return;
}


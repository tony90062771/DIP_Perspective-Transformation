#include <iostream>
#include <cmath>
#include <stdlib.h>
#include "matrix.h"
#include "bmp.h"

using namespace std;

int R[MaxBMPSizeX][MaxBMPSizeY];
int G[MaxBMPSizeX][MaxBMPSizeY];
int B[MaxBMPSizeX][MaxBMPSizeY];

int r[MaxBMPSizeX][MaxBMPSizeY];
int g[MaxBMPSizeX][MaxBMPSizeY];
int b[MaxBMPSizeX][MaxBMPSizeY];

// �z���ܴ��x�}
void get_perspective_transform(double **perspective, int ori[4][2], int move[4][2]) {
    double **matrix_k; // �ʺA�G���}�C
    double **matrix_kT; // �ϯx�}���ʺA�G���}�C
    double *matrix_b;
    double matrix_A[8] = {0};

    // �إ߰ʺA�G���}�C
    matrix_k = new double *[8];
    for (int i = 0; i < 8; i++)
        matrix_k[i] = new double[8];

    matrix_kT = new double *[8];
    for (int i = 0; i < 8; i++)
        matrix_kT[i] = new double[8];

    // ��l�Ƥϯx�}
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            if (i == j)
                matrix_kT[i][j] = 1;
            else
                matrix_kT[i][j] = 0;
        }

    matrix_b = new double[8];

    // �p��x�} K
    for (int i = 0; i < 4; i++) {
        matrix_k[i][0] = (double)ori[i][0];
        matrix_k[i][1] = (double)ori[i][1];
        matrix_k[i][2] = (double)1;
        matrix_k[i][6] = -(double)(ori[i][0] * move[i][0]);
        matrix_k[i][7] = -(double)(ori[i][1] * move[i][0]);
        matrix_k[i + 4][3] = (double)ori[i][0];
        matrix_k[i + 4][4] = (double)ori[i][1];
        matrix_k[i + 4][5] = (double)1;
        matrix_k[i + 4][6] = -(double)(ori[i][0] * move[i][1]);
        matrix_k[i + 4][7] = -(double)(ori[i][1] * move[i][1]);

        for (int j = 0; j < 3; j++) {
            matrix_k[i][3 + j] = 0;
            matrix_k[i + 4][j] = 0;
        }

        matrix_b[i] = (double)move[i][0];
        matrix_b[i + 4] = (double)move[i][1];
    }

    // �D�ѯx�}K���f�x�}
    matrix_inverse(8, matrix_k, matrix_kT);

    // �p��z���ܴ������G�x�}
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            matrix_A[i] += matrix_kT[i][j] * matrix_b[j];
        }

    // �N���G�x�}�ഫ���z���ܴ��x�}�Φ�
    int k = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            perspective[i][j] = matrix_A[k++];

    perspective[2][2] = 1;
    return;
}

// �u�ʤ����k
double linear_interpolation(double v00, double v01, double v10, double v11, double dx, double dy) {
    return v00 * (1 - dx) * (1 - dy) + v10 * dx * (1 - dy) + v01 * (1 - dx) * dy + v11 * dx * dy;
}

int main(int argc, char *argv[]) {
    int width, height;

    // Ū����l�Ϲ�
    open_bmp("lena_std.bmp", R, G, B, width, height);


    int original_point[4][2] = {{1, 1}, {511, 1}, {511, 511}, {1, 511}};
    int moved_point[4][2] = {{1, 1}, {480, 12}, {511, 511}, {210, 352}};
    //                         ���U    �k�U      �k�W      ���W

    //�s�x�z���ܴ��x�}�M��f�x�}
    double **perspective;
    double **perspective_inv;
    perspective = new double *[3];
    for (int i = 0; i < 3; i++)
        perspective[i] = new double[3];
    perspective_inv = new double *[3];
    for (int i = 0; i < 3; i++)
        perspective_inv[i] = new double[3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            if (i == j)
                perspective_inv[i][j] = 1;
            else
                perspective_inv[i][j] = 0;
        }

    // ����z���ܴ��x�}
    get_perspective_transform(perspective, original_point, moved_point);

    // �p��f�x�}Inverse Mapping
    matrix_inverse(3, perspective, perspective_inv);

    //�s�Ϲ����e�שM����
    int newWidth = width * 2;
    int newHeight = height * 2;

    // �i��z���ܴ��M�u�ʤ����k
    for (int x = 0; x < newWidth; x++)
        for (int y = 0; y < newHeight; y++)
        {
            double oriP[3] = {double(x), double(y), 1.0};
            double dstP[3] = {0};
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    dstP[i] += perspective_inv[i][j] * oriP[j];

            double ox = dstP[0] / dstP[2];
            double oy = dstP[1] / dstP[2];

            // �i��u�ʤ����k�p��
            int ix = static_cast<int>(floor(ox));
            int iy = static_cast<int>(floor(oy));

            if (ix >= 0 && ix < width - 1 && iy >= 0 && iy < height - 1)
            {
                double dx = ox - ix;
                double dy = oy - iy;

                r[x][y] = static_cast<int>(linear_interpolation(R[ix][iy], R[ix + 1][iy], R[ix][iy + 1], R[ix + 1][iy + 1], dx, dy));
                g[x][y] = static_cast<int>(linear_interpolation(G[ix][iy], G[ix + 1][iy], G[ix][iy + 1], G[ix + 1][iy + 1], dx, dy));
                b[x][y] = static_cast<int>(linear_interpolation(B[ix][iy], B[ix + 1][iy], B[ix][iy + 1], B[ix + 1][iy + 1], dx, dy));
            }
            else
            {
                r[x][y] = 0;
                g[x][y] = 0;
                b[x][y] = 0;
            }
        }
        
    save_bmp("lenna_new1.bmp", r, g, b);
    printf("\n����!\n");

    // ������l�Ϲ����
    close_bmp();

    system("pause");
    return 0;
}


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

// 透視變換矩陣
void get_perspective_transform(double **perspective, int ori[4][2], int move[4][2]) {
    double **matrix_k; // 動態二維陣列
    double **matrix_kT; // 反矩陣的動態二維陣列
    double *matrix_b;
    double matrix_A[8] = {0};

    // 建立動態二維陣列
    matrix_k = new double *[8];
    for (int i = 0; i < 8; i++)
        matrix_k[i] = new double[8];

    matrix_kT = new double *[8];
    for (int i = 0; i < 8; i++)
        matrix_kT[i] = new double[8];

    // 初始化反矩陣
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            if (i == j)
                matrix_kT[i][j] = 1;
            else
                matrix_kT[i][j] = 0;
        }

    matrix_b = new double[8];

    // 計算矩陣 K
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

    // 求解矩陣K的逆矩陣
    matrix_inverse(8, matrix_k, matrix_kT);

    // 計算透視變換的結果矩陣
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            matrix_A[i] += matrix_kT[i][j] * matrix_b[j];
        }

    // 將結果矩陣轉換為透視變換矩陣形式
    int k = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            perspective[i][j] = matrix_A[k++];

    perspective[2][2] = 1;
    return;
}

// 線性內插法
double linear_interpolation(double v00, double v01, double v10, double v11, double dx, double dy) {
    return v00 * (1 - dx) * (1 - dy) + v10 * dx * (1 - dy) + v01 * (1 - dx) * dy + v11 * dx * dy;
}

int main(int argc, char *argv[]) {
    int width, height;

    // 讀取原始圖像
    open_bmp("lena_std.bmp", R, G, B, width, height);


    int original_point[4][2] = {{1, 1}, {511, 1}, {511, 511}, {1, 511}};
    int moved_point[4][2] = {{1, 1}, {480, 12}, {511, 511}, {210, 352}};
    //                         左下    右下      右上      左上

    //存儲透視變換矩陣和其逆矩陣
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

    // 獲取透視變換矩陣
    get_perspective_transform(perspective, original_point, moved_point);

    // 計算逆矩陣Inverse Mapping
    matrix_inverse(3, perspective, perspective_inv);

    //新圖像的寬度和高度
    int newWidth = width * 2;
    int newHeight = height * 2;

    // 進行透視變換和線性內插法
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

            // 進行線性內插法計算
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
    printf("\n完成!\n");

    // 關閉原始圖像文件
    close_bmp();

    system("pause");
    return 0;
}


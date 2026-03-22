#include "../inc/jacobi_boundary.h"
using namespace std;

void init_boundary_2d(int coords[2], int dims[2], int local_rows, int local_cols, vector<vector<double>>& a) 
{
    // 内部点
    for (int i = 0; i < local_rows + 2; ++i) {
        for (int j = 0; j < local_cols + 2; ++j) {
            a[i][j] = 0.0;
        }
    }

    // 填充上边界
    if (coords[0] == 0) {
        for (int j = 0; j < local_cols + 2; ++j) {
            a[1][j] = 1000.0;
        }
    }

    // 填充下边界
    if (coords[0] == dims[0] - 1) {
        for (int j = 0; j < local_cols + 2; ++j) {
            a[local_rows][j] = 1000.0;
        }
    }

    // 填充左边界
    if (coords[1] == 0) {
        for (int i = 0; i < local_rows + 2; ++i) {
            a[i][1] = 1000.0;
        }
    }

    // 填充右边界
    if (coords[1] == dims[1] - 1) {
        for (int i = 0; i < local_rows + 2; ++i) {
            a[i][local_cols] = 1000.0;
        }
    }
}
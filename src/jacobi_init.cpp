#include "../inc/jacobi_init.h"
using namespace std;

// 自动计算最均衡的网格划分
void setup_grid_dims(int size, int dims[2])
{
    dims[0] = 0; dims[1] = 0; // 设0表示自动分配
    MPI_Dims_create(size, 2, dims);
}

// 计算当前进程负责
void calculate_local_size(int dims[2], int coords[2], int& local_rows, int& local_cols)
{
    // 处理行
    int base_rows = TOTAL_ROWS / dims[0];
    int rem_rows = TOTAL_ROWS % dims[0];
    local_rows = (coords[0] < rem_rows) ? base_rows + 1 : base_rows;

    // 处理列
    int base_cols = TOTAL_COLS / dims[1];
    int rem_cols = TOTAL_COLS % dims[1];
    local_cols = (coords[1] < rem_cols) ? base_cols + 1 : base_cols;
}

// 初始化矩阵内存
void init_matrix_memory_2d(int local_rows, int local_cols, vector<vector<double>>& a, vector<vector<double>>& b)
{
    a.assign(local_rows + 2, vector<double>(local_cols + 2, 0.0));
    b.assign(local_rows + 2, vector<double>(local_cols + 2, 0.0));
}
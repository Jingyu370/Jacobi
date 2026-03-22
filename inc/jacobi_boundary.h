#ifndef JACOBI_BOUNDARY_H
#define JACOBI_BOUNDARY_H

#include "jacobi_common.h"
#include <vector>
using namespace std;

// 基于笛卡尔拓扑初始化边界
void init_boundary_2d(int coords[2], int dims[2], int local_rows, int local_cols, vector<vector<double>>& a);

#endif
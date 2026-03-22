#ifndef JACOBI_ITERATION_H
#define JACOBI_ITERATION_H

#include "jacobi_common.h"
#include <vector>
using namespace std;

// 迭代主循环驱动 (非阻塞通信版本)
void jacobi_iteration_loop_2d(MPI_Comm cart_comm, int up, int down, int left, int right, 
                             int local_rows, int local_cols, vector<vector<double>>& a, vector<vector<double>>& b, 
                             double& comm_init_time);

#endif
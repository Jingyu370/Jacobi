#ifndef JACOBI_INIT_H
#define JACOBI_INIT_H

#include "jacobi_common.h"
#include <vector>
using namespace std;

void setup_grid_dims(int size, int dims[2]);

void calculate_local_size(int dims[2], int coords[2], int& local_rows, int& local_cols);

void init_matrix_memory_2d(int local_rows, int local_cols, vector<vector<double>>& a, vector<vector<double>>& b);

#endif
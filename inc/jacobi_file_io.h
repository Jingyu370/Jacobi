#ifndef JACOBI_FILE_IO_H
#define JACOBI_FILE_IO_H

#include "jacobi_common.h"
#include <vector>
#include <string>

using namespace std;

string generate_filename(int size);

void write_result_header(const string& filename);

void write_all_process_data(int rank, int size, int local_rows, int local_cols, int coords[2], const string& filename, vector<vector<double>>& a);

#endif
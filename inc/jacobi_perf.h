#ifndef JACOBI_PERF_H
#define JACOBI_PERF_H

#include "jacobi_common.h"
using namespace std;

// 写入性能报告到文件
void write_performance_report(const string& filename, int size, int dims[2], double mem_init_time, double comm_total, double compute_total, double io_time, double total_time);

// 控制台输出性能报告
void print_performance_console(int size, int dims[2], double mem_init_time, double comm_total, double compute_total, double io_time, double total_time);

#endif // JACOBI_PERF_H
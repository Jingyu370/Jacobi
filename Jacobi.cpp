#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include "mpi.h"

#ifndef TOTALSIZE
#define TOTALSIZE 128
#endif

using namespace std;

const int totalsize = TOTALSIZE;
const int step = 1000;

const int tag1 = 3;
const int tag2 = 4;

int main(int argc, char **argv) {
    int rank, size;
    int upper, lower;
    int begin_row = 1, end_row;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // 全局总计时
    double total_start = MPI_Wtime();

    // 内存初始化计时
    double mem_init_start = MPI_Wtime();
    const int base_size = totalsize / size;
    const int remainder = totalsize % size;

    int mysize;
    if (rank < remainder) {
        mysize = base_size + 1;
    } else {
        mysize = base_size;
    }

    MPI_Request req[4];
    MPI_Status status[4];

    end_row = mysize;

    vector<vector<double>> a(mysize + 2, vector<double>(totalsize, 0.0));
    vector<vector<double>> b(mysize + 2, vector<double>(totalsize, 0.0));

    // 边界初始化
    for (int i = 0; i < totalsize; ++i) {
        a[0][i] = 8.0;
        a[mysize + 1][i] = 8.0;
    }
    for (int i = 0; i < mysize + 2; ++i) {
        a[i][0] = 8.0;
        a[i][totalsize - 1] = 8.0;
    }
    if (rank == 0) {
        begin_row = 2;
        for (int j = 0; j < totalsize; ++j) {
            a[1][j] = 8.0;
        }
    }
    if (rank == size - 1) {
        end_row = mysize - 1;
        for (int j = 0; j < totalsize; ++j) {
            a[mysize][j] = 8.0;
        }
    }

    upper = (rank > 0) ? rank - 1 : MPI_PROC_NULL;
    lower = (rank < size - 1) ? rank + 1 : MPI_PROC_NULL;

    double mem_init_end = MPI_Wtime();
    double mem_init_time = mem_init_end - mem_init_start;

    double iteration_total_time = 0.0;
    double comm_init_time = 0.0;

    double com_t1 = MPI_Wtime();
    MPI_Send_init(&b[end_row][0], totalsize, MPI_DOUBLE, lower, tag1, MPI_COMM_WORLD, &req[0]);
    MPI_Send_init(&b[begin_row][0], totalsize, MPI_DOUBLE, upper, tag2, MPI_COMM_WORLD, &req[1]);

    MPI_Recv_init(&a[0][0], totalsize, MPI_DOUBLE, upper, tag1, MPI_COMM_WORLD, &req[2]);
    MPI_Recv_init(&a[mysize + 1][0], totalsize, MPI_DOUBLE, lower, tag2, MPI_COMM_WORLD, &req[3]);
    double com_t2 = MPI_Wtime();
    comm_init_time = com_t2 - com_t1;

    // 迭代总计时开始
    double iteration_start = MPI_Wtime();
    for (int n = 0; n < step; ++n) {

        for (int j = 1; j < totalsize - 1; ++j) {
            b[begin_row][j] = 0.25 * (a[begin_row + 1][j] + a[begin_row - 1][j] + a[begin_row][j + 1] + a[begin_row][j - 1]);
            b[end_row][j] = 0.25 * (a[end_row + 1][j] + a[end_row - 1][j] + a[end_row][j + 1] + a[end_row][j - 1]);
        }

        MPI_Startall(4, req);

        for (int i = begin_row + 1; i < end_row; ++i) {
            for (int j = 1; j < totalsize - 1; ++j) {
                b[i][j] = 0.25 * (a[i + 1][j] + a[i - 1][j] + a[i][j + 1] + a[i][j - 1]);
            }
        }

        MPI_Waitall(4, req, status);

        for (int i = begin_row; i <= end_row; ++i) {
            for (int j = 1; j < totalsize - 1; ++j) {
                a[i][j] = b[i][j];
            }
        }
    }
    // 迭代总计时结束
    double iteration_end = MPI_Wtime();
    iteration_total_time = iteration_end - iteration_start;

    MPI_Barrier(MPI_COMM_WORLD);

    // 文件IO计时
    double io_start = MPI_Wtime();
    string filename = "result_nonblock/result_" + to_string(size) + "proc_" +
                      to_string(totalsize) + "rows_" + to_string(step) + "steps.txt";

    if (rank == 0) {
        ofstream outfile(filename);
        if (outfile.is_open()) {
            outfile << "==================================== Jacobi迭代结果汇总 ====================================" << endl;
            outfile << setw(10) << "进程ID" << setw(10) << "本地行号" << setw(8) << "列";
            for (int j = 0; j < totalsize; ++j) {
                outfile << setw(8) << j;
            }
            outfile << endl;
            outfile << "--------------------------------------------------------------------------------------------" << endl;
            outfile.close();
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    for (int myid = 0; myid < size; ++myid) {
        if (rank == myid) {
            ofstream outfile(filename, ios::app);
            if (outfile.is_open()) {
                for (int i = 1; i <= mysize; ++i) {
                    outfile << setw(10) << myid << setw(10) << i << setw(8) << ":";
                    for (int j = 0; j < totalsize; ++j) {
                        outfile << setw(8) << fixed << setprecision(2) << a[i][j];
                    }
                    outfile << endl;
                }
                outfile.close();
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    double io_end = MPI_Wtime();
    double io_time = io_end - io_start;

    double total_end = MPI_Wtime();
    double total_time = total_end - total_start;

    // 输出性能报告
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        ofstream perf_file(filename, ios::app);
        if (perf_file.is_open()) {
            perf_file << "\n\n==================================== Jacobi性能剖析报告 ====================================" << endl;
            perf_file << "进程总数: " << size << ", 矩阵规模: " << totalsize << "列, 迭代步数: " << step << endl;
            perf_file << "--------------------------------------------------------------------------------------------" << endl;
            perf_file << fixed << setprecision(6);
            perf_file << "内存初始化耗时:          " << setw(10) << mem_init_time << " 秒" << endl;
            perf_file << "通信对象初始化耗时:      " << setw(10) << comm_init_time << " 秒" << endl;
            perf_file << "迭代总耗时(含通信+计算): " << setw(10) << iteration_total_time << " 秒" << endl;
            perf_file << "文件IO总耗时:           " << setw(10) << io_time << " 秒" << endl;
            perf_file << "程序总耗时:             " << setw(10) << total_time << " 秒" << endl;
            perf_file << "--------------------------------------------------------------------------------------------" << endl;
            perf_file << "============================================================================================\n" << endl;
            perf_file.close();
        }

        cout << "==================================== Jacobi性能剖析报告 ====================================" << endl;
        cout << "进程总数: " << size << ", 矩阵规模: " << totalsize << "列, 迭代步数: " << step << endl;
        cout << "--------------------------------------------------------------------------------------------" << endl;
        cout << fixed << setprecision(6);
        cout << "内存初始化耗时:          " << setw(10) << mem_init_time << " 秒" << endl;
        cout << "通信对象初始化耗时:      " << setw(10) << comm_init_time << " 秒" << endl;
        cout << "迭代总耗时(含通信+计算): " << setw(10) << iteration_total_time << " 秒" << endl;
        cout << "文件IO总耗时:           " << setw(10) << io_time << " 秒" << endl;
        cout << "程序总耗时:             " << setw(10) << total_time << " 秒" << endl;
        cout << "--------------------------------------------------------------------------------------------" << endl;
        cout << "============================================================================================\n" << endl;
    }

    for (int i = 0; i < 4; ++i) {
        MPI_Request_free(&req[i]);
    }

    MPI_Finalize();
    return 0;
}
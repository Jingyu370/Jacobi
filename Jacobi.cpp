#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include "mpi.h"

using namespace std;

const int totalsize = 2048;    // 全局矩阵列数（可变）
const int step = 1000;         // 迭代步数

const int tag1 = 3;
const int tag2 = 4;

int main(int argc, char **argv){
    int rank, size;
    int upper, lower;
    int begin_row = 1, end_row;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //初始化计时：总耗时起点
    double total_start = MPI_Wtime();

    //内存与变量初始化计时
    double mem_init_start = MPI_Wtime();
    const int base_size = totalsize / size;
    const int remainder = totalsize % size;
    
    // 修改：当线程数无法整除时，按顺序均分
    int mysize;
    if (rank < remainder) {
        mysize = base_size + 1;
    } else {
        mysize = base_size;
    }

    MPI_Status status;
    end_row = mysize;

    vector<vector<double>> a(mysize + 2, vector<double>(totalsize, 0.0));
    vector<vector<double>> b(mysize + 2, vector<double>(totalsize, 0.0));

    for(int i = 0; i < totalsize; ++i){
        a[0][i] = 8.0;
        a[mysize + 1][i] = 8.0;
    }
 
    for (int i = 0; i < mysize + 2; ++i) {
        a[i][0] = 8.0;
        a[i][totalsize - 1] = 8.0;
    }

    if(rank == 0){
        begin_row = 2;
        for(int j = 0; j < totalsize ; ++j){
            a[1][j] = 8.0; 
        }
    }

    if(rank == size - 1){
        end_row = mysize - 1;
        for(int j = 0; j < totalsize ; ++j){
            a[mysize][j] = 8.0;
        }
    }

    upper = (rank > 0) ? rank - 1 : MPI_PROC_NULL;
    lower = (rank < size - 1) ? rank + 1 : MPI_PROC_NULL;
    double mem_init_end = MPI_Wtime();
    double mem_init_time = mem_init_end - mem_init_start;

    //Jacobi迭代计时
    double comm_total = 0.0;   // 总通信耗时
    double compute_total = 0.0;// 总计算耗时
    for(int n = 0; n < step; ++n){
        // 通信阶段计时
        double comm_start = MPI_Wtime();
        MPI_Sendrecv(&a[1][0], totalsize, MPI_DOUBLE, upper, tag1,
                    &a[mysize + 1][0], totalsize, MPI_DOUBLE, lower, tag1,
                    MPI_COMM_WORLD, &status);

        MPI_Sendrecv(&a[mysize][0], totalsize, MPI_DOUBLE, lower, tag2,
                    &a[0][0], totalsize, MPI_DOUBLE, upper, tag2,
                    MPI_COMM_WORLD, &status);
        double comm_end = MPI_Wtime();
        comm_total += (comm_end - comm_start);

        // 计算阶段计时
        double compute_start = MPI_Wtime();
        for(int i = begin_row; i <= end_row; ++i){
            for (int j = 1; j < totalsize - 1; ++j) {
                b[i][j] = (a[i + 1][j] + a[i - 1][j] + a[i][j + 1] + a[i][j - 1]) * 0.25;
            }
        }

        for (int i = begin_row; i <= end_row; ++i) {
            for (int j = 1; j < totalsize - 1; ++j) {
                a[i][j] = b[i][j];
            }
        }
        double compute_end = MPI_Wtime();
        compute_total += (compute_end - compute_start);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //文件IO耗时 ==========
    double io_start = MPI_Wtime();
    string filename = "result/result_" + to_string(size) + "proc_" + 
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

    //总耗时结束
    double total_end = MPI_Wtime();
    double total_time = total_end - total_start;

    // ========== 性能报告输出（仅rank=0汇总） ==========
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        cout << "==================================== Jacobi性能剖析报告 ====================================" << endl;
        cout << "进程总数: " << size << ", 矩阵规模: " << totalsize << "列, 迭代步数: " << step << endl;
        cout << "--------------------------------------------------------------------------------------------" << endl;
        cout << fixed << setprecision(6);
        cout << "内存/变量初始化耗时:    " << setw(10) << mem_init_time << " 秒" << endl;
        cout << "迭代总通信耗时:         " << setw(10) << comm_total << " 秒 (每步平均: " << comm_total/step << " 秒)" << endl;
        cout << "迭代总计算耗时:         " << setw(10) << compute_total << " 秒 (每步平均: " << compute_total/step << " 秒)" << endl;
        cout << "文件IO总耗时:           " << setw(10) << io_time << " 秒" << endl;
        cout << "程序总耗时:             " << setw(10) << total_time << " 秒" << endl;
        cout << "--------------------------------------------------------------------------------------------" << endl;
        cout << "耗时占比分析（基于总耗时）:" << endl;
        cout << "内存初始化:   " << setw(8) << (mem_init_time/total_time)*100 << " %" << endl;
        cout << "通信耗时:     " << setw(8) << (comm_total/total_time)*100 << " %" << endl;
        cout << "计算耗时:     " << setw(8) << (compute_total/total_time)*100 << " %" << endl;
        cout << "文件IO:       " << setw(8) << (io_time/total_time)*100 << " %" << endl;
        cout << "其他耗时:     " << setw(8) << (total_time - mem_init_time - comm_total - compute_total - io_time)/total_time*100 << " %" << endl;
        cout << "============================================================================================\n" << endl;
    }

    MPI_Finalize();
    return 0;
}
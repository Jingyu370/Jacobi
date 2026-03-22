#include "../inc/jacobi_perf.h"
using namespace std;

// 写入性能报告到文件
void write_performance_report(const string& filename, int size, int dims[2], double mem_init_time, double comm_total, double compute_total, double io_time, double total_time) {
    ofstream perf_file(filename, ios::app); // 结果末尾追加性能报告
    if (perf_file.is_open()) {
        perf_file << "\n\n========================= Jacobi 二维分解性能报告 =========================" << endl;
        perf_file << "进程总数: " << size << " (网格形状: " << dims[0] << "x" << dims[1] << ")" << endl;
        perf_file << "矩阵规模: " << TOTAL_ROWS << "行 x " << TOTAL_COLS << "列" << endl;
        perf_file << "迭代步数: " << step << endl;
        perf_file << "--------------------------------------------------------------------------" << endl;
        
        perf_file << fixed << setprecision(6);
        perf_file << "内存/变量初始化耗时:    " << setw(10) << mem_init_time << " 秒" << endl;
        perf_file << "迭代总通信耗时:         " << setw(10) << comm_total << " 秒 (每步平均: " << comm_total/step << " 秒)" << endl;
        perf_file << "迭代总计算耗时:         " << setw(10) << compute_total << " 秒 (每步平均: " << compute_total/step << " 秒)" << endl;
        perf_file << "文件 IO 总耗时:         " << setw(10) << io_time << " 秒" << endl;
        perf_file << "程序总耗时 (MPI_Wtime): " << setw(10) << total_time << " 秒" << endl;
        perf_file << "--------------------------------------------------------------------------" << endl;
        
        perf_file << "耗时占比分析 (基于程序总耗时):" << endl;
        perf_file << "内存初始化:   " << setw(8) << (mem_init_time/total_time)*100 << " %" << endl;
        perf_file << "通信耗时:     " << setw(8) << (comm_total/total_time)*100 << " %" << endl;
        perf_file << "计算耗时:     " << setw(8) << (compute_total/total_time)*100 << " %" << endl;
        perf_file << "文件 IO:       " << setw(8) << (io_time/total_time)*100 << " %" << endl;
        
        double other_time = total_time - mem_init_time - comm_total - compute_total - io_time;
        // 规避浮点数误差出现负数占比
        if (other_time < 0) other_time = 0; 
        perf_file << "其他耗时:     " << setw(8) << (other_time/total_time)*100 << " %" << endl;
        perf_file << "==========================================================================\n" << endl;
        perf_file.close();
    }
}

// 控制台输出性能报告
void print_performance_console(int size, int dims[2], double mem_init_time, double comm_total, double compute_total, double io_time, double total_time) {
    cout << "\n========================= Jacobi 二维分解性能报告 =========================" << endl;
    cout << "进程总数: " << size << " (网格形状: " << dims[0] << "x" << dims[1] << ")" << endl;
    cout << "矩阵规模: " << TOTAL_ROWS << "行 x " << TOTAL_COLS << "列" << endl;
    cout << "迭代步数: " << step << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    
    cout << fixed << setprecision(6);
    cout << "内存/变量初始化耗时:    " << setw(10) << mem_init_time << " 秒" << endl;
    cout << "迭代总通信耗时:         " << setw(10) << comm_total << " 秒 (每步平均: " << comm_total/step << " 秒)" << endl;
    cout << "迭代总计算耗时:         " << setw(10) << compute_total << " 秒 (每步平均: " << compute_total/step << " 秒)" << endl;
    cout << "文件 IO 总耗时:         " << setw(10) << io_time << " 秒" << endl;
    cout << "程序总耗时 (MPI_Wtime): " << setw(10) << total_time << " 秒" << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    
    cout << "耗时占比分析 (基于程序总耗时):" << endl;
    cout << "内存初始化:   " << setw(8) << (mem_init_time/total_time)*100 << " %" << endl;
    cout << "通信耗时:     " << setw(8) << (comm_total/total_time)*100 << " %" << endl;
    cout << "计算耗时:     " << setw(8) << (compute_total/total_time)*100 << " %" << endl;
    cout << "文件 IO:       " << setw(8) << (io_time/total_time)*100 << " %" << endl;
    
    double other_time = total_time - mem_init_time - comm_total - compute_total - io_time;
    if (other_time < 0) other_time = 0;
    cout << "其他耗时:     " << setw(8) << (other_time/total_time)*100 << " %" << endl;
    cout << "==========================================================================\n" << endl;
}
#include "../inc/jacobi_perf.h"
using namespace std;

void write_performance_report(const string& filename, int size, int dims[2], double mem_init_time, double comm_init_time, double iteration_time, double io_time, double total_time) {
    ofstream perf_file(filename, ios::app);
    if (perf_file.is_open()) {
        perf_file << "\n\n========================= Jacobi 二维非阻塞性能报告 =========================" << endl;
        perf_file << "进程总数: " << size << " (网格形状: " << dims[0] << "x" << dims[1] << ")" << endl;
        perf_file << "矩阵规模: " << TOTAL_ROWS << "行 x " << TOTAL_COLS << "列" << endl;
        perf_file << "迭代步数: " << step << endl;
        perf_file << "--------------------------------------------------------------------------" << endl;
        
        perf_file << fixed << setprecision(6);
        perf_file << "内存/变量初始化耗时:      " << setw(10) << mem_init_time << " 秒" << endl;
        perf_file << "通信对象初始化耗时:       " << setw(10) << comm_init_time << " 秒" << endl;
        perf_file << "迭代总耗时(含通信+计算):  " << setw(10) << iteration_time << " 秒" << endl;
        perf_file << "文件 IO 总耗时:           " << setw(10) << io_time << " 秒" << endl;
        perf_file << "程序总耗时 (MPI_Wtime):   " << setw(10) << total_time << " 秒" << endl;
        perf_file << "==========================================================================\n" << endl;
        perf_file.close();
    }
}

void print_performance_console(int size, int dims[2], double mem_init_time, double comm_init_time, double iteration_time, double io_time, double total_time) {
    cout << "\n========================= Jacobi 二维非阻塞性能报告 =========================" << endl;
    cout << "进程总数: " << size << " (网格形状: " << dims[0] << "x" << dims[1] << ")" << endl;
    cout << "矩阵规模: " << TOTAL_ROWS << "行 x " << TOTAL_COLS << "列" << endl;
    cout << "迭代步数: " << step << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    
    cout << fixed << setprecision(6);
    cout << "内存/变量初始化耗时:      " << setw(10) << mem_init_time << " 秒" << endl;
    cout << "通信对象初始化耗时:       " << setw(10) << comm_init_time << " 秒" << endl;
    cout << "迭代总耗时(含通信+计算):  " << setw(10) << iteration_time << " 秒" << endl;
    cout << "文件 IO 总耗时:           " << setw(10) << io_time << " 秒" << endl;
    cout << "程序总耗时 (MPI_Wtime):   " << setw(10) << total_time << " 秒" << endl;
    cout << "==========================================================================\n" << endl;
}
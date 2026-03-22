#include "../inc/jacobi_file_io.h"
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

string generate_filename(int size) {
    return "./result/result_proc" + to_string(size) + "_" + 
           to_string(TOTAL_ROWS) + "x" + to_string(TOTAL_COLS) + "_" + 
           to_string(step) + "steps.txt";
}

// 写入结果文件头部（只有 Rank 0 调用）
void write_result_header(const string& filename) {
    ofstream outfile(filename, ios::out);
    if (outfile.is_open()) {
        outfile << "================ Jacobi 2D 迭代结果汇总 ================" << endl;
        outfile << "全局规模: " << TOTAL_ROWS << " x " << TOTAL_COLS << endl;
        outfile << "迭代步数: " << step << endl;
        outfile << "--------------------------------------------------------" << endl;
        outfile.close();
    }
}

// 写入单个进程的数据
void write_process_data_2d(int rank, int local_rows, int local_cols, int coords[2], const string& filename, vector<vector<double>>& a)
{
    ofstream outfile(filename, ios::app); // 追加模式
    if (outfile.is_open()) {
        outfile << "\n[进程 Rank " << rank << " | 坐标 (" << coords[0] << "," << coords[1] << ")]" << endl;
        outfile << "本地子块大小: " << local_rows << "x" << local_cols << endl;
        
        for (int i = 1; i <= local_rows; ++i) {
            outfile << "行 " << setw(3) << i << ": ";
            for (int j = 1; j <= local_cols; ++j) {
                outfile << setw(8) << fixed << setprecision(2) << a[i][j];
            }
            outfile << endl;
        }
        outfile << "--------------------------------------------------------" << endl;
        outfile.close();
    }
}

// 按顺序写入所有进程的数据
void write_all_process_data(int rank, int size, int local_rows, int local_cols, int coords[2], const string& filename, vector<vector<double>>& a)
{
    for (int p = 0; p < size; ++p) {
        if (rank == p) {
            write_process_data_2d(rank, local_rows, local_cols, coords, filename, a);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}
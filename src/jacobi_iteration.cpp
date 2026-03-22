#include "../inc/jacobi_iteration.h"

void jacobi_iteration_loop_2d(MPI_Comm cart_comm, int up, int down, int left, int right, 
                             int local_rows, int local_cols, vector<vector<double>>& a, vector<vector<double>>& b, 
                             double& comm_total, double& compute_total) 
{
    for (int s = 0; s < step; ++s) {
        // 1. 通信计时
        jacobi_communication_2d(cart_comm, up, down, left, right, local_rows, local_cols, a, comm_total);
        
        // 2. 计算计时
        double start_comp = MPI_Wtime();
        jacobi_computation_2d(local_rows, local_cols, a, b);
        compute_total += (MPI_Wtime() - start_comp);
    }
}

void jacobi_communication_2d(MPI_Comm cart_comm, int up, int down, int left, int right, int local_rows, int local_cols, vector<vector<double>>& a, double& comm_time)
{
    MPI_Status status;
    double start = MPI_Wtime();

    // 上下通信
    MPI_Sendrecv(&a[1][0], local_cols + 2, MPI_DOUBLE, up, TAG_UP,
                 &a[local_rows + 1][0], local_cols + 2, MPI_DOUBLE, down, TAG_UP,
                 cart_comm, &status);

    MPI_Sendrecv(&a[local_rows][0], local_cols + 2, MPI_DOUBLE, down, TAG_DOWN,
                 &a[0][0], local_cols + 2, MPI_DOUBLE, up, TAG_DOWN,
                 cart_comm, &status);

    //左右通信 (内存不连续，需要 Buffer 中转)
    vector<double> send_buf(local_rows);
    vector<double> recv_buf(local_rows);

    //向左发，从右收
    if (left != MPI_PROC_NULL || right != MPI_PROC_NULL) {
        for (int i = 0; i < local_rows; ++i) send_buf[i] = a[i + 1][1]; // 抽取本地第1列
        MPI_Sendrecv(send_buf.data(), local_rows, MPI_DOUBLE, left, TAG_LEFT,
                     recv_buf.data(), local_rows, MPI_DOUBLE, right, TAG_LEFT,
                     cart_comm, &status);
        if (right != MPI_PROC_NULL) {
            for (int i = 0; i < local_rows; ++i) a[i + 1][local_cols + 1] = recv_buf[i]; // 放入右虚点
        }
    }

    //向右发，从左收
    if (right != MPI_PROC_NULL || left != MPI_PROC_NULL) {
        for (int i = 0; i < local_rows; ++i) send_buf[i] = a[i + 1][local_cols]; // 抽取本地最后一列
        MPI_Sendrecv(send_buf.data(), local_rows, MPI_DOUBLE, right, TAG_RIGHT,
                     recv_buf.data(), local_rows, MPI_DOUBLE, left, TAG_RIGHT,
                     cart_comm, &status);
        if (left != MPI_PROC_NULL) {
            for (int i = 0; i < local_rows; ++i) a[i + 1][0] = recv_buf[i]; // 放入左虚点
        }
    }

    comm_time += (MPI_Wtime() - start);
}

void jacobi_computation_2d(int local_rows, int local_cols, vector<vector<double>>& a, vector<vector<double>>& b) 
{
    // 计算新值到b
    for (int i = 1; i <= local_rows; ++i) {
        for (int j = 1; j <= local_cols; ++j) {
            b[i][j] = (a[i - 1][j] + a[i + 1][j] + a[i][j - 1] + a[i][j + 1]) * 0.25;
        }
    }

    // 更新回a
    for (int i = 1; i <= local_rows; ++i) {
        for (int j = 1; j <= local_cols; ++j) {
            a[i][j] = b[i][j];
        }
    }
}
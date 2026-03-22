#include "../inc/jacobi_iteration.h"

void jacobi_iteration_loop_2d(MPI_Comm cart_comm, int up, int down, int left, int right, 
                             int local_rows, int local_cols, vector<vector<double>>& a, vector<vector<double>>& b, 
                             double& comm_init_time) 
{
    MPI_Request reqs[8];
    MPI_Status statuses[8];

    // 用于左右通信的非连续内存缓冲区
    vector<double> send_left(local_rows, 0.0);
    vector<double> recv_left(local_rows, 0.0);
    vector<double> send_right(local_rows, 0.0);
    vector<double> recv_right(local_rows, 0.0);

    //初始化持久通信请求
    double start_init = MPI_Wtime();

    // 上下方向
    MPI_Send_init(&b[1][0], local_cols + 2, MPI_DOUBLE, up, TAG_UP, cart_comm, &reqs[0]);
    MPI_Recv_init(&a[0][0], local_cols + 2, MPI_DOUBLE, up, TAG_DOWN, cart_comm, &reqs[1]);

    MPI_Send_init(&b[local_rows][0], local_cols + 2, MPI_DOUBLE, down, TAG_DOWN, cart_comm, &reqs[2]);
    MPI_Recv_init(&a[local_rows + 1][0], local_cols + 2, MPI_DOUBLE, down, TAG_UP, cart_comm, &reqs[3]);

    // 左右方向
    MPI_Send_init(send_left.data(), local_rows, MPI_DOUBLE, left, TAG_LEFT, cart_comm, &reqs[4]);
    MPI_Recv_init(recv_left.data(), local_rows, MPI_DOUBLE, left, TAG_RIGHT, cart_comm, &reqs[5]);

    MPI_Send_init(send_right.data(), local_rows, MPI_DOUBLE, right, TAG_RIGHT, cart_comm, &reqs[6]);
    MPI_Recv_init(recv_right.data(), local_rows, MPI_DOUBLE, right, TAG_LEFT, cart_comm, &reqs[7]);

    comm_init_time = MPI_Wtime() - start_init;

    // 迭代主循环
    for (int s = 0; s < step; ++s) {
        
        //上下边界行
        for (int j = 1; j <= local_cols; ++j) {
            b[1][j] = (a[0][j] + a[2][j] + a[1][j - 1] + a[1][j + 1]) * 0.25;
            if (local_rows > 1) {
                b[local_rows][j] = (a[local_rows - 1][j] + a[local_rows + 1][j] + a[local_rows][j - 1] + a[local_rows][j + 1]) * 0.25;
            }
        }
        
        //左右边界列
        for (int i = 2; i <= local_rows - 1; ++i) {
            b[i][1] = (a[i - 1][1] + a[i + 1][1] + a[i][0] + a[i][2]) * 0.25;
            if (local_cols > 1) {
                b[i][local_cols] = (a[i - 1][local_cols] + a[i + 1][local_cols] + a[i][local_cols - 1] + a[i][local_cols + 1]) * 0.25;
            }
        }

        if (left != MPI_PROC_NULL) {
            for (int i = 1; i <= local_rows; ++i) send_left[i - 1] = b[i][1];
        }
        if (right != MPI_PROC_NULL) {
            for (int i = 1; i <= local_rows; ++i) send_right[i - 1] = b[i][local_cols];
        }

        MPI_Startall(8, reqs);


        for (int i = 2; i <= local_rows - 1; ++i) {
            for (int j = 2; j <= local_cols - 1; ++j) {
                b[i][j] = (a[i - 1][j] + a[i + 1][j] + a[i][j - 1] + a[i][j + 1]) * 0.25;
            }
        }

        MPI_Waitall(8, reqs, statuses);

        if (left != MPI_PROC_NULL) {
            for (int i = 1; i <= local_rows; ++i) a[i][0] = recv_left[i - 1];
        }
        if (right != MPI_PROC_NULL) {
            for (int i = 1; i <= local_rows; ++i) a[i][local_cols + 1] = recv_right[i - 1];
        }

        for (int i = 1; i <= local_rows; ++i) {
            for (int j = 1; j <= local_cols; ++j) {
                a[i][j] = b[i][j];
            }
        }
    }

    for (int i = 0; i < 8; ++i) {
        MPI_Request_free(&reqs[i]);
    }
}
#include "../inc/allHeaders.h"
using namespace std;

int main(int argc, char **argv){
    int rank, size;
    int dims[2] = {0, 0};          // 维度：dims[0]为行方向，dims[1]为列方向
    int coords[2];                 // 当前进程在网格中的坐标
    int periods[2] = {0, 0};       // 是否周期性（0表示非周期，即有边界，界外为空）
    int up, down, left, right;     // 四个方向的邻居进程号
    
    int local_rows, local_cols;
    vector<vector<double>> a, b;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    setup_grid_dims(size, dims);
    
    MPI_Comm cart_comm;
    // 创建通信域
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
    // 在新通信域中获取当前的 rank 和 坐标
    MPI_Comm_rank(cart_comm, &rank);
    MPI_Cart_coords(cart_comm, rank, 2, coords);

    // 自动查找邻居进程
    MPI_Cart_shift(cart_comm, 0, 1, &up, &down);
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);

    // 总计时开始
    double total_start = MPI_Wtime();

    // 内存与子块初始化
    double mem_init_start = MPI_Wtime();

    // 计算当前进程负责的局部规模
    calculate_local_size(dims, coords, local_rows, local_cols);

    // 分配带矩阵的内存
    init_matrix_memory_2d(local_rows, local_cols, a, b);
    
    // 边界初始化函数
    init_boundary_2d(coords, dims, local_rows, local_cols, a);

    double mem_init_time = MPI_Wtime() - mem_init_start;

    // Jacobi 迭代
    double comm_init_time = 0.0;
    
    double iteration_start = MPI_Wtime(); // 记录迭代开始
    
    jacobi_iteration_loop_2d(cart_comm, up, down, left, right, local_rows, local_cols, a, b, comm_init_time);
    
    MPI_Barrier(cart_comm);
    double iteration_time = MPI_Wtime() - iteration_start; // 记录迭代总耗时

    // 文件 IO
    double io_start = MPI_Wtime();
    string filename = generate_filename(size);
    if (rank == 0) {
        write_result_header(filename);
    }
    MPI_Barrier(cart_comm);
    
    write_all_process_data(rank, size, local_rows, local_cols, coords, filename, a);

    double io_time = MPI_Wtime() - io_start;
    double total_time = MPI_Wtime() - total_start;

    // 性能报告
    if (rank == 0) {
        print_performance_console(size, dims, mem_init_time, comm_init_time, iteration_time, io_time, total_time);
        write_performance_report(filename, size, dims, mem_init_time, comm_init_time, iteration_time, io_time, total_time);
    }

    MPI_Finalize();
    return 0;
}
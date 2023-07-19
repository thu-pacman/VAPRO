#include <iostream>
#include <cmath>
#include <mpi.h>
#include <cassert>
//#include <random>

//enum communication
//{
//    Chain,
//};
const int MAX_LOOP_LEN = 100000000;    // maximum of send_size
const int MAX_LOOPS = 1000000;        // MAX LOOPS LIMIMT
int send_size = 100000000;    // comm time
int loop_len = 1;        // time of every calc()
int cnt_loops = 3000;        // real loop times
double calc_time = 1;

int mpi_size, mpi_rank, mpi_size_per_node;
double start_time, start_time2, end_time;
struct Timestamp
{
    double before_calc, before_comm, before_PMPI, after_comm;
} local_time[MAX_LOOPS];
int cnt_iter;

double calc_result[MAX_LOOP_LEN];
double recv_buffer[MAX_LOOP_LEN];
//std::minstd_rand rand_engine;

int calc_smallop()
{
    int x = 1, y = 2, z = 3;
    for (int i = 0; i < 20000; ++i)
    {
        z = x * y;
        x = y * z;
        y = x * z;

    }
    return x + y + z;
}

int calc_op()// cost 0.6s
{
    int t;
    int sum = 0;
    for (int i = 0; i < 20 * calc_time / 0.6; ++i)
    {
        t = calc_smallop();
        sum += t;
    }
    return sum;
}

int v[1000000], p[1000000];
const int c_count = 1000000;

void calc()
{
    calc_result[0] = 0;
    for (int i = 0; i < loop_len; ++i)
    {
        //calc_result[j]+=int(calc_result[j-1]+int(sin(sin(rand_engine())))*10000)%123123;
        calc_result[i] = calc_op();
    }
    for (int i = 0; i < c_count; ++i)
    {
        v[i] = rand() + p[i];
    }
}

constexpr int send_cnt = 6553600;

int a[send_cnt];

void comm()
{
    if (mpi_rank < mpi_size / 2) //send
    {
//        fprintf(stderr, "rk=%d st=%d\n", mpi_rank, mpi_rank + mpi_size / 2);
        MPI_Send(a, send_cnt, MPI_INT, mpi_rank + mpi_size / 2, 0, MPI_COMM_WORLD);
    } else
    { //recv
        MPI_Status status;
//        fprintf(stderr, "rk=%d st=%d\n", mpi_rank, mpi_rank - mpi_size / 2);
        MPI_Recv(a, send_cnt, MPI_INT, mpi_rank - mpi_size / 2, 0, MPI_COMM_WORLD, &status);
    }
}


void collective_comm()
{
    MPI_Allreduce(v, p, c_count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "argc != 2");
        return -1;
    }
    MPI_Init(nullptr, nullptr);
    loop_len = atoi(argv[1]);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
//    if (mpi_rank == 0)
//    {
//        puts("start");
//    }
    double tt[4], s[4] = {};
    for (int i = 0; i < cnt_loops; ++i)
    {
        tt[0] = MPI_Wtime();
        calc();
        tt[1] = MPI_Wtime();
//            puts("start comm");
        comm();
        tt[2] = MPI_Wtime();
//            puts("start ccomm");
        collective_comm();
        tt[3] = MPI_Wtime();
        s[0] += tt[1] - tt[0];
        s[1] += tt[2] - tt[1];
        s[2] += tt[3] - tt[2];
        if (mpi_rank == 0 && (i & (512 - 1)) == 0)
        {
            printf("loop %d\n", i);
        }
    }
    if (mpi_rank==100000)
        printf("%lf", v[0]);

    MPI_Finalize();
//    fprintf(stderr, "Rank %d finalized.\n", mpi_rank);
    if (mpi_rank == 0)
    for (int i = 0; i < 3; ++i)
    {
        printf("%lf ", s[i] / cnt_loops);
        puts("");
    }
    return 0;
}

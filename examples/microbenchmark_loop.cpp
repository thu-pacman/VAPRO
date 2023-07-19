#include <iostream>
#include <cmath>
#include <mpi.h>
#include <cassert>
#include <random>
// gettimeofday
#include<sys/time.h>
#include<unistd.h>

//enum communication
//{
//    Chain,
//};
const int MAX_LOOP_LEN=100000000;	// maximum of send_size
const int MAX_LOOPS=1000000;		// MAX LOOPS LIMIMT
int send_size=100000000;	// comm time
int loop_len=1;		// time of every calc()
//int cnt_loops=1000;		// real loop times
int cnt_loops=100000;		// real loop times
double calc_time=1;

int mpi_size,mpi_rank,mpi_size_per_node;
double start_time,start_time2,end_time;
struct Timestamp
{
    double before_calc, before_comm, before_PMPI, after_comm;
}local_time[MAX_LOOPS];
int cnt_iter;

double calc_result[MAX_LOOP_LEN];
double recv_buffer[MAX_LOOP_LEN];
std::minstd_rand rand_engine;

//int calc_op() //{
//    int x=1,y=2,z=3;
//    for (int i=0;i<100000;++i)
//    {
//        z=x*y;
//        x=y*z;
//        y=x*z;
//    }
//    return x+y+z;
//}

static __inline__ unsigned long long rdtsc(void)
{
    //unsigned hi, lo;
    //__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    //return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
	struct  timeval end;
	unsigned  long diff;
	gettimeofday(&end,NULL);
	diff = 1000000 * (end.tv_sec)+ end.tv_usec;
	return diff;
}
unsigned long long last_time;

int calc_op()// cost 0.6s
{
    int t;
    int sum=0;
//    for (int i=0;i<20*calc_time/0.6;++i)
    for (int i=0;i<2*calc_time/0.6;++i)
    {
        int x=1,y=2,z=3;
        for (int i=0;i<200;++i)
//            for (int i=0;i<20000;++i)
        {
            z=x*y;
            x=y*z;
            y=x*z;
                                        
        }
        t=x+y+z;
        sum+=t;
    }
    return sum;
}


void calc()
{
    calc_result[0]=0;
    for (int i = 0; i < loop_len; ++i)
    {
            //calc_result[j]+=int(calc_result[j-1]+int(sin(sin(rand_engine())))*10000)%123123;
        calc_result[i]=calc_op();
    }
}

int main(int argc, char* argv[])
{
    if (argc!=2)
    {
        fprintf(stderr,"argc != 2");
        return -1;
    }
    MPI_Init(nullptr, nullptr);
    //MPI_Barrier(MPI_COMM_WORLD);

    loop_len=atoi(argv[1]);
    //double send_time=atof(argv[1]);
    //loop_len=atoi(argv[2]);
//    calc_time=atof(argv[2]);
//    cnt_loops=atoi(argv[3]);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
//    mpi_size_per_node=mpi_size/2;
//    MPI_Barrier(MPI_COMM_WORLD);
//    start_time=MPI_Wtime();
//    MPI_Barrier(MPI_COMM_WORLD);
//    start_time2=MPI_Wtime();

    auto f=fopen("time.out","w");
    for (int i=0;i<cnt_loops;++i)
    {
//    fprintf(stderr,"start calc %d\n",i);//DEBUG
        local_time[i].before_calc=MPI_Wtime();
        last_time=rdtsc();
        calc();
//        fprintf(f,"%llu\n",rdtsc()-last_time);
//        local_time[i].before_comm=MPI_Wtime();
//        comm(i);
//        local_time[i].after_comm=MPI_Wtime();
//    fprintf(stderr,"finish calc %d\n",i);//DEBUG
    }
    fclose(f);

//    MPI_Barrier(MPI_COMM_WORLD);
//    end_time=MPI_Wtime();
//
//    MPI_Datatype mpi_timestamp;
//    int length[1]={4};
//    MPI_Aint disp[1]={0};
//    MPI_Datatype types[1]={MPI_DOUBLE};
//    MPI_Type_create_struct(1, length, disp, types, &mpi_timestamp);
//    MPI_Type_commit(&mpi_timestamp);
//    if (mpi_rank==0)
//    {
//        fprintf(stderr, "Wall time: %lf\n", end_time - start_time);
//        //fprintf(stdout, "Wall time: %lf\n", end_time - start_time);
//        fprintf(stderr, "3~Wall time: %lf\n", end_time - local_time[2].before_calc);
//    }
    MPI_Finalize();
    //fprintf(stderr, "Rank %d finalized.\n", mpi_rank);
    return 0;
}

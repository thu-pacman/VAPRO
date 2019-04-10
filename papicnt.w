// -*- c++ -*-
//
// world48
// Todd Gamblin, tgamblinWllnl.gov
//
// This file is an example of how to use wrap.py to fool an application
// into thinking it is running on a different communicator from
// MPI_Comm_world.
//
// This was originally intended to allow applications on Blue Gene/Q to
// run with 48 MPI processes per node, rather than just the power of 2
// that IBM provides settings for.  The MPI_Init wrapper here will
// split MPI_Comm_world into 2 groups: one for the first 48 out of every
// 64 ranks and one for the last 16.  The last 16 ranks of every 64 just
// call MPI_Finalize and exit normally inside of MPI_Init.  The rest of
// the ranks continue to execute the rest of the application, thinking
// that the world is only 1/4 as big as the real MPI_COMM_WORLD.
//
// To build:
//    wrap.py world48.w > world48.C
//    mpicc -c world48.C
//    ar cr libworld48.a world48.o
//    ranlib libworld48.a
//
// Link your application with libworld48.a, or build it as a shared lib
// and LD_PRELOAD it to try out this tool.
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#define UNW_LOCAL_ONLY
#include <libunwind.h> // libunwind-version backtrace
#include "../papi_wrap.h"

// This is a communicator that will contain the first 48 out of
// every 64 ranks in the applicatPAPI failed to read countersion.
//static MPI_Comm world48;
static int rank, size, fd;
static volatile int *addr;
static int *addr_nonvol;
static int cnt_send;

struct Timestamp
{
    double before_calc, before_comm, before_PMPI, after_comm;
};

// backtrace info
extern VertexType vertex_type;
extern void *backtrace_buffer[];
extern int trace_size;

inline void get_backtrace()
{
    if (vertex_type!=VertexType::Function)
        trace_size = unw_backtrace(backtrace_buffer, (vertex_type==VertexType::CallSite)?3:10);
}


// avoid compiler optimization
int foo(int a,int b, int c)
{
    return a+b+c;
}

// k=1000 cost about 2 us
int consumeTime(int k)
{
    int a[3] = {rand(),rand(),rand()};
    int t;
    for (int n = 0; n < k; n++)
    {
        t = a[2];
        a[2] = a[1] * a[0];
        a[0]=a[1];
        a[1]=t;
    }
    foo(a[0],a[1],a[2]);
    return a[0]*a[1]*a[2];
}



// MPI_Init does all the communicator setup
//
{{fn func MPI_Init}}{
    get_backtrace();
    papi_init();
    {{callfn}}
    fprintf(stderr,"Shared library loaded..\n");
    papi_update(1, {{fn_id}}, 0, 0, nullptr);
    PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
}{{endfn}}
// __attribute_noinline__ void papi_update
//(int suffix, int mpi_func, int count, int target, void *mpi_comm)

{{fn func MPI_Finalize}}{
    get_backtrace();
    papi_update(0, {{fn_id}}, 0, 0, nullptr);
    {{callfn}}
    //if (rank==0)
    print_graph(rank);

}{{endfn}}

{{fn func MPI_Send}}{
    get_backtrace();
    papi_update(0, {{fn_id}}, {{1}}, {{3}}, (void*){{5}});
    {{callfn}}
    papi_update(1, {{fn_id}}, {{1}}, {{3}}, (void*){{5}});
}{{endfn}}

{{fn func MPI_Recv}}{
    get_backtrace();
    papi_update(0, {{fn_id}}, {{1}}, {{3}}, (void*){{5}});
    {{callfn}}
    papi_update(1, {{fn_id}}, {{1}}, {{3}}, (void*){{5}});
}{{endfn}}

{{fn func MPI_Irecv}}{
    get_backtrace();
    papi_update(0, {{fn_id}}, {{1}}, {{3}}, (void*){{5}});
    {{callfn}}
    papi_update(1, {{fn_id}}, {{1}}, {{3}}, (void*){{5}});
}{{endfn}}

// This generates interceptors that will catch every MPI routine
// *except* MPI_Init.  The interceptors just make sure that if
// they are called with an argument of type MPI_Comm that has a
// value of MPI_COMM_WORLD, they switch it with world48.
{{fnall func MPI_Init MPI_Finalize MPI_Init_thread MPI_Initialized MPI_Send MPI_Recv MPI_Irecv}}{
    get_backtrace();
    papi_update(0, {{fn_id}}, 0, 0, nullptr);
    {{callfn}}
    papi_update(1, {{fn_id}}, 0, 0, nullptr);
    //if (rank==0)
    //    puts({{fn_id}}); //output MPI function name
}{{endfnall}}

#include <errno.h>
#include <fcntl.h>
#include <fcntl.h> /* For O_* constants */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/types.h>
#include <unistd.h>

#include <linux/module.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <sys/param.h>

#include "/home/jyh/1/VAPRO/3rd-party/dbg-macro/dbg.h"
#include "controller.h"

namespace vapro {
extern Controller controller;
}
using vapro::controller;

{{fn func MPI_Init}} {

    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}
    controller.leaveExternal(d1);
    controller.datastore.showdata();
    
}{{endfn}}

{{fn func MPI_Finalize}} {

    auto d1 = controller.enterExternal();
    controller.datastore.showdata();


    int id=0;
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();

    controller.savedata(id);
}{{endfn}}

{{fn func MPI_Send}} {
    printf("send\n");
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_Recv}} {
    printf("recv\n");
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_Irecv}} {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}
    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_File_read_at_all}} {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_File_write_at_all}} {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_Allreduce}} {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_Alltoall}} {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}
    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_File_read_at}} {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

{{fn func MPI_File_write_at}} {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    {{callfn}}

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}{{endfn}}

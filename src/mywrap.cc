
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _EXTERN_C_
#ifdef __cplusplus
#define _EXTERN_C_ extern "C"
#else /* __cplusplus */
#define _EXTERN_C_
#endif /* __cplusplus */
#endif /* _EXTERN_C_ */

#ifdef MPICH_HAS_C2F
_EXTERN_C_ void *MPIR_ToPointer(int);
#endif // MPICH_HAS_C2F

#ifdef PIC
/* For shared libraries, declare these weak and figure out which one was linked
   based on which init wrapper was called.  See mpi_init wrappers.  */
#pragma weak pmpi_init
#pragma weak PMPI_INIT
#pragma weak pmpi_init_
#pragma weak pmpi_init__
#endif /* PIC */

_EXTERN_C_ void pmpi_init(MPI_Fint *ierr);
_EXTERN_C_ void PMPI_INIT(MPI_Fint *ierr);
_EXTERN_C_ void pmpi_init_(MPI_Fint *ierr);
_EXTERN_C_ void pmpi_init__(MPI_Fint *ierr);

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

static int fortran_init = 0;
/* ================== C Wrappers for MPI_Init ================== */
_EXTERN_C_ int PMPI_Init(int *argc, char ***argv);
_EXTERN_C_ int MPI_Init(int *argc, char ***argv) { 
    int _wrap_py_return_val = 0;
 {

    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

        if (fortran_init) {
#ifdef PIC
        if (!PMPI_INIT && !pmpi_init && !pmpi_init_ && !pmpi_init__) {
            fprintf(stderr, "ERROR: Couldn't find fortran pmpi_init function.  Link against static library instead.\n");
            exit(1);
        }        switch (fortran_init) {
        case 1: PMPI_INIT(&_wrap_py_return_val);   break;
        case 2: pmpi_init(&_wrap_py_return_val);   break;
        case 3: pmpi_init_(&_wrap_py_return_val);  break;
        case 4: pmpi_init__(&_wrap_py_return_val); break;
        default:
            fprintf(stderr, "NO SUITABLE FORTRAN MPI_INIT BINDING\n");
            break;
        }
#else /* !PIC */
        pmpi_init_(&_wrap_py_return_val);
#endif /* !PIC */
    } else {
        _wrap_py_return_val = PMPI_Init(argc, argv);
    }

    controller.leaveExternal(d1);
    controller.datastore.showdata();
    
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_Init =============== */
static void MPI_Init_fortran_wrapper(MPI_Fint *ierr) { 
    int argc = 0;
    char ** argv = NULL;
    int _wrap_py_return_val = 0;
    _wrap_py_return_val = MPI_Init(&argc, &argv);
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_INIT(MPI_Fint *ierr) { 
    fortran_init = 1;
    MPI_Init_fortran_wrapper(ierr);
}

_EXTERN_C_ void mpi_init(MPI_Fint *ierr) { 
    fortran_init = 2;
    MPI_Init_fortran_wrapper(ierr);
}

_EXTERN_C_ void mpi_init_(MPI_Fint *ierr) { 
    fortran_init = 3;
    MPI_Init_fortran_wrapper(ierr);
}

_EXTERN_C_ void mpi_init__(MPI_Fint *ierr) { 
    fortran_init = 4;
    MPI_Init_fortran_wrapper(ierr);
}

/* ================= End Wrappers for MPI_Init ================= */




/* ================== C Wrappers for MPI_Finalize ================== */
_EXTERN_C_ int PMPI_Finalize();
_EXTERN_C_ int MPI_Finalize() { 
    int _wrap_py_return_val = 0;
 {

    auto d1 = controller.enterExternal();
    controller.datastore.showdata();


    int id=0;
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    _wrap_py_return_val = PMPI_Finalize();

    controller.leaveExternal(d1);
    controller.datastore.showdata();

    controller.savedata(id);
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_Finalize =============== */
static void MPI_Finalize_fortran_wrapper(MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
    _wrap_py_return_val = MPI_Finalize();
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_FINALIZE(MPI_Fint *ierr) { 
    MPI_Finalize_fortran_wrapper(ierr);
}

_EXTERN_C_ void mpi_finalize(MPI_Fint *ierr) { 
    MPI_Finalize_fortran_wrapper(ierr);
}

_EXTERN_C_ void mpi_finalize_(MPI_Fint *ierr) { 
    MPI_Finalize_fortran_wrapper(ierr);
}

_EXTERN_C_ void mpi_finalize__(MPI_Fint *ierr) { 
    MPI_Finalize_fortran_wrapper(ierr);
}

/* ================= End Wrappers for MPI_Finalize ================= */




/* ================== C Wrappers for MPI_Send ================== */
_EXTERN_C_ int PMPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
_EXTERN_C_ int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;
 {
    printf("send\n");
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_Send(buf, count, datatype, dest, tag, comm);

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_Send =============== */
static void MPI_Send_fortran_wrapper(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_Send((const void*)buf, *count, (MPI_Datatype)(*datatype), *dest, *tag, (MPI_Comm)(*comm));
#else /* MPI-2 safe call */
    _wrap_py_return_val = MPI_Send((const void*)buf, *count, MPI_Type_f2c(*datatype), *dest, *tag, MPI_Comm_f2c(*comm));
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_SEND(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Send_fortran_wrapper(buf, count, datatype, dest, tag, comm, ierr);
}

_EXTERN_C_ void mpi_send(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Send_fortran_wrapper(buf, count, datatype, dest, tag, comm, ierr);
}

_EXTERN_C_ void mpi_send_(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Send_fortran_wrapper(buf, count, datatype, dest, tag, comm, ierr);
}

_EXTERN_C_ void mpi_send__(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Send_fortran_wrapper(buf, count, datatype, dest, tag, comm, ierr);
}

/* ================= End Wrappers for MPI_Send ================= */




/* ================== C Wrappers for MPI_Recv ================== */
_EXTERN_C_ int PMPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
_EXTERN_C_ int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) { 
    int _wrap_py_return_val = 0;
 {
    printf("recv\n");
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_Recv(buf, count, datatype, source, tag, comm, status);

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_Recv =============== */
static void MPI_Recv_fortran_wrapper(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_Recv((void*)buf, *count, (MPI_Datatype)(*datatype), *source, *tag, (MPI_Comm)(*comm), (MPI_Status*)status);
#else /* MPI-2 safe call */
    MPI_Status temp_status;
    MPI_Status_f2c(status, &temp_status);
    _wrap_py_return_val = MPI_Recv((void*)buf, *count, MPI_Type_f2c(*datatype), *source, *tag, MPI_Comm_f2c(*comm), &temp_status);
    MPI_Status_c2f(&temp_status, status);
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_RECV(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_Recv_fortran_wrapper(buf, count, datatype, source, tag, comm, status, ierr);
}

_EXTERN_C_ void mpi_recv(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_Recv_fortran_wrapper(buf, count, datatype, source, tag, comm, status, ierr);
}

_EXTERN_C_ void mpi_recv_(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_Recv_fortran_wrapper(buf, count, datatype, source, tag, comm, status, ierr);
}

_EXTERN_C_ void mpi_recv__(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_Recv_fortran_wrapper(buf, count, datatype, source, tag, comm, status, ierr);
}

/* ================= End Wrappers for MPI_Recv ================= */




/* ================== C Wrappers for MPI_Irecv ================== */
_EXTERN_C_ int PMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;
 {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_Irecv =============== */
static void MPI_Irecv_fortran_wrapper(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_Irecv((void*)buf, *count, (MPI_Datatype)(*datatype), *source, *tag, (MPI_Comm)(*comm), (MPI_Request*)request);
#else /* MPI-2 safe call */
    MPI_Request temp_request;
    temp_request = MPI_Request_f2c(*request);
    _wrap_py_return_val = MPI_Irecv((void*)buf, *count, MPI_Type_f2c(*datatype), *source, *tag, MPI_Comm_f2c(*comm), &temp_request);
    *request = MPI_Request_c2f(temp_request);
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_IRECV(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierr) { 
    MPI_Irecv_fortran_wrapper(buf, count, datatype, source, tag, comm, request, ierr);
}

_EXTERN_C_ void mpi_irecv(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierr) { 
    MPI_Irecv_fortran_wrapper(buf, count, datatype, source, tag, comm, request, ierr);
}

_EXTERN_C_ void mpi_irecv_(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierr) { 
    MPI_Irecv_fortran_wrapper(buf, count, datatype, source, tag, comm, request, ierr);
}

_EXTERN_C_ void mpi_irecv__(MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *ierr) { 
    MPI_Irecv_fortran_wrapper(buf, count, datatype, source, tag, comm, request, ierr);
}

/* ================= End Wrappers for MPI_Irecv ================= */




/* ================== C Wrappers for MPI_File_read_at_all ================== */
_EXTERN_C_ int PMPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;
 {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_File_read_at_all(fh, offset, buf, count, datatype, status);

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_File_read_at_all =============== */
static void MPI_File_read_at_all_fortran_wrapper(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_File_read_at_all((MPI_File)(*fh), *offset, (void*)buf, *count, (MPI_Datatype)(*datatype), (MPI_Status*)status);
#else /* MPI-2 safe call */
    MPI_Status temp_status;
    MPI_Status_f2c(status, &temp_status);
    _wrap_py_return_val = MPI_File_read_at_all(MPI_File_f2c(*fh), *offset, (void*)buf, *count, MPI_Type_f2c(*datatype), &temp_status);
    MPI_Status_c2f(&temp_status, status);
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_FILE_READ_AT_ALL(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_read_at_all(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_read_at_all_(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_read_at_all__(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

/* ================= End Wrappers for MPI_File_read_at_all ================= */




/* ================== C Wrappers for MPI_File_write_at_all ================== */
_EXTERN_C_ int PMPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;
 {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_File_write_at_all(fh, offset, buf, count, datatype, status);

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_File_write_at_all =============== */
static void MPI_File_write_at_all_fortran_wrapper(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_File_write_at_all((MPI_File)(*fh), *offset, (const void*)buf, *count, (MPI_Datatype)(*datatype), (MPI_Status*)status);
#else /* MPI-2 safe call */
    MPI_Status temp_status;
    MPI_Status_f2c(status, &temp_status);
    _wrap_py_return_val = MPI_File_write_at_all(MPI_File_f2c(*fh), *offset, (const void*)buf, *count, MPI_Type_f2c(*datatype), &temp_status);
    MPI_Status_c2f(&temp_status, status);
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_FILE_WRITE_AT_ALL(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_write_at_all(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_write_at_all_(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_write_at_all__(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_all_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

/* ================= End Wrappers for MPI_File_write_at_all ================= */




/* ================== C Wrappers for MPI_Allreduce ================== */
_EXTERN_C_ int PMPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
_EXTERN_C_ int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;
 {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_Allreduce =============== */
static void MPI_Allreduce_fortran_wrapper(MPI_Fint *sendbuf, MPI_Fint *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_Allreduce((const void*)sendbuf, (void*)recvbuf, *count, (MPI_Datatype)(*datatype), (MPI_Op)(*op), (MPI_Comm)(*comm));
#else /* MPI-2 safe call */
    _wrap_py_return_val = MPI_Allreduce((const void*)sendbuf, (void*)recvbuf, *count, MPI_Type_f2c(*datatype), MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_ALLREDUCE(MPI_Fint *sendbuf, MPI_Fint *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Allreduce_fortran_wrapper(sendbuf, recvbuf, count, datatype, op, comm, ierr);
}

_EXTERN_C_ void mpi_allreduce(MPI_Fint *sendbuf, MPI_Fint *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Allreduce_fortran_wrapper(sendbuf, recvbuf, count, datatype, op, comm, ierr);
}

_EXTERN_C_ void mpi_allreduce_(MPI_Fint *sendbuf, MPI_Fint *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Allreduce_fortran_wrapper(sendbuf, recvbuf, count, datatype, op, comm, ierr);
}

_EXTERN_C_ void mpi_allreduce__(MPI_Fint *sendbuf, MPI_Fint *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Allreduce_fortran_wrapper(sendbuf, recvbuf, count, datatype, op, comm, ierr);
}

/* ================= End Wrappers for MPI_Allreduce ================= */




/* ================== C Wrappers for MPI_Alltoall ================== */
_EXTERN_C_ int PMPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;
 {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_Alltoall =============== */
static void MPI_Alltoall_fortran_wrapper(MPI_Fint *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_Alltoall((const void*)sendbuf, *sendcount, (MPI_Datatype)(*sendtype), (void*)recvbuf, *recvcount, (MPI_Datatype)(*recvtype), (MPI_Comm)(*comm));
#else /* MPI-2 safe call */
    _wrap_py_return_val = MPI_Alltoall((const void*)sendbuf, *sendcount, MPI_Type_f2c(*sendtype), (void*)recvbuf, *recvcount, MPI_Type_f2c(*recvtype), MPI_Comm_f2c(*comm));
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_ALLTOALL(MPI_Fint *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Alltoall_fortran_wrapper(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr);
}

_EXTERN_C_ void mpi_alltoall(MPI_Fint *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Alltoall_fortran_wrapper(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr);
}

_EXTERN_C_ void mpi_alltoall_(MPI_Fint *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Alltoall_fortran_wrapper(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr);
}

_EXTERN_C_ void mpi_alltoall__(MPI_Fint *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *comm, MPI_Fint *ierr) { 
    MPI_Alltoall_fortran_wrapper(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr);
}

/* ================= End Wrappers for MPI_Alltoall ================= */




/* ================== C Wrappers for MPI_File_read_at ================== */
_EXTERN_C_ int PMPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;
 {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_File_read_at(fh, offset, buf, count, datatype, status);

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_File_read_at =============== */
static void MPI_File_read_at_fortran_wrapper(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_File_read_at((MPI_File)(*fh), *offset, (void*)buf, *count, (MPI_Datatype)(*datatype), (MPI_Status*)status);
#else /* MPI-2 safe call */
    MPI_Status temp_status;
    MPI_Status_f2c(status, &temp_status);
    _wrap_py_return_val = MPI_File_read_at(MPI_File_f2c(*fh), *offset, (void*)buf, *count, MPI_Type_f2c(*datatype), &temp_status);
    MPI_Status_c2f(&temp_status, status);
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_FILE_READ_AT(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_read_at(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_read_at_(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_read_at__(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_read_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

/* ================= End Wrappers for MPI_File_read_at ================= */




/* ================== C Wrappers for MPI_File_write_at ================== */
_EXTERN_C_ int PMPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;
 {
    auto d1 = controller.enterExternal();
    controller.datastore.showdata();

    _wrap_py_return_val = PMPI_File_write_at(fh, offset, buf, count, datatype, status);

    controller.leaveExternal(d1);
    controller.datastore.showdata();
}    return _wrap_py_return_val;
}

/* =============== Fortran Wrappers for MPI_File_write_at =============== */
static void MPI_File_write_at_fortran_wrapper(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    int _wrap_py_return_val = 0;
#if (!defined(MPICH_HAS_C2F) && defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */
    _wrap_py_return_val = MPI_File_write_at((MPI_File)(*fh), *offset, (const void*)buf, *count, (MPI_Datatype)(*datatype), (MPI_Status*)status);
#else /* MPI-2 safe call */
    MPI_Status temp_status;
    MPI_Status_f2c(status, &temp_status);
    _wrap_py_return_val = MPI_File_write_at(MPI_File_f2c(*fh), *offset, (const void*)buf, *count, MPI_Type_f2c(*datatype), &temp_status);
    MPI_Status_c2f(&temp_status, status);
#endif /* MPICH test */
    *ierr = _wrap_py_return_val;
}

_EXTERN_C_ void MPI_FILE_WRITE_AT(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_write_at(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_write_at_(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

_EXTERN_C_ void mpi_file_write_at__(MPI_Fint *fh, MPI_Fint *offset, MPI_Fint *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *status, MPI_Fint *ierr) { 
    MPI_File_write_at_fortran_wrapper(fh, offset, buf, count, datatype, status, ierr);
}

/* ================= End Wrappers for MPI_File_write_at ================= */




//
// Created by eric on 19-1-19.
//

#ifndef SC19_TEST_H
#define SC19_TEST_H
#include <cstring>
#include <stdint.h>

const double eps = 1e-10;

// PAPI constants
constexpr int CNT_EVENTS = 2;
const int I_PAPI_TOT_INS=0;
const int I_PAPI_TOT_CYC = I_PAPI_TOT_INS + 1;
const int I_PAPI_SR_INS = I_PAPI_TOT_CYC + 1;



struct Comm_key
{
    int target, mpi_count;

    friend bool operator < (const Comm_key &a, const Comm_key &b)
    {
        if (a.target!=b.target)
            return a.target < b.target;
        else
            return (a.mpi_count < b.mpi_count);
    }
};

struct DataType
{
    long long papi[CNT_EVENTS];
    unsigned long long elapsed, timestamp;
    // MPI function only
    int target, mpi_func, mpi_count;
    void* mpi_comm;

    void set_papi_data(long long data[])
    {
        memcpy(papi, data, sizeof(uint64_t) * CNT_EVENTS);
    }

    Comm_key to_comm_key() const
    {
        Comm_key comm_key{};
        comm_key.target = target;
        comm_key.mpi_count = mpi_count;
        return comm_key;
    }
};

enum VertexType {CallSite=0, CallPath = 1, Function = 2};

void papi_init();
void papi_update(int suffix);
//void papi_update(int suffix, int function_number, int count);
void papi_update(int suffix, int mpi_func, int count, int target, void *mpi_comm);
void print_graph(int rank);

#endif //SC19_TEST_H

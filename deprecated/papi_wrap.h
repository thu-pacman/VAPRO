//
// Created by eric on 19-1-19.
//

#ifndef SC19_TEST_H
#define SC19_TEST_H
#include <cstring>
#include <stdint.h>
#include <utility>
#include <vector>
#include <map>

#ifndef CPU_FREQ
#warning Use default TSC frequency
#define CPU_FREQ 2.3e9 // todo: 2.3 or 2.4?
#endif
const double eps = 1e-10;

// PAPI constants
//constexpr int CNT_PAPI_EVENTS = 1;
constexpr int CNT_PAPI_EVENTS = 10;
constexpr int CNT_RUSAGE_EVENTS = 4;
constexpr int CNT_TOTAL_EVENTS = CNT_PAPI_EVENTS + CNT_RUSAGE_EVENTS;
const int I_PAPI_TOT_INS=0; // Index of TOT_INS for workload analysis 


extern int mpi_size, mpi_rank;

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
    long long papi[CNT_TOTAL_EVENTS];
    unsigned long long elapsed; // elapsed time of this code snippet
    unsigned long long timestamp; // rdtsc timestamp
    // MPI function only
    int target, mpi_func, mpi_count;
    void* mpi_comm;

    void set_papi_data(unsigned long long data[])
    {
        memcpy(papi, data, sizeof(uint64_t) * CNT_PAPI_EVENTS);
    }

    void set_rusage_data(long long data[])
    {
        memcpy(papi + CNT_PAPI_EVENTS, data, sizeof(uint64_t) * CNT_RUSAGE_EVENTS);
    }

    Comm_key to_comm_key() const
    {
        Comm_key comm_key{};
        comm_key.target = target;
        comm_key.mpi_count = mpi_count;
        return comm_key;
    }
};

struct OnlineCommType
{
    unsigned long begin_addr, end_addr;
    double mean_cycles, min_time;
};

enum VertexType {CallSite=0, CallPath = 1, Function = 2};

typedef unsigned long long ULL;
//DataType accumulate_data;
typedef std::pair<void*,void*> GraphKey;
typedef std::vector<DataType> GraphValue;
typedef std::map<GraphKey, GraphValue> Graph;

void papi_init();
void papi_update(int suffix);
//void papi_update(int suffix, int function_number, int count);
void papi_update(int suffix, int mpi_func, int count, int target, void *mpi_comm);
void print_graph(int rank);
bool path_data_adequate(const GraphValue &path_data);
bool path_stable(const GraphValue &path_data);
void online_generate_latest_period(ULL current_tsc, ULL last_online_tsc);

#endif //SC19_TEST_H

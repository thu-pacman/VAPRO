//
// Created by eric on 19-7-9.
//

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <zconf.h>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <papi.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <json/json.h>
#include "papi_wrap.h"
#include "clustering.h"

using namespace std;

MPI_Datatype mpi_online_type;


OnlineCommType online_calc_snippet_summary(const GraphKey &k, const vector<DataType> &v)
{
    double send_cnt = 0, min_time = 1e100;

    for (const auto &vv:v)
    {
        min_time = min(min_time, vv.elapsed / CPU_FREQ);
        send_cnt = vv.mpi_count;
    }

    return OnlineCommType{(unsigned long) k.first, (unsigned long) k.second,
                          send_cnt, min_time};
}

/**
 * Online analyze to get mean and best performance value
 * @param graph
 * @param time_l
 * @param time_r
 * @return
 */
vector<OnlineCommType> online_get_comm_data(const Graph &graph, int time_l, int time_r)
{
    vector<OnlineCommType> ret;
    for (auto &kv : graph)
    {
        vector<vector<DataType>> classified = calc_classify(kv.second, 0.02);
//        vector<vector<DataType>> classified = calc_classify_fake(kv.second, 0.02);
        for (auto &v:classified)
        {
            if (path_data_adequate(v) && path_stable(v))
            {
                ret.emplace_back(online_calc_snippet_summary(kv.first, v));
            }
        }
    }
    return ret;
}

const int MaxOnlineBuffer = 1000;
OnlineCommType *recv_buffer;
MPI_Request *isend_request;
MPI_Request *irecv_request;
bool *recv_status;
int *recv_cnts;
// GraphKey, [(mean_cycles0, min_time0), ]
map<GraphKey, vector<pair<double, double> > > interprocess_info;


bool online_inited = false;
void online_anaylze_init()
{
    recv_buffer=new OnlineCommType[mpi_size*MaxOnlineBuffer];
    assert(recv_buffer!=nullptr);
    isend_request = new MPI_Request[mpi_size];
    irecv_request = new MPI_Request[mpi_size];
    recv_status=new bool[mpi_size];
    recv_cnts = new int[mpi_size];
//    fprintf(stderr, "DEBUG: s_req r_req=%x %x\n", isend_request, irecv_request);


    // create type
/* Setup description of the 4 MPI_FLOAT fields x, y, z, velocity */
    int blockcounts[2];
    MPI_Datatype oldtypes[2];
    MPI_Aint offsets[2], extent;
    offsets[0] = 0;
    oldtypes[0] = MPI_UNSIGNED_LONG;
    blockcounts[0] = 2;

/* Setup description of the 2 MPI_INT fields n, type */
/* Need to first figure offset by getting size of MPI_FLOAT */
    PMPI_Type_extent(oldtypes[0], &extent);
    offsets[1] = 2 * extent;
    oldtypes[1] = MPI_DOUBLE;
    blockcounts[1] = 2;

/* Now define structured type and commit it */
    PMPI_Type_struct(2, blockcounts, offsets, oldtypes, &mpi_online_type);
    PMPI_Type_commit(&mpi_online_type);

    online_inited = true;
}

void online_isend_to_zero(Graph &graph, int time_l, int time_r)
{
    auto data = online_get_comm_data(graph, time_l, time_r);

    PMPI_Isend(data.data(), data.size(), mpi_online_type, 0, 111111, MPI_COMM_WORLD,
              isend_request + 0);
}

bool online_irecv_all()
{
    assert(online_inited);
    for (int i = 0; i < mpi_size; ++i)
    {
        PMPI_Irecv(recv_buffer + i * MaxOnlineBuffer, MaxOnlineBuffer,
                  mpi_online_type, i, 111111, MPI_COMM_WORLD, irecv_request + i);
    }
}

void
online_isend_all(const Graph &calc_graph, const Graph &comm_graph,
                 const Graph &io_graph, int time_l, int time_r)
{

    auto data = online_get_comm_data(calc_graph, time_l, time_r);
    auto comm_data = online_get_comm_data(comm_graph, time_l, time_r);
    auto io_data = online_get_comm_data(io_graph, time_l, time_r);
    data.insert(data.end(), comm_data.begin(), comm_data.end());
    data.insert(data.end(), io_data.begin(), io_data.end());

    // DEBUG
//    if (mpi_rank<2)

//    printf("DES: r=%d io_graph=%lu io_data size=%lu\n", mpi_rank,
//           io_graph.size(), io_data.size());
//    for (const auto &v:io_data)
//    {
//        printf("DE: r=%d dst=%lx,%lx mc=%lf mt=%lf\n", mpi_rank, v.begin_addr,
//               v.end_addr, v.mean_cycles, v.min_time);
//    }


    for (int i = 0; i < mpi_size; ++i)
    {
        //TODO: limit send size
        PMPI_Isend(data.data(), min(MaxOnlineBuffer, (int)data.size()),
                   mpi_online_type, i, 111111,
                   MPI_COMM_WORLD, isend_request + i);
    }
}
//
//void online_finish_all_irecv()
//{
//    int flag;
//    MPI_Status status;
//    for (int i = 0; i < mpi_size; ++i)
//    {
//        MPI_Test(irecv_request + i, &flag, &status);
//        recv_status[i]=flag;
//        if (!flag)
//        {
//            MPI_Cancel(irecv_request);
//        }
//    }
//}

void online_finish_all_requests(MPI_Request *requests, bool isRecv = 0)
{
    int flag;
    MPI_Status status;

    int finished_cnt = 0;
    if (isRecv)
        memset(recv_status, 0, mpi_size);

    for (int i = 0; i < mpi_size; ++i)
    {
        PMPI_Test(requests + i, &flag, &status);
        if (isRecv)
        {
            recv_status[i] = flag;
            if (flag)
                PMPI_Get_count(&status, mpi_online_type, recv_cnts + i);
            else
                recv_cnts[i] = 0;
        }
        if (!flag)
        {
            PMPI_Cancel(requests + i);
        } else
            ++finished_cnt;
        PMPI_Wait(requests+ i, MPI_STATUS_IGNORE);
    }
//    fprintf(stderr, "INFO: recv from %d/%d processes\n", finished_cnt, mpi_size);
//    for (int i = 0; i < mpi_size; ++i)
//    {
//        if (!recv_status[i])
//    }
}

/**
 * Finish online communication in last iteration and generate report
 * @param current_tsc
 * @param last_online_tsc
 */
void online_analyze(ULL current_tsc, ULL last_online_tsc)
{
    online_finish_all_requests(isend_request);
//    fprintf(stderr, "DEBUG0: final isend rank=%3d\n", mpi_rank);
    online_finish_all_requests(irecv_request, 1);
//    fprintf(stderr, "DEBUG1: final irecv rank=%3d\n", mpi_rank);
    for (int i = 0; i < mpi_size; ++i)
    {
        for (int j = 0; j < recv_cnts[i]; ++j)
        {
            const OnlineCommType &item = recv_buffer[i * MaxOnlineBuffer + j];
            const GraphKey key = make_pair(
                    (void *)item.begin_addr, (void *)item.end_addr);
            if (!interprocess_info.count(key))
            {
                interprocess_info[key]=vector<pair<double, double> >();
            }
            interprocess_info[key].emplace_back(item.mean_cycles,
                                                item.min_time);
        }
    }
    online_generate_latest_period(current_tsc, last_online_tsc);
}

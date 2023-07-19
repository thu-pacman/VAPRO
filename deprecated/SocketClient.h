//
// Created by zheng on 2020/4/8.
//

#ifndef SC19_SOCKETCLIENT_H
#define SC19_SOCKETCLIENT_H

#include "papi_wrap.h"
const int MAXBUFSIZ = 2*1024*1024;
using SGraphValue = DataType;

class SocketClient
{
public:
    SocketClient(char *addr, int port);
    int serialize(int rank, unsigned long begin, unsigned long end, const Graph& graph_calc,
            const Graph& graph_comm, const Graph& graph_io);
    int send();
private:
    char addr[100];
    int port;
    int sockfd;
    unsigned int msg_size;
    char buf[MAXBUFSIZ];

    int connect_to_server();
};

struct Header{
    int rank;
    int msg_size;
    unsigned long begin, end;
    int n_keys;
};

struct SGraphKey{
    void *first,*second;
    int num;
};

//struct SGraphValue{
//    long long papi[CNT_PAPI_EVENTS];
//    unsigned long long elapsed; // elapsed time of this code snippet
//    unsigned long long timestamp; // rdtsc timestamp
//    // MPI function only
//    int target, mpi_func, mpi_count;
//    void* mpi_comm;
//
//    void load_from_DataType(const DataType& graphValue);
//};

#endif //SC19_SOCKETCLIENT_H

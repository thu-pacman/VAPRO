#include "controller.h"
#include "common.h"
#include <asm/ptrace.h>
#include <execinfo.h>
#include <papi.h>
#include <string.h>
#define MAX_DEPTH 20

namespace vapro {

DataVec diff(DataVec d1, DataVec d2) {
    DataVec d3;

    if (d1.size() != d2.size()) {
        printf("error");
    } else {
        int size = d1.size();
        for (int i = 0; i < size; i++) {
            d3.push_back(d2[i] - d1[i]);
        }
    }
    return d3;
}

Controller::Controller() {
    collectors.emplace_back(std::make_unique<CollectorPapi>());
}

DataVec Controller::readData() {
    DataVec dtvc;
    // std::unique_ptr<CollectorPapi> c = std::make_unique<CollectorPapi>();
    // for (auto &c : collectors) {
    DataVec data = collectors[0]->readData();
    //dbg(data[0]);
    //dbg(data[1]);
    //dbg(data[2]);
    dtvc.push_back(data[0]);
    dtvc.push_back(data[1]);
    dtvc.push_back(data[2]);
    collectors[0]->data = dtvc;
    datas.push_back(dtvc);
    times.push_back(collectors[0]->time);
    // collectors.emplace_back(std::move(c));
    return dtvc;
}

DataVec Controller::enterExternal() {
    // TODO:
    // 1. read data D1.
    // 2. get address
    // 3. If hasData, save the data into store. The data should be D1-D0.
    // 4. get the current time T0

    printf("begin enter\n");

    DataVec datavec = readData();
    void *buffer[MAX_DEPTH];
    int depth = backtrace(buffer, MAX_DEPTH);
    char **func_names = backtrace_symbols(buffer, depth);
    for (int i = 0; i < depth; i++) {
        //printf("Depth: %d, func name: %s ,address:%p\n", i, func_names[i],buffer[i]);
    }
    char *stop;
    unsigned long long add = (unsigned long long)buffer[0];
    // auto add = std::strtol(buffer[0], &stop, 16);
    printf("add:%lld\n", add);
    StoreKey key(true, add);



    int csize = datas.size();

    if (csize > 1) {
        datastore.insert(key, diff(datas[csize - 2], datas[csize - 1]));
    }

    //times.push_back(collectors[0]->time);
    return datavec;
}

DataVec Controller::leaveExternal(DataVec workload) {
    printf("begin leave\n");
    // TODO:
    // 1. get the current time T1
    // 2. store performacne data of the external functions into store.
    // <time T1-T0, workload>
    // 3. save current performance data D0.
    DataVec datavec;
    datavec = readData();
    datas.push_back(datavec);
    //times.push_back(collectors[0]->time);
    int csize = datas.size();
    if (times.size() >1&&csize > 1) {
        StoreKey key(false, times[times.size() - 1] - times[times.size() - 2]);
        datastore.insert(key, diff(datas[csize - 2], datas[csize - 1]));
    }
    return datavec;
}

} // namespace vapro

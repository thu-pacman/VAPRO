#include "controller.h"
using namespace vapro;

int main() {
    Controller controller;
    // 完成CollectorPapi.cc中的TODO。利用PAPI采集PAPI_TOT_INS，即程序片段的总指令数。如果遇到采集出来为0，或者出错，可能和系统的权限相关，可以来问下我。

    // This readData function will print the performance data
    controller.readData();
    // TODO: add some workload here for profiling
    controller.readData();
    // TOOD: compare the whether the printed data matches your expectation
}

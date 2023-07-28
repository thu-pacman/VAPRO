#include "controller.h"
#include "stdio.h"
#include <papi.h>
using namespace vapro;


void workload()
{
    int load=0;
    for(int i=0;i<10000;i++)
    for(int j=0;j<i;j++)
    load+=(i+j);
}


int main() {

    printf("begin test\n");
    //test0();
    Controller controller;
    // 完成CollectorPapi.cc中的TODO。利用PAPI采集PAPI_TOT_INS，即程序片段的总指令数。如果遇到采集出来为0，或者出错，可能和系统的权限相关，可以来问下我。

    // This readData function will print the performance data
    controller.readData();
    // TODO: add some workload here for profiling

    workload();

    controller.readData();
    // TOOD: compare the whether the printed data matches your expectation
    

    printf("end test\n");
    return 0;
}
//在test终端打开
//source /opt/spack/share/spack/setup-env.sh
//spack load openmpi@4.1.5
//spack load papi
//cmake ..
//make
//./test_vapro


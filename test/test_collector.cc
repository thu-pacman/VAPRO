#include "controller.h"
#include "papi.h"
#include "stdio.h"
using namespace vapro;

void workload() {
    int load = 0;
    for (int i = 0; i < 10000; i++)
        for (int j = 0; j < i; j++)
            load += (i + j);
}
void workload_1() {
    int N = 1000;
    int x[N];
    for (int j = 0; j < N; j++) {
        x[j] = j;
    }
    for (int loop = 0; loop < 10; loop++) {
        for (int i = 0; i < N; i++) {
            x[i] = x[i] * x[i];
        }
    }
}
void workload_2() {
    int N = 1000;
    int x[N];
    for (int j = 0; j < N; j++) {
        x[j] = j;
    }
    for (int i = 0; i < N; i++) {
        for (int loop = 0; loop < 10; loop++) {
            x[i] = x[i] * x[i];
        }
    }
}

void result(DataVec d1,DataVec d2)
{
    printf("TOT_INS:%ld\n",d2[0]-d1[0]);
    printf("TOT_CM:%ld\n",d2[1]-d1[1]);
    printf("TOT_TIME:%ld\n",d2[2]-d1[2]);
}

int main() {

    printf("begin test\n");
    // test0();
    Controller controller;

    DataVec d1,d2,d3;
    // 完成CollectorPapi.cc中的TODO。利用PAPI采集PAPI_TOT_INS，即程序片段的总指令数。如果遇到采集出来为0，或者出错，可能和系统的权限相关，可以来问下我。

    // This readData function will print the performance data
    d1=controller.readData();
    // TODO: add some workload here for profiling

    workload_1();

    d2=controller.readData();

    result(d1,d2);
    // TOOD: compare the whether the printed data matches your expectation

    workload_2();

    d3=controller.readData();

    result(d2,d3);

    printf("end test\n");
    return 0;
}
// 在test终端打开
// source /opt/spack/share/spack/setup-env.sh
// spack load openmpi@4.1.5
// spack load papi
// cmake ..
// make
//./test_vapro

// source /opt/spack/share/spack/setup-env.sh;spack load openmpi@4.1.5;spack load papi;cmake ..;make;./test_vapro
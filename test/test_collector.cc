#include "controller.h"
#include "papi.h"
#include "stdio.h"
using namespace vapro;

void create_index(int *arr, int len) {
    int i, j, temp;
    for (i = 0; i < len; i++) {
        if (i % 2 == 0) {
            arr[i] = len - 1 - i;
        }
    }
    for (i = 0; i < len; i++) {
        if (i % 5 == 0) {
            arr[i] = len - 1 - i;
        }
    }
}
void workload(int *a, int *b, int n) {
    for (int i = 0; i < n; i++) {
        a[b[i]] = a[b[i]] * a[b[i]];
    }
}

void result(DataVec d1, DataVec d2) {
    printf("TOT_INS:%ld\n", d2[0] - d1[0]);
    printf("TOT_CM:%ld\n", d2[1] - d1[1]);
    printf("TOT_TIME:%ld\n", d2[2] - d1[2]);
}

int main() {

    printf("begin test\n");
    // test0();
    Controller controller;

    DataVec d1, d2, d3, d4, d5, d6;

    const int N = 1000000;

    int test_data[N];

    int test_index[N];

    for (int i = 0; i < N; i++) {
        test_data[i] = i;
        test_index[i] = i;
    }

    // 完成CollectorPapi.cc中的TODO。利用PAPI采集PAPI_TOT_INS，即程序片段的总指令数。如果遇到采集出来为0，或者出错，可能和系统的权限相关，可以来问下我。

    // This readData function will print the performance data
    d1 = controller.readData();
    // TODO: add some workload here for profiling
    workload(test_data, test_index, N);
    d2 = controller.readData();
    result(d1, d2);

    // TOOD: compare the whether the printed data matches your expectation

    d3 = controller.readData();
    workload(test_data, test_index, N);
    d4 = controller.readData();
    result(d3, d4);
    printf("%d %d %d %d %d %d %d %d\n", test_index[0], test_index[1],
           test_index[2], test_index[3], test_index[4], test_index[5],
           test_index[6], test_index[7]);
    for (int i = 0; i < N; i++) {
        test_data[i] = i;
        test_index[i] = i;
    }
    create_index(test_index, N);

    d5 = controller.readData();
    workload(test_data, test_index, N);
    d6 = controller.readData();
    result(d5, d6);
    printf("%d %d %d %d %d %d %d %d\n", test_index[0], test_index[1],
           test_index[2], test_index[3], test_index[4], test_index[5],
           test_index[6], test_index[7]);

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

// source /opt/spack/share/spack/setup-env.sh;spack load openmpi@4.1.5;spack
// load papi;cmake ..;make;./test_vapro
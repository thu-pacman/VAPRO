#include "controller.h"
#include "stdio.h"
#include <papi.h>
using namespace vapro;

void test()
{
    printf("1\n");
    int EventSet;
    int i, sum;
    long long int values[2], values1[2], values2[2];

    printf("2\n");
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    exit(-1);
    //这段代码中，PAPI_library_init 是 PAPI 库的初始化函数，它接受一个版本号作为参数。如果初始化成功，函数返回当前的版本号；否则，返回 -1，表示初始化失败。
    printf("3\n");
    EventSet = PAPI_NULL; 
    if (PAPI_create_eventset(&EventSet) != PAPI_OK)
    exit(-1);
    //具体来说，这段代码先定义了一个空事件集合 EventSet，并将其初始化为 PAPI_NULL。然后在 if 语句中，使用 PAPI_create_eventset 函数来尝试创建一个新的事件集合。如果创建成功，EventSet 将会存储这个新的事件集合。否则，如果创建失败，程序将会退出并返回 -1。
    printf("4\n");//在这之后退出
    if (PAPI_add_event(EventSet, PAPI_TOT_INS) != PAPI_OK)
    {
        printf("error\n");
        printf("%d\n",PAPI_add_event(EventSet, PAPI_TOT_INS));
        printf("%d\n",PAPI_OK);
        exit(-1);
    }
    //代码中的 if 语句检查 PAPI_add_event() 函数是否成功将 PAPI_TOT_INS 事件添加到 EventSet 事件集中。如果添加失败，那么程序将会退出并返回 -1。PAPI_add_event() 函数的返回值是 PAPI_OK（0）或者一个错误代码。

    if (PAPI_add_event(EventSet, PAPI_L1_DCM) != PAPI_OK)
    exit(-1);
    if (PAPI_start(EventSet) != PAPI_OK)
    exit(-1);//
    if (PAPI_read(EventSet, values1) != PAPI_OK)
    exit(-1);
    //begin
    for (i=0;i<10000;i++)
    sum+=i;
    
    if (PAPI_stop(EventSet, values2) != PAPI_OK)
    exit(-1);
    if (PAPI_cleanup_eventset(EventSet) != PAPI_OK)
    exit(-1);
    if (PAPI_destroy_eventset(&EventSet) != PAPI_OK)
    exit(-1);
    
    PAPI_shutdown();
    /* Get value */
    values[0]=values2[0]-values1[0];
    values[1]=values2[1]-values1[1];
    printf("TOT_INS:%lld\nL1_DCM: %lld\n",values[0], values[1]);
}

void test0()
{printf("0\n");}

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


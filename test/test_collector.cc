#include "controller.h"
#include "papi.h"
#include <cstdio>
#include <linux/kernel.h>
#include <execinfo.h>
#include <iostream>  
#include <string>  
#include <cstdio>  
#include <cstdlib>
using namespace std;

using namespace vapro;

void create_index(std::vector<int> &arr, int len) {
    int i;
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
void workload(std::vector<int> &a, std::vector<int> &b, int n) {
    for (int i = 0; i < n; i++) {
        a[b[i]] = a[b[i]] * a[b[i]];
    }
}

void result(DataVec d1, DataVec d2) {
    printf("TOT_INS:%ld\n", d2[0] - d1[0]);
    printf("TOT_CM:%ld\n", d2[1] - d1[1]);
    printf("TOT_TIME:%ld\n", d2[2] - d1[2]);
    printf("PAPI_TIME:%ld\n", d2[3] - d1[3]);
}

void func1(void);
void func2(void);
void func3(void);


void test_workload()
{
    func1();
}

void testController() {
    // TODO: the current test case should look like this
    // controller.EnterExternal()
    // Send()
    // controller.EnterExternal()

    // Computation workload

    // controller.EnterExternal()
    // Recv
    // std::cout
    // controller.EnterExternal()
}

void func1()
{
    printf("1");
    func2();
}

void func2()
{
    printf("2");
    func3();
}
void func3()
{
    printf("3\n");
}




int main() {

    printf("begin test\n");
    // TODO: the current test case should look like this

    // controller.EnterExternal()

    // Send()

    // controller.LeaveExternal();


    // Computation workload

    // controller.EnterExternal()

    // Recv()
    // controller.LeaveExternal();

    int a[100];
    for(int i=0;i<100;i++)
    {
        a[i]=i;
    }

    Controller controller;

    test_workload();

    auto d1=controller.enterExternal();
    
    controller.datastore.showdata();

    
    controller.leaveExternal(d1);

    controller.datastore.showdata();

    test_workload();
    for(int i=1;i<100;i++)
    {
        a[i]=a[i]+a[i-1];
    }


    auto d2=controller.enterExternal();

    controller.datastore.showdata();
    for(int i=90;i<100;i++)
    {
        std::cout << a[i];
    }
    std::cout << endl;

    controller.leaveExternal(d2);

    controller.datastore.showdata();

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

// source /opt/spack/share/spack/setup-env.sh;spack load /awa3vt5
// spack load papi;cmake ..;make;./test_vapro

// spack load openmpi@4.1.5;
// python wrap.py -f -o mywrap.cc mywrap.w
// g++ -shared -fPIC warpper.cpp -o warpper.so
// export LD_PRELOAD=/home/jyh/1/VAPRO/src/warpper.so
// unset LD_PRELOAD

// python wrap.py -f -o mywrap.cc mywrap.w
// g++ -shared -fPIC mywrap.cc -o mywrap.so
// 
// export LD_PRELOAD=/home/jyh/1/VAPRO/build/libvapro.so
// mpic++ mpi.o -o mpi;mpic++ -c mpi.cpp -o mpi.o
// mpirun -np 4 ./mpi
// unset LD_PRELOAD

// export LD_PRELOAD=/home/jyh/1/VAPRO/build/libvapro.so
// mpic++ -c example_mpi_program.cpp -o example.o;mpic++ example.o -o example;
// mpirun -np 4 ./example 5 5 5
// unset LD_PRELOAD
// jyh@nico0:~/1/VAPRO$ python3 -m venv --without-pip myvapro
// jyh@nico0:~/1/VAPRO$ source myvapro/bin/activate
// mpirun -np 4 ./example 9 9 9

// source /opt/spack/share/spack/setup-env.sh;spack load /awa3vt5;spack load papi
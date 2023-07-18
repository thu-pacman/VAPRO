//
// Created by zheng on 2020/7/29.
//

#ifndef SC19_PMU_H
#define SC19_PMU_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <syscall.h>
#include <unistd.h>
//#include <fcntl-linux.h>
#include <fcntl.h>
#include <vector>

// There is currently no glibc wrapper, so you have to call it as syscall
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

// high-performance clock
static __inline__ unsigned long long rdtsc()
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
    //------------------------------------------
    // gettimeofday function
    //struct  timeval end;
    //unsigned  long diff;
    //gettimeofday(&end,NULL);
    //diff = 1000000 * (end.tv_sec)+ end.tv_usec;
    //return diff;
}

// rdpmc_instructions uses a "fixed-function" performance counter to return the count of retired instructions on
//       the current core in the low-order 48 bits of an unsigned 64-bit integer.
inline uint64_t rdpmc_instructions()
{
    uint32_t a, d, c;
    c = (1<<30);
    __asm__ volatile("rdpmc" : "=a" (a), "=d" (d) : "c" (c));
    return ((uint64_t)a) | (((uint64_t)d) << 32);;
}

//const int PackagesNum = 2;
//const int ChaPerCore = 18;
//int msr_fd[2];				// one for each socket
//
//// open fd to read snoop filter counters.
//// Root privilege and msr system module are required.
//void open_cha_counters()
//{
//    char filename[100];
//    uint64_t  msr_val;
//    int nr_cpus = sysconf(_SC_NPROCESSORS_ONLN);
//    int proc_in_pkg[8];			// one Logical Processor number for each socket
//    proc_in_pkg[0] = 0;                 // logical processor 0 is in socket 0 in all TACC systems
//    proc_in_pkg[1] = nr_cpus-1;         // logical processor N-1 is in socket 1 in all TACC 2-socket systems
//    for (int pkg=0; pkg<PackagesNum; pkg++) {
//        sprintf(filename,"/dev/cpu/%d/msr",proc_in_pkg[pkg]);
//        msr_fd[pkg] = open(filename, O_RDWR);
//        if (msr_fd[pkg] == -1) {
//            fprintf(stderr,"ERROR %s when trying to open %s\n",strerror(errno),filename);
//            exit(-1);
//        }
//    }
//
//    pread(msr_fd[0],&msr_val,sizeof(msr_val),0x186);
////    printf("Core PerfEvtSel0 0x%lx\n",msr_val);
//    if (msr_val != 0x40073d) {
//        fprintf(stderr,"ERROR: CHA counter 0 is not correctly configured. Current setting: 0x%lx\n", msr_val);
//        exit(-1);
//    }
//    pread(msr_fd[0],&msr_val,sizeof(msr_val),0x187);
//    if (msr_val != 0x400334) {
//        fprintf(stderr,"ERROR: CHA counter 1 is not correctly configured. Current setting: 0x%lx\n", msr_val);
//        exit(-1);
//    }
//    pread(msr_fd[0],&msr_val,sizeof(msr_val),0x188);
//    if (msr_val != 0x400534) {
//        fprintf(stderr,"ERROR: CHA counter 2 is not correctly configured. Current setting: 0x%lx\n", msr_val);
//        exit(-1);
//    }
//    pread(msr_fd[0],&msr_val,sizeof(msr_val),0x189);
//    if (msr_val != 0x40af37) {
//        fprintf(stderr,"ERROR: CHA counter 3 is not correctly configured. Current setting: 0x%lx\n", msr_val);
//        exit(-1);
//    }
//
//}
//
//// rdpmc_instructions uses a "/sys/" performance counter to return the count of retired instructions on
////       the current core in the low-order 48 bits of an unsigned 64-bit integer.
//inline std::vector<uint64_t> read_cha_counters()
//{
//    std::vector<uint64_t> ret;
//    uint64_t msr_val;
//    for (int package = 0; package < PackagesNum; ++package)
//    {
//        for (int tile = 0; tile < ChaPerCore; tile++)
//        {
//            for (int counter = 0; counter < 4; counter++)
//            {
//                uint64_t msr_num = 0xe00 + 0x10 * tile + 0x8 + counter;
//                if (pread(msr_fd[package], &msr_val, sizeof(msr_val), msr_num) == -1)
//                {
//                    fprintf(stderr, "ERROR %s when trying to read MSR %d %d %d\n", strerror(errno), package, tile, counter);
//                    exit(-1);
//                }
//                ret.push_back(msr_val);
//            }
//        }
//    }
//    return ret;
//}
std::vector<long long> get_rusage_data();

#endif //SC19_PMU_H

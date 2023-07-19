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
#include <linux/perf_event.h>
#include <sys/ioctl.h>
using namespace std;
using ULL = unsigned long long;

// There is currently no glibc wrapper, so you have to call it as syscall
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

// high-performance clock
static __inline__ unsigned long long rdtsc()
{
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

// rdpmc_instructions uses a "fixed-function" performance counter to return the count of retired instructions on
//       the current core in the low-order 48 bits of an unsigned 64-bit integer.
inline uint64_t rdpmc_instructions()
{
    uint32_t a, d, c;
    c = (1 << 30);
    __asm__ volatile("rdpmc"
                     : "=a"(a), "=d"(d)
                     : "c"(c));
    return ((uint64_t)a) | (((uint64_t)d) << 32);
    ;
}

void pmc_enable()
{
    int i;
    struct perf_event_attr attr;
    int pmcFd;
    int event[1] = {PERF_COUNT_HW_INSTRUCTIONS}; //, PERF_COUNT_HW_CPU_CYCLES, PERF_COUNT_HW_REF_CPU_CYCLES};

    for (i = 0; i < 1; ++i)
    {
        // Configure th event
        memset(&attr, 0, sizeof(struct perf_event_attr));
        attr.type = PERF_TYPE_HARDWARE;
        attr.size = sizeof(struct perf_event_attr);
        attr.config = event[i];
        attr.inherit = 1;

        // Due to the setting of attr.inherit, it will also count all child
        pmcFd = perf_event_open(&attr, 0, -1, -1, 0);
        if (pmcFd < 0)
            fprintf(stderr, "Failed to start [%d]\n", i);
        // Start counters
        ioctl(pmcFd, PERF_EVENT_IOC_ENABLE, 0);
    }
}

/**
 * This function has side effect. It changes last_time and last_totcycle.
 * @param cur_time
 * @return
 */
inline vector<ULL> pmc_get_data()
{
    vector<ULL> retv;
    retv.push_back(rdtsc());
    retv.push_back(rdpmc_instructions());
    return retv;
}

const char *e[] = {"TSC", "TOT_INST"};

int main()
{
    pmc_enable();
    auto p0 = pmc_get_data();
    double t = 1000000;
    for (int i = 0; i < 100000000; ++i)
    {
        t = sqrt(t);
    }
    auto p1 = pmc_get_data();
    for (int i = 0; i < p0.size(); ++i)
    {
        printf("%10s %llu\n", e[i], p1[i] - p0[i]);
    }
}
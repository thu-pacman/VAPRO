#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <syscall.h>
#include <unistd.h>
#include <vector>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <cassert>
#include "jevents/rdpmc.h"
#include "jevents/jevents.h"
#include "jevents/perf-iter.h"
using namespace std;
using ULL = unsigned long long;

// high-performance clock
static __inline__ unsigned long long rdtsc()
{
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

const int CNT_EVNET = 7;
const char *e[] = {"idq_uops_not_delivered.core",
                   "cpu_clk_unhalted.thread",
                   "uops_retired.retire_slots",
                   "uops_issued.any",
                   "int_misc.recovery_cycles",
                   "cpu_clk_unhalted.ref_tsc",
                   "cpu/instructions/"};

rdpmc_ctx ctx_rdpmc[CNT_EVNET];

void pmc_start()
{
    perf_event_attr attr;
    for (int i = 0; i < CNT_EVNET; ++i)
    {
        if (resolve_event(e[i], &attr) < 0)
            assert(!"No event");
        if (rdpmc_open_attr(&attr, ctx_rdpmc + i, (i == 0) ? nullptr : ctx_rdpmc + 0) < 0)
            assert(!"Cannot open rdpmc");
    }
}

vector<ULL> pmc_read()
{
    vector<ULL> ret;
    for (int i = 0; i < CNT_EVNET; ++i)
        ret.emplace_back(rdpmc_read(ctx_rdpmc + i));
    return ret;
}

int main()
{
    pmc_start();
    auto p0 = pmc_read();
    double t = 1000000;
    for (int i = 0; i < 100000000; ++i)
    {
        t = sqrt(t);
    }
    auto p1 = pmc_read();
    for (int i = 0; i < p0.size(); ++i)
    {
        printf("%10s %llu\n", e[i], p1[i] - p0[i]);
    }
}
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
#include "papi_wrap.h"
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

// const int CNT_EVNET = 5;
const char *e[CNT_PAPI_EVENTS] = {"cpu/instructions/",  // Fixed
                    "idq_uops_not_delivered.core",
                   "uops_retired.retire_slots",
                   "uops_issued.any",
                   "int_misc.recovery_cycles",
                   "cpu_clk_unhalted.thread",           // Fixed 
                   "cpu_clk_unhalted.ref_tsc",           // Fixed, For core utilization 
                   // Memory_Bound L3
                   "cycle_activity.stalls_ldm_pending",
                   "cycle_activity.stalls_l1d_pending",
                   "cycle_activity.stalls_l2_pending"
                   };

rdpmc_ctx ctx_rdpmc[CNT_PAPI_EVENTS];

void pmc_enable_real()
{
    assert(sizeof(e) / sizeof(*e) == CNT_PAPI_EVENTS);
    perf_event_attr attr;
    for (int i = 0; i < CNT_PAPI_EVENTS; ++i)
    {
        if (resolve_event(e[i], &attr) < 0) {
            fprintf(stderr, "Event %d %s\n", i, e[i]);
            assert(!"No event");
        }
        if (rdpmc_open_attr(&attr, ctx_rdpmc + i, (i == 0) ? nullptr : ctx_rdpmc + 0) < 0)
            assert(!"Cannot open rdpmc");
    }
}

vector<ULL> pmc_read_real()
{
    vector<ULL> ret;
    for (int i = 0; i < CNT_PAPI_EVENTS; ++i)
        ret.emplace_back(rdpmc_read(ctx_rdpmc + i));
    return ret;
}
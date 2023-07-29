#pragma once
#include <collector.h>

#define checkPapi(call)                                                        \
{                                                                          \
    auto err = call;                                                       \
    if (PAPI_OK != err) {                                                  \
        fprintf(stderr, "PAPI error in %s:%i : %s.\n", __FILE__, __LINE__, \
                PAPI_strerror(retval));                                    \
        exit(EXIT_FAILURE);                                                \
    }                                                                      \
}

static __inline__ unsigned long long rdtsc()
{
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

namespace vapro {

const string metricTotalInst("TOT_INST"), metricTSC("TSC");
class CollectorPapi : public Collector {

    MetricVec allMetrics = {metricTotalInst, metricTSC};
    int EventSet;

  public:
    CollectorPapi();
    virtual MetricVec getAvailableMetric() const { return allMetrics; }
    virtual MetricVec getEnabledMetric() const { return allMetrics; }
    virtual MetricVec setMetric() { IT_TODO_HALT(); }
    virtual DataVec readData();
};

} // namespace vapro

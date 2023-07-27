#pragma once
#include <collector.h>

namespace vapro {

const string metricTotalInst("TOT_INST"), metricTSC("TSC");
class CollectorPapi : public Collector {

    MetricVec allMetrics = {metricTotalInst, metricTSC};int EventSet;

  public:
    CollectorPapi();
    virtual MetricVec getAvailableMetric() const { return allMetrics; }
    virtual MetricVec getEnabledMetric() const { return allMetrics; }
    virtual MetricVec setMetric() { IT_TODO_HALT(); }
    virtual DataVec readData();
};

} // namespace vapro

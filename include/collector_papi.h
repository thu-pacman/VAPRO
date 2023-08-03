#pragma once
#include <collector.h>

namespace vapro {

const string metricTotalInst("TOT_INST"), metricTSC("TSC");

class CollectorPapi : public Collector {

    MetricVec allMetrics = {metricTotalInst, metricTSC};
    int EventSet;

  public:
    CollectorPapi();
    MetricVec getAvailableMetric() const override { return allMetrics; }
    MetricVec getEnabledMetric() const override { return allMetrics; }
    MetricVec setMetric() override { IT_TODO_HALT(); }
    DataVec readData() override;
    StoreKey getKey() override;
};

} // namespace vapro

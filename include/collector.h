#pragma once
#include <common.h>
#include <iostream>

namespace vapro {

using MetricVec = vector<string>;
using DataVec = vector<uint64_t>;

class Collector {
  public:
    // Collector(const Collector &) = delete;
    virtual ~Collector() {}

    virtual MetricVec getAvailableMetric() const = 0;
    virtual MetricVec getEnabledMetric() const = 0;
    virtual MetricVec setMetric() = 0;
    virtual DataVec readData() = 0;

  private:
};

} // namespace vapro

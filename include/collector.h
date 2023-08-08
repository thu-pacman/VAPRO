#pragma once
#include <common.h>
#include <iostream>

namespace vapro {

using MetricVec = vector<string>;
using DataVec = vector<uint64_t>;

class StoreKey {
    public:
    
    bool isComputation;
    uint64_t address;
    
    StoreKey(bool isc,uint64_t add):isComputation(isc),address(add){}

    friend bool operator <(const StoreKey a,const StoreKey b);


    //区分是否是计算，储存地址
};



class Collector {
  public:
    // Collector(const Collector &) = delete;
    virtual ~Collector() {}

    virtual MetricVec getAvailableMetric() const = 0;
    virtual MetricVec getEnabledMetric() const = 0;
    virtual MetricVec setMetric() = 0;
    virtual StoreKey getKey() = 0;
    virtual DataVec readData() = 0;

    Collector(){}
    DataVec data;

  private:
};

} // namespace vapro

#pragma once
#include "collector.h"
#include <common.h>

namespace vapro {

using StoreValue = vector<DataVec>;

class DataStore {
    map<StoreKey, DataVec> store;
    //性能数据

  public:
    void insert(const StoreKey &key, const DataVec &value);
    
    const StoreValue &get(const StoreKey &key);

    void showdata();

  private:
};

} // namespace vapro

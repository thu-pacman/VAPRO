#pragma once
#include "collector.h"
#include <common.h>

namespace vapro {

using StoreValue = vector<DataVec>;

class DataStore {
    map<StoreKey, StoreValue> store;

  public:
    void insert(const StoreKey &key, const DataVec &value);
    const StoreValue &get(const StoreKey &key);

  private:
};

} // namespace vapro

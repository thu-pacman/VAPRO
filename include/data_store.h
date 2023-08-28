#pragma once
#include "collector.h"
#include "common.h"

namespace vapro {

using StoreValue = vector<DataVec>;

class DataStore {
    public:
    map<StoreKey, StoreValue> store;
    //class StoreKey
    //private:
    //bool isComputation;
    //uint64_t address;

    void insert(const StoreKey &key, const DataVec &value);
    
    const StoreValue &get(const StoreKey &key);

    void showdata();

    private:
};

} // namespace vapro

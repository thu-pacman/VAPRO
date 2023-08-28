#pragma once
#include "collector.h"
#include "collector_papi.h"
#include "data_store.h"
#include "common.h"
#include "json/json.h"

namespace vapro {

class Controller {
  public:

    vector<unique_ptr<CollectorPapi>> collectors;
    vector<DataVec> datas;
    vector<unsigned long long> times;
    bool hasData = false;
    DataStore datastore;

    Json::Value root;
    Json::StyledWriter writer;
    Json::Value* person;

  public:
    Controller();

    DataVec readData();

    DataVec enterExternal();
        // TODO:
        // 1. read data D1.
        // 2. get address
        // 3. If hasData, save the data into store. The data should be D1-D0.
        // 4. get the current time T0
    DataVec leaveExternal(DataVec workload);
        // TODO:
        // 1. get the current time T1
        // 2. store performacne data of the external functions into store.
        // <time T1-T0, workload>
        // 3. save current performance data D0.
    void savedata(int id);
    
};

} // namespace vapro

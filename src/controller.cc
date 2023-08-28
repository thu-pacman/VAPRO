#include "controller.h"
#include "common.h"
#include <asm/ptrace.h>
#include <execinfo.h>
#include <papi.h>
#include <string.h>
#include "json/json.h"
#include <fstream>
#include <iostream>
constexpr int MAX_DEPTH = 100;


namespace vapro {

unsigned long long getaddress()
{
    void *buffer[MAX_DEPTH];
    int depth = backtrace(buffer, MAX_DEPTH);
    //char **func_names = backtrace_symbols(buffer, depth);
    char *stop;
    unsigned long long add = (unsigned long long)buffer[depth-1];
    return add;
}

DataVec diff(DataVec d1, DataVec d2) {
    DataVec d3;
    if (d1.size() != d2.size()) {
        printf("error");
    } 
    else {
        int size = d1.size();
        for (int i = 0; i < size; i++) {
            d3.push_back(d2[i] - d1[i]);
        }
    }
    return d3;
}

Controller::Controller() {
    collectors.emplace_back(std::make_unique<CollectorPapi>());
}

DataVec Controller::readData() {
    DataVec dtvc;
    DataVec data = collectors[0]->readData();
    //dtvc.push_back(data[0]);
    //dtvc.push_back(data[1]);
    //dtvc.push_back(data[2]);
    dtvc=data;
    collectors[0]->data = dtvc;
    datas.push_back(dtvc);
    times.push_back(collectors[0]->time);
    return dtvc;
}

DataVec Controller::enterExternal() {
    // TODO:
    // 1. read data D1.
    // 2. get address
    // 3. If hasData, save the data into store. The data should be D1-D0.
    // 4. get the current time T0

    printf("begin enter\n");
    DataVec datavec = readData();
    unsigned long long add=getaddress();
    printf("addresss:%lld\n", add);
    StoreKey key(true, add);
    int csize = datas.size();
    if (csize > 1) {
        datastore.insert(key, diff(datas[csize - 2], datas[csize - 1]));
    }
    return datavec;
}

DataVec Controller::leaveExternal(DataVec workload) {
    printf("begin leave\n");
    // TODO:
    // 1. get the current time T1
    // 2. store performacne data of the external functions into store.
    // <time T1-T0, workload>
    // 3. save current performance data D0.
    DataVec datavec;
    datavec = readData();
    datas.push_back(datavec);
    int csize = datas.size();
    if (times.size() > 1 && csize > 1) {
        StoreKey key(false, times[times.size() - 1] - times[times.size() - 2]);
        datastore.insert(key, diff(datas[csize - 2], datas[csize - 1]));
    }
    return datavec;
}

void Controller::savedata(int id)
{
    std::ofstream ofs;
    std::string filename = "data" + std::to_string(id) + ".json";  
    ofs.open(filename,std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        std::cout << "open file error!" << std::endl;
    }




    //save data
    int i=0;
    int s;
    int k;
    int datasize=datastore.store.size();
    person=new Json::Value[datasize];
    for (auto it = datastore.store.begin(); it!= datastore.store.end(); ++it) {
        person[i]["isComputation"]=(it->first).getisComputation();
        person[i]["address"]=(it->first).getaddress();
        k=0;

        for(auto j=(it->second).begin();j!=(it->second).end();j++)
        {
            auto name=std::to_string(k);
            for( s=0;s<j->size();s++)
            {
                person[i][name].append((it->second)[k][s]);
            }
            k++;
        }
        
        if(i>0){
            person[i]["time"]=((uint64_t)(times[i])-(uint64_t)(times[i-1]));
        }
        else {
            person[i]["time"]=(uint64_t)(0);
        }
        person[i]["id"]=id;
        root.append(person[i]);
        i++;
    }


    //io
    std::string json_file = writer.write(root);
    ofs << json_file;
    ofs.close();

    //info
    filename = "info.json";  
    ofs.open(filename,std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        std::cout << "open file error!" << std::endl;
    }
    Json::Value info;
    Json::Value inforoot;
    info["no.1"]="TOT_INS";
    info["no.2"]="L1_DCM";
    info["no.3"]="TIME";
    info["data type"]=s;
    info["data num"]=k;
    
    inforoot.append(info);
    json_file = writer.write(inforoot);
    ofs << json_file;
    ofs.close();
}

Controller controller;

} // namespace vapro

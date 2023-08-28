
#include "data_store.h"
using namespace std;
namespace vapro {
    void DataStore::insert(const StoreKey &key, const DataVec &value)
    {
        auto it=store.find(key);

        if (it!= store.end()) {
            //存在
            printf("exist %d\n",it->second.size());
            StoreValue newdata=(it->second);
            newdata.push_back(value);
            it->second=newdata;
            printf("saves %d\n",store.find(key)->second.size());
        } else {  
            //不存在
            StoreValue newdata;
            newdata.push_back(value);
            store.insert(make_pair(key,newdata));
        }
        
    }
    
    const StoreValue & DataStore::get(const StoreKey &key)
    {
        

    }

    void DataStore::showdata()
    {
        //printf("--------------------\n");
        //printf("show stored data:isdata & address(or time) & PAPI_TOT_INS(diff)\n");
        for (auto it = store.begin(); it!= store.end(); ++it) {
            //std::cout <<(it->first).getisComputation() << " -> "<< (it->first).getaddress() << " -> " << it->second[0] << std::endl;  
        }
        //printf("--------------------\n");
    }
}



       
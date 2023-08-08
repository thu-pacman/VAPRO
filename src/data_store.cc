
#include "data_store.h"
using namespace std;
namespace vapro {
    void DataStore::insert(const StoreKey &key, const DataVec &value)
    {
        store.insert(make_pair(key,value));
    }
    
    const StoreValue & DataStore::get(const StoreKey &key)
    {
        

    }

    void DataStore::showdata()
    {
        printf("\n");
        printf("--------------------\n");
        
        printf("show stored data:isdata & address(or time) & PAPI_TOT_INS(diff) & time(diff)\n");
        for (auto it = store.begin(); it!= store.end(); ++it) {  
            //printf("%lld!\n",it->first.address);
            std::cout <<it->first.isComputation << " -> "<< it->first.address << " -> " << it->second[0] << std::endl;  
        }
        
        printf("--------------------\n");
        printf("\n");


    }
}



       
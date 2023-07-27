#include "collector_papi.h"
#include <papi.h>
namespace vapro {

CollectorPapi::CollectorPapi() {
    // TODO: init the PAPI library. Refer to PAPI-reated functions in papi_init()
    // and papi_start_counters() in deprecated/pmc.cpp.
    /*
    */
    int retval =0;
    EventSet=PAPI_NULL;
    if (retval=PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT){
        fprintf(stderr, "-1,PAPI error %d: %s\n", retval, PAPI_strerror(retval));  
        exit(-1);  
    } 
    else
        printf("init success\n");

    if (retval =PAPI_create_eventset(&EventSet) != PAPI_OK){
        fprintf(stderr, "0,PAPI error %d: %s\n", retval, PAPI_strerror(retval));  
        exit(-1);  
    }
    if (retval =PAPI_add_event(EventSet, PAPI_TOT_INS) != PAPI_OK){
    fprintf(stderr, "1,PAPI error %d: %s\n", retval, PAPI_strerror(retval));  
    exit(-1);  
    }

    if (retval =PAPI_add_event(EventSet, PAPI_L1_DCM) != PAPI_OK){
    fprintf(stderr, "2,PAPI error %d: %s\n", retval, PAPI_strerror(retval));  
    exit(-1);  
    }

    if (retval = PAPI_start(EventSet)!= PAPI_OK) {  
    fprintf(stderr, "3,PAPI error %d: %s\n", retval, PAPI_strerror(retval));  
    exit(-1);  
    }
    
}

DataVec CollectorPapi::readData() {
    dbg("Invoked");
    // TODO: read data by the PAPI library. Refer to papi_get_data() in
    // deprecated/pmc.cpp.
    DataVec dtvc;
    //int EventSet=PAPI_NULL;
    long long int values[2], values1[2], values2[2];
    int retval=0;  
    /*
    if (retval=PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT){
    fprintf(stderr, "-1,PAPI error %d: %s\n", retval, PAPI_strerror(retval));  
    exit(-1);  
    }
    */
    if (retval = PAPI_read(EventSet, values2)!= PAPI_OK) {  
    fprintf(stderr, "4,PAPI error %d: %s\n", retval, PAPI_strerror(retval));  
    exit(-1);  
    }
    
    dtvc.emplace_back(values2[0]);
    return dtvc;
}

} // namespace vapro

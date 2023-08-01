#include "collector_papi.h"
#include <papi.h>

namespace vapro {
CollectorPapi::CollectorPapi() {
    int retval = 0;
    EventSet = PAPI_NULL;
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        printf("Error: PAPI_library_close failed with return value %d\n",
               retval);
    }
    checkPapi(PAPI_create_eventset(&EventSet));
    checkPapi(PAPI_add_event(EventSet, PAPI_TOT_INS));
    checkPapi(PAPI_add_event(EventSet, PAPI_L1_DCM));
    checkPapi(PAPI_start(EventSet));
}

DataVec CollectorPapi::readData() {
    // dbg("Invoked");
    //  TODO: read data by the PAPI library. Refer to papi_get_data() in
    //  deprecated/pmc.cpp.
    DataVec dtvc;
    long long int values[2];
    int retval = 0;
    if (retval = PAPI_read(EventSet, values) != PAPI_OK) {
        fprintf(stderr, "4,PAPI error %d: %s\n", retval, PAPI_strerror(retval));
        exit(-1);
    }

    dtvc.emplace_back(values[0]);
    dtvc.emplace_back(values[1]);
    dtvc.emplace_back(rdtsc());
    return dtvc;
}

} // namespace vapro

#include "collector_papi.h"
#include <papi.h>

#define checkPapi(call)                                                        \
    {                                                                          \
        auto err = call;                                                       \
        if (PAPI_OK != err) {                                                  \
            fprintf(stderr, "PAPI error in %s:%i : %s.\n", __FILE__, __LINE__, \
                    PAPI_strerror(retval));                                    \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    }



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
    DataVec datavec;
    long long int values[3];
    int retval = 0;
    checkPapi(PAPI_read(EventSet, values));

    datavec.emplace_back(values[0]);
    datavec.emplace_back(values[1]);
    datavec.emplace_back(values[2]);
    time=rdtsc();
    return datavec;
}

StoreKey CollectorPapi::getKey() {
    // TODO: get the callpath here
}

} // namespace vapro

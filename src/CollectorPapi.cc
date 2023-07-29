#include "collector_papi.h"
#include <papi.h>

#define checkPapi(call,stdpapi)                                                        \
    {                                                                          \
        auto err = call;                                                       \
        if (stdpapi != err) {                                                  \
            fprintf(stderr, "PAPI error in %s:%i : %s.\n", __FILE__, __LINE__, \
                    PAPI_strerror(retval));                                    \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    }

namespace vapro {
CollectorPapi::CollectorPapi() {
    int retval = 0;
    EventSet = PAPI_NULL;
    checkPapi(PAPI_library_init(PAPI_VER_CURRENT),PAPI_VER_CURRENT);
    checkPapi(PAPI_create_eventset(&EventSet),PAPI_OK);
    checkPapi(PAPI_add_event(EventSet, PAPI_TOT_INS),PAPI_OK);
    checkPapi(PAPI_add_event(EventSet, PAPI_L1_DCM),PAPI_OK);
    checkPapi(PAPI_start(EventSet),PAPI_OK);
}

DataVec CollectorPapi::readData() {
    dbg("Invoked");
    // TODO: read data by the PAPI library. Refer to papi_get_data() in
    // deprecated/pmc.cpp.
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

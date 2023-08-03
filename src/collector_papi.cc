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

static __inline__ unsigned long long rdtsc() {
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
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
    DataVec dtvc;
    long long int values[2];
    int retval = 0;
    checkPapi(PAPI_read(EventSet, values));

    dtvc.emplace_back(values[0]);
    dtvc.emplace_back(values[1]);
    dtvc.emplace_back(rdtsc());
    return dtvc;
}

StoreKey CollectorPapi::getKey() {
    // TODO: get the callpath here
}

} // namespace vapro

#include "collector_papi.h"

namespace vapro {

CollectorPapi::CollectorPapi() {
    // TODO: init the PAPI library. Refer to PAPI-reated functions in papi_init()
    // and papi_start_counters() in deprecated/pmc.cpp.
}

DataVec CollectorPapi::readData() {
    dbg("Invoked");
    // TODO: read data by the PAPI library. Refer to papi_get_data() in
    // deprecated/pmc.cpp.
}

} // namespace vapro

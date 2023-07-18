#include <vector>
#define USE_JEVENTS
// #define USE_ASSEMBLY_RDPMC
// #define USE_PAPI
#define USE_RUSAGE

#ifdef USE_JEVENTS

void pmc_enable_real();
std::vector<unsigned long long> pmc_read_real();

#endif
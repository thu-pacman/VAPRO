#include "collector.h"

namespace vapro {
    bool operator<(const StoreKey a, const StoreKey b) {
        if (a.address < b.address)
            return true;
        else
            return false;
    }
    bool StoreKey::getisComputation() const { return isComputation; }
    uint64_t StoreKey::getaddress() const { return address; }
}; // namespace vapro
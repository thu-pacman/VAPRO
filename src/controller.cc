#include "controller.h"

namespace vapro {

Controller::Controller() {
    collectors.emplace_back(std::make_unique<CollectorPapi>());
}

DataVec Controller::readData() {
    DataVec dtvc;
    for (auto &c : collectors) {
        auto data = c->readData();
        dbg(data[0]);
        dbg(data[1]);
        dbg(data[2]);
        dtvc.push_back(data[0]);
        dtvc.push_back(data[1]);
        dtvc.push_back(data[2]);
    }
    return dtvc;
}

} // namespace vapro

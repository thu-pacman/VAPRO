#include "controller.h"

namespace vapro {

Controller::Controller() {
    printf("construct\n");
    collectors.emplace_back(std::make_unique<CollectorPapi>());
}

void Controller::readData() {
    for (auto &c : collectors) {
        auto data = c->readData();
        dbg(data);
    }
}

} // namespace vapro

#pragma once
#include "collector.h"
#include "collector_papi.h"
#include <common.h>

namespace vapro {

class Controller {
  private:
    vector<unique_ptr<Collector>> collectors;

  public:
    Controller();

    void readData();
};

} // namespace vapro

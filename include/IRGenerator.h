#ifndef ARCVM_IRGENERATOR_H
#define ARCVM_IRGENERATOR_H

#include "Common.h"

#include <vector>

namespace arcvm {

// TODO make generating code from multiple threads more friendly
class IRGenerator {
  public:
    IRGenerator();

    Module* create_module();
    void link_modules();

  private:
    std::vector<Module*> modules_;
};

};
#endif
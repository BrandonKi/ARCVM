#ifndef ARCVM_PASS_MANAGER_H
#define ARCVM_PASS_MANAGER_H

#include <Common.h>
#include "Pass.h"

namespace arcvm {

template <Pass... Passes>
class PassManager {
  public:
    void module_pass(Module* module) {
        run_pass<Passes...>(module);
    }

  private:

        template <Pass P>
    void run_pass(Module* module) {
        P pass;
        pass.module_pass(module);
    }

    template <Pass P, Pass P2, Pass... Ps>
    void run_pass(Module* module) {
        P pass;
        pass.module_pass(module);
        run_pass<P2, Ps...>(module);
    }
};

};

#endif //ARCVM_PASS_MANAGER_H

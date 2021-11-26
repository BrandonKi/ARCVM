#ifndef ARCVM_PASS_H
#define ARCVM_PASS_H

// this is pretty much just a "interface" or template for a Pass

#include "Common.h"

namespace arcvm {

template <typename T>
concept Pass = requires(T t) {
    //{t.block_pass()};
    //{t.function_pass()};
    t.module_pass(static_cast<Module*>(nullptr));
};

}

#endif //ARCVM_PASS_H

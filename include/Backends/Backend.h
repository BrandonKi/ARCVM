#ifndef ARCVM_BACKEND_H
#define ARCVM_BACKEND_H

#include "Common.h"

namespace arcvm {

template <typename T>
concept Backend = requires(T t) {
    t.compile_module(static_cast<Module*>(nullptr));
};

}

#endif // ARCVM_BACKEND_H
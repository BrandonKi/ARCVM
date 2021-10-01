#include "Arcvm.h"

Arcvm::Arcvm(Args args) : args_{std::move(args)} {}

Arcvm::Arcvm(): args_{} {}

Arcvm::load_module(Module* module) {
    modules_.push_back(module);
    // there's probably some work I could be doing here
}

// run in interpret mode
Arcvm::run() {
    IRInterpreter
    for(auto* module: modules_) {

    }
}

// run in JIT mode
Arcvm::jit() {

}

// compile to binary, does not run
Arcvm::compile() {

}

// write to file
// assume compile() has been called
Arcvm::write_file() {

}

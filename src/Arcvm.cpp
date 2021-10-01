#include "Arcvm.h"

Arcvm::Arcvm(Args args) : args_{std::move(args)} {}

Arcvm::Arcvm(): args_{} {}

void Arcvm::load_module(Module* module) {
    modules_.push_back(module);
    // there's probably some work I could be doing here
}

// run in interpret mode
// TODO run all the modules in one interpreter context
// then just return the result of the entrypoint function
i32 Arcvm::run() {
    for(auto* module: modules_) {
       IRInterpreter interp(module);
       return interp.run();
    }
}

// run in JIT mode
i32 Arcvm::jit() {
    return 0;
}

// compile to binary, does not run
i32 Arcvm::compile() {
    return 0;
}

// write to file
// assume compile() has been called
void Arcvm::write_file() {

}

#include "Arcvm.h"

#include "Passes/PassManager.h"

using namespace arcvm;

Arcvm::Arcvm(Args args) : args_{std::move(args)} {}

Arcvm::Arcvm(): args_{} {}

void Arcvm::load_module(Module* module) {
    modules_.push_back(module);
    // there's probably some work I could be doing here
}

void Arcvm::optimize() {
    for(auto* module : modules_)
        optimize_module(module);
}

void Arcvm::optimize_module(Module* module) {
    PassManager<
        CFResolutionPass
    > pm;
    pm.module_pass(module);
}

// run in interpret mode
// TODO run all the modules in one interpreter context
// then just return the result of the entrypoint function
i32 Arcvm::run() {
    for(auto* module: modules_) {
       IRInterpreter interp(module);
       return interp.run();
    }
    return -1;
}

// run in JIT mode
i32 Arcvm::jit() {
    // FIXME assume windows_x64 for now
    x86_64_Backend b{x86_64::ABIType::windows_x64};
    b.compile_module(modules_[0]);
    return b.run();
}

// compile to binary, does not run
i32 Arcvm::compile() {
    // FIXME assume windows_x64 for now
    x86_64_Backend b{x86_64::ABIType::windows_x64};
    b.compile_module(modules_[0]);
    return 0;
}

// write to file
// assume compile() has been called
void Arcvm::write_file() {

}

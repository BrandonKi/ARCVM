#include "Passes/ConstantPropogation.h"

using namespace arcvm;

void ConstantPropogation::module_pass(Module* module) {
    for(auto* fn : module->functions) {
        process_function(fn);
    }
}

void ConstantPropogation::process_function(Function* function) {
    process_block(function->block);
}

void ConstantPropogation::process_block(Block* block) {
    for(auto* bblock : block->blocks) {
        for(auto* entry : bblock->entries) {
            // TODO I'm putting off implementing this because
            // it's not very important and provides minimal perf gain
        }
    }
}

#include "Passes/CFResolutionPass.h"

using namespace arcvm;

// step 1: remove dead unconditional br instructions (always happens)
// step 2: add explicit fallthrough jumps

// could use a CFG but these are so trivial is doesn't matter

static bool is_control_flow(Instruction inst) {
    ARCVM_PROFILE();
    switch(inst) {
        case Instruction::br:
        case Instruction::brz:
        case Instruction::brnz:
        case Instruction::call:
        case Instruction::ret:
            return true;
        default:
            return false;
    }
}

static bool is_terminating_control_flow(Instruction inst) {
    ARCVM_PROFILE();
    switch(inst) {
        case Instruction::br:
        case Instruction::brz:
        case Instruction::brnz:
        case Instruction::ret:
            return true;
        default:
            return false;
    }
}

void CFResolutionPass::module_pass(Module* module) {
    ARCVM_PROFILE();
    for(auto* fn : module->functions) {
        process_function(fn);
    }
}

void CFResolutionPass::process_function(Function* function) {
    ARCVM_PROFILE();
    process_block(function->block);
}

void CFResolutionPass::process_block(Block* block) {
    ARCVM_PROFILE();
    for(int i = 0; i < block->blocks.size(); ++i) {
        auto* current_block = block->blocks[i];
        BasicBlock* next_block = nullptr;
        if(i + 1 < block->blocks.size())
            next_block = block->blocks[i + 1];

        remove_dead_br(current_block);
        add_explicit_fallthrough(current_block, next_block);
    }
}

void CFResolutionPass::remove_dead_br(BasicBlock* bb) {
    ARCVM_PROFILE();
    if(bb->entries.size() <= 1) // not our job to handle this
        return;
    for(int i = 0; i < bb->entries.size(); ++i) {
        if(i + 1 == bb->entries.size() &&
               bb->entries[i]->instruction == Instruction::br &&
               bb->entries[i-1]->instruction == Instruction::ret) {
            bb->entries.erase(bb->entries.begin() + i);
        }
    }
}


void CFResolutionPass::add_explicit_fallthrough(BasicBlock* current, BasicBlock* next) {
    ARCVM_PROFILE();
    if(next && current->entries.empty()) {
        current->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string(next->label.name)}});
        return;
    }
    for(int i = 0; i < current->entries.size(); ++i) {
        if(i + 1 == current->entries.size() && !is_terminating_control_flow(current->entries[i]->instruction)) {
            current->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string(next->label.name)}});
        }
    }
}
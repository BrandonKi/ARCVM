// TODO
/*
#include "Passes/ConstantFold.h"

using namespace arcvm;

void ConstantFold::module_pass(Module* module) {
    for(auto* fn : module->functions) {
        process_function(fn);
    }
}

void ConstantFold::process_function(Function* function) {
    process_block(function->block);
}

void ConstantFold::process_block(Block* block) {
    for(auto* bblock : block->blocks) {
        for(auto* entry : bblock->entries) {
            switch (entry->instruction) {
                case Instruction::alloc: {
                    // TODO
                    break;
                }
                case Instruction::load: {
                    // TODO
                    break;
                }
                case Instruction::store: {
                    // TODO
                    break;
                }
                case Instruction::call: {
                    // TODO
                    break;
                }
                case Instruction::ret: {
                    // TODO
                    break;
                }
                case Instruction::br: {
                    // TODO
                    break;
                }
                case Instruction::brz: {
                    // TODO
                    break;
                }
                case Instruction::brnz: {
                    // TODO
                    break;
                }
                case Instruction::index: {
                    // TODO
                    break;
                }
                case Instruction::add: {
                    auto lhs = entry->arguments[0].value;
                    auto rhs = entry->arguments[1].value;
                    if(lhs.type != immediate || rhs.type != immediate)
                        break;
                    auto sum = lhs + rhs;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, sum};
                    break;
                }
                case Instruction::sub: {
                    // FIXME assumes we are using references
                    auto sum = ir_register.back()[entry->arguments[0].value].value -
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, sum};
                    break;
                }
                case Instruction::mul: {
                    // FIXME assumes we are using references
                    auto sum = ir_register.back()[entry->arguments[0].value].value *
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, sum};
                    break;
                }
                case Instruction::div: {
                    // FIXME assumes we are using references
                    auto sum = ir_register.back()[entry->arguments[0].value].value /
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, sum};
                    break;
                }
                case Instruction::mod: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value %
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::bin_or: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value |
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::bin_and: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value &
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::bin_xor: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value ^
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::lshift: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value <<
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::rshift: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value >>
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::lt: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value <
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::gt: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value >
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::lte: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value <=
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::gte: {
                    // FIXME assumes we are using references
                    auto result = ir_register.back()[entry->arguments[0].value].value >=
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::eq: {
                    auto result = ir_register.back()[entry->arguments[0].value].value ==
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::neq: {
                    auto result = ir_register.back()[entry->arguments[0].value].value !=
                        ir_register.back()[entry->arguments[1].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                case Instruction::neg: {
                    auto result = -ir_register.back()[entry->arguments[0].value].value;
                    ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                    break;
                }
                default:
                    assert(false);
            }
    }
}
*/
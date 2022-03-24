// TODO

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
        propogate_constants(bblock);
    }
}

void ConstantPropogation::propogate_constants(BasicBlock* bblock) {
    std::array<WrappedIRValue, 100> ir_registers;
    // for(auto* entry : bblock->entries) {
    for(int i = 0; i < bblock->entries.size(); ++i) {
        auto* entry = bblock->entries[i];
        for(int x = 0; x < entry->arguments.size(); ++x) {
            auto& arg = entry->arguments[x];
            if(!isImmediate(arg) && isReference(arg) && isConstant(ir_registers[arg.value])) {
                entry->arguments[x] = IRValue{IRValueType::immediate, ir_registers[entry->arguments[x].value].value.value};
            }
        }
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
            case Instruction::phi: {
                // TODO
                break;
            }
            case Instruction::dup: {
                auto arg = entry->arguments[0];
                if(isImmediate(arg))
                    ir_registers[entry->dest.value] = {arg, true};
                else if(isReference(arg) && isConstant(ir_registers[arg.value]))
                    ir_registers[entry->dest.value] = {ir_registers[arg.value].value, true};
                remove_entry(bblock->entries, i);
                i -= 1;
                break;
            }
            case Instruction::index: {
                // TODO
                break;
            }
            case Instruction::add: {
                auto lhs = entry->arguments[0];
                auto rhs = entry->arguments[1];
                i64 result;

                if(isImmediate(lhs) && isImmediate(rhs))
                    result = lhs.value + rhs.value;
                else if(isImmediate(rhs) && isConstant(ir_registers[lhs.value]))
                    result = ir_registers[lhs.value].value.value + rhs.value;
                else if(isReference(rhs) && isConstant(ir_registers[lhs.value]) && isConstant(ir_registers[rhs.value])) {
                    result = ir_registers[lhs.value].value.value + ir_registers[rhs.value].value.value;
                }
                else {
                    assert(false); // tbh if it gets here idk what went wrong
                }
                ir_registers[entry->dest.value] = WrappedIRValue{IRValue{IRValueType::immediate, result}, true};
                remove_entry(bblock->entries, i);
                i -= 1;
                // TODO should be safe just removing the entry, but keeping this here until I double check
                // replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                break;
            }
            /*case Instruction::sub: {
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
            }*/
            default:
                assert(false);
        }
    }
}
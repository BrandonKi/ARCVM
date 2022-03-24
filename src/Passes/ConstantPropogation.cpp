// TODO

#include "Passes/ConstantPropogation.h"

using namespace arcvm;

// TODO I stole this from IRInterpreter
// it should be moved into a function and cleaned up in general
// NOTE this version is slightly modified
#define CP_CAST_IF_TYPED_BIN_OP() do {                                             \
                                   if(entry->arguments.size() == 3)                \
                                       switch(entry->arguments[2].type_value) {    \
                                           case Type::ir_b1:                       \
                                           case Type::ir_b8:                       \
                                           case Type::ir_i8:                       \
                                               result = (i8)result;    \
                                               break;                              \
                                           case Type::ir_u8:                       \
                                               result = (u8)result;    \
                                               break;                              \
                                           case Type::ir_i16:                      \
                                               result = (i16)result;   \
                                               break;                              \
                                           case Type::ir_u16:                      \
                                               result = (u16)result;   \
                                               break;                              \
                                           case Type::ir_i32:                      \
                                               result = (i32)result;   \
                                               break;                              \
                                           case Type::ir_u32:                      \
                                               result = (u32)result;   \
                                               break;                              \
                                           case Type::ir_i64:                      \
                                               result = (i64)result;   \
                                               break;                              \
                                           case Type::ir_u64:                      \
                                               result = (u64)result;   \
                                               break;                              \
                                           default:                                \
                                               assert(false);                      \
                                       }                                           \
                               } while(false)    

#define CP_BIN_OP(op)                                                                                                   \
                    auto lhs = entry->arguments[0];                                                                     \
                    auto rhs = entry->arguments[1];                                                                     \
                    i64 result;                                                                                         \
                                                                                                                        \
                    if(isImmediate(lhs) && isImmediate(rhs))                                                            \
                        result = lhs.value op rhs.value;                                                                \
                    else {                                                                                              \
                        continue;                                                                                       \
                    }                                                                                                   \
                    CP_CAST_IF_TYPED_BIN_OP();                                                                          \
                    ir_registers[entry->dest.value] = WrappedIRValue{IRValue{IRValueType::immediate, result}, true};    \
                    remove_entry(bblock->entries, i);                                                                   \
                    i -= 1


void ConstantPropogation::module_pass(Module* module) {
    ARCVM_PROFILE();
    for(auto* fn : module->functions) {
        process_function(fn);
    }
}

void ConstantPropogation::process_function(Function* function) {
    ARCVM_PROFILE();
    process_block(function->block);
}

void ConstantPropogation::process_block(Block* block) {
    ARCVM_PROFILE();
    for(auto* bblock : block->blocks) {
        propogate_constants(bblock);
    }
}

void ConstantPropogation::propogate_constants(BasicBlock* bblock) {
    ARCVM_PROFILE();
    std::array<WrappedIRValue, 100> ir_registers;
    // for(auto* entry : bblock->entries) {
    for(int i = 0; i < bblock->entries.size(); ++i) {
        auto* entry = bblock->entries[i];
        for(int x = 0; x < entry->arguments.size(); ++x) {
            auto& arg = entry->arguments[x];
            if(isReference(arg) && isConstant(ir_registers[arg.value]) && isValidValue(ir_registers[arg.value])) {
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
                else
                    continue;
                remove_entry(bblock->entries, i);
                i -= 1;
                break;
            }
            case Instruction::index: {
                // TODO
                break;
            }
            case Instruction::add: {
                CP_BIN_OP(+);
                break;
            }
            case Instruction::sub: {
                CP_BIN_OP(-);
                break;
            }
            case Instruction::mul: {
                CP_BIN_OP(*);
                break;
            }
            case Instruction::div: {
                CP_BIN_OP(/);
                break;
            }
            case Instruction::mod: {
                CP_BIN_OP(%);
                break;
            }
            case Instruction::bin_or: {
                CP_BIN_OP(|);
                break;
            }
            case Instruction::bin_and: {
                CP_BIN_OP(&);
                break;
            }
            case Instruction::bin_xor: {
                CP_BIN_OP(^);
                break;
            }
            case Instruction::lshift: {
                CP_BIN_OP(<<);
                break;
            }
            case Instruction::rshift: {
                CP_BIN_OP(>>);
                break;
            }
            case Instruction::lt: {
                CP_BIN_OP(<);
                break;
            }
            case Instruction::gt: {
                CP_BIN_OP(>);
                break;
            }
            case Instruction::lte: {
                CP_BIN_OP(<=);
                break;
            }
            case Instruction::gte: {
                CP_BIN_OP(>=);
                break;
            }
            case Instruction::eq: {
                CP_BIN_OP(==);
                break;
            }
            case Instruction::neq: {
                CP_BIN_OP(-);
                break;
            }
            case Instruction::neg: {
                // auto result = -ir_register.back()[entry->arguments[0].value].value;
                // ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
                break;
            }
            default:
                assert(false);
        }
    }
}
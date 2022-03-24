// TODO

#include "Passes/ImmediateCanonicalization.h"

using namespace arcvm;

// TODO I stole this from IRInterpreter
// it should be moved into a function and cleaned up in general
#define IC_CAST_IF_TYPED_BIN_OP() do {                                             \
                                   if(entry->arguments.size() == 3)                \
                                       switch(entry->arguments[2].type_value) {    \
                                           case Type::ir_b1:                       \
                                           case Type::ir_b8:                       \
                                           case Type::ir_i8:                       \
                                               result.value = (i8)result.value;    \
                                               break;                              \
                                           case Type::ir_u8:                       \
                                               result.value = (u8)result.value;    \
                                               break;                              \
                                           case Type::ir_i16:                      \
                                               result.value = (i16)result.value;   \
                                               break;                              \
                                           case Type::ir_u16:                      \
                                               result.value = (u16)result.value;   \
                                               break;                              \
                                           case Type::ir_i32:                      \
                                               result.value = (i32)result.value;   \
                                               break;                              \
                                           case Type::ir_u32:                      \
                                               result.value = (u32)result.value;   \
                                               break;                              \
                                           case Type::ir_i64:                      \
                                               result.value = (i64)result.value;   \
                                               break;                              \
                                           case Type::ir_u64:                      \
                                               result.value = (u64)result.value;   \
                                               break;                              \
                                           default:                                \
                                               assert(false);                      \
                                       }                                           \
                               } while(false)                                      \


void ImmediateCanonicalization::module_pass(Module* module) {
    for(auto* fn : module->functions) {
        process_function(fn);
    }
}

void ImmediateCanonicalization::process_function(Function* function) {
    process_block(function->block);
}

void ImmediateCanonicalization::process_block(Block* block) {
    for(auto* bblock : block->blocks) {
        for(int i = 0; i < bblock->entries.size(); ++i) {
            auto* entry = bblock->entries[i];

            if(entry->arguments.size() <= 1)
                continue;

            // TODO only works for binary operations
            if(entry->arguments[0].type != IRValueType::immediate || entry->arguments[1].type != IRValueType::immediate)
                break;
            auto lhs = entry->arguments[0].value;
            auto rhs = entry->arguments[1].value;

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
                    auto result = IRValue{IRValueType::immediate, lhs + rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::sub: {
                    auto result = IRValue{IRValueType::immediate, lhs - rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::mul: {
                    auto result = IRValue{IRValueType::immediate, lhs * rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::div: {
                    auto result = IRValue{IRValueType::immediate, lhs / rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::mod: {
                    auto result = IRValue{IRValueType::immediate, lhs % rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::bin_or: {
                    auto result = IRValue{IRValueType::immediate, lhs | rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::bin_and: {
                    auto result = IRValue{IRValueType::immediate, lhs & rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::bin_xor: {
                    auto result = IRValue{IRValueType::immediate, lhs ^ rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::lshift: {
                    auto result = IRValue{IRValueType::immediate, lhs << rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::rshift: {
                    auto result = IRValue{IRValueType::immediate, lhs >> rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::lt: {
                    auto result = IRValue{IRValueType::immediate, lhs < rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::gt: {
                    auto result = IRValue{IRValueType::immediate, lhs > rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::lte: {
                    auto result = IRValue{IRValueType::immediate, lhs <= rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::gte: {
                    auto result = IRValue{IRValueType::immediate, lhs >= rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::eq: {
                    auto result = IRValue{IRValueType::immediate, lhs == rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::neq: {
                    auto result = IRValue{IRValueType::immediate, lhs != rhs};
                    IC_CAST_IF_TYPED_BIN_OP();
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                case Instruction::neg: {
                    auto result = IRValue{IRValueType::immediate, -lhs};
                    replace_entry(bblock->entries, i, Entry{IRValue{IRValueType::immediate, entry->dest.value}, Instruction::dup, {result}});
                    break;
                }
                default:
                    assert(false);
            }
        }
    }
}
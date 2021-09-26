#include "IRInterpreter.h"

IRInterpreter::IRInterpreter(Module* module)
    : module_{module}, jump_table{}, function_table{}, entrypoint_name{}, ir_register{} {}

i32 IRInterpreter::run() {
    ARCVM_PROFILE();
    return run_module(module_);
}

void IRInterpreter::build_jump_table(Module* module) {
    ARCVM_PROFILE();
    for (auto& function : module->functions) {
        for (auto const& attribute : function.attributes) {
            if (attribute == Attribute::entrypoint)
                entrypoint_name = function.name;
        }
        if (function.is_complete) {
            for (size_t i = 0; i < function.block.blocks.size(); ++i) {
                auto& basicblock = function.block.blocks[i];
                jump_table.emplace(basicblock.label.name, IRLocation{&function.block, i});
                function_table.emplace(basicblock.label.name, &function);
            }
        }
    }
}

i32 IRInterpreter::run_module(Module* module) {
    ARCVM_PROFILE();
    build_jump_table(module);
    return run_entry_function();
}

i32 IRInterpreter::run_entry_function() {
    ARCVM_PROFILE();
    Value ret_val = run_function(function_table.at(entrypoint_name));
    if (ret_val.type != ValueType::none)
        return static_cast<i32>(ret_val.value);
    return 0;
}

Value IRInterpreter::run_function(Function* function) {
    ARCVM_PROFILE();
    return run_block(&function->block);
}

Value IRInterpreter::run_block(Block* block) {
    ARCVM_PROFILE();
    for (size_t i = 0; i < block->blocks.size(); ++i) {
        auto ret_val = run_basicblock(&block->blocks[i]);
        if (ret_val.type != ValueType::none)
            return ret_val;
    }
    return Value{ValueType::none};
}

Value IRInterpreter::run_basicblock(BasicBlock* basicblock) {
    ARCVM_PROFILE();
    for (size_t i = 0; i < basicblock->entries.size(); ++i) {
        auto ret_val = run_entry(&basicblock->entries[i]);
        if (ret_val.type != ValueType::none)
            return ret_val;
    }
    return Value{ValueType::none};
}

// TODO there are a lot of common patterns here that can be extracted into macros
//
// FIXME this is purposely inefficient
// fix at some point
Value IRInterpreter::run_entry(Entry* entry) {
    // ARCVM_PROFILE();
    switch (entry->instruction) {
        case Instruction::alloc:
            {
                auto num_bytes = type_size(entry->arguments[0].type_value);
                ir_register[entry->dest.value] = Value(ValueType::pointer, malloc(num_bytes));
                break;
            }
        case Instruction::load:
            {
                // FIXME wow this workaround is hideous
                // not much I can do though
                auto load = [&]<std::integral T>(T t) {
                    auto* ptr = reinterpret_cast<T*>(ir_register[entry->arguments[0].value].pointer_value);
                    ir_register[entry->dest.value] = Value(ValueType::immediate, *ptr);
                };
                if(entry->arguments.size() > 1) {
                    switch(entry->arguments[1].type_value) {
                        case Type::ir_b1:
                        case Type::ir_b8:
                        case Type::ir_i8:
                        case Type::ir_u8:
                            load((i8)0);
                            break;
                        case Type::ir_i16:
                        case Type::ir_u16:
                            load((i16)0);
                            break;
                        case Type::ir_i32:
                        case Type::ir_u32:
                            load((i32)0);
                            break;
                        case Type::ir_i64:
                        case Type::ir_u64:
                            load((i64)0);
                            break;
                        default:
                            load((i64)0);
                    }
                }
                else {
                    load((i64)0);
                }
                break;
            }
        case Instruction::store:
            {
                // another hideous workaround :)
                auto store = [&]<std::integral T>(T t) {
                    if (entry->arguments[1].type == ValueType::immediate) {
                        auto* ptr = reinterpret_cast<T*>(ir_register[entry->arguments[0].value].pointer_value);
                        *ptr = static_cast<T>(entry->arguments[1].value);
                    } else if (entry->arguments[1].type == ValueType::reference) {
                        auto* ptr = reinterpret_cast<T*>(ir_register[entry->arguments[0].value].pointer_value);
                        *ptr = static_cast<T>(ir_register[entry->arguments[1].value].value);
                    }
                };

                if(entry->arguments.size() > 2) {
                    switch(entry->arguments[2].type_value) {
                        case Type::ir_b1:
                        case Type::ir_b8:
                        case Type::ir_i8:
                        case Type::ir_u8:
                            store((i8)0);
                            break;
                        case Type::ir_i16:
                        case Type::ir_u16:
                            store((i16)0);
                            break;
                        case Type::ir_i32:
                        case Type::ir_u32:
                            store((i32)0);
                            break;
                        case Type::ir_i64:
                        case Type::ir_u64:
                            store((i64)0);
                            break;
                        default:
                            store((i64)0);
                    }
                }
                else {
                    store((i64)0);
                }
                break;
            }
        case Instruction::call:
            break;
        case Instruction::ret:
            {
                return ir_register[entry->arguments[0].value];
            }
        case Instruction::index:
            {
                // TODO clean this up
                auto* ptr = reinterpret_cast<i8*>(ir_register[entry->arguments[0].value].pointer_value);
                if(entry->arguments[1].type == ValueType::immediate) {
                    ptr += entry->arguments[1].value;
                }
                else if(entry->arguments[1].type == ValueType::reference) {
                    ptr += ir_register[entry->arguments[1].value].value;
                }
                ir_register[entry->dest.value].pointer_value = (uintptr_t)ptr;
                break;
            }
        case Instruction::add:
            {
                // FIXME assumes we are using references
                auto sum = ir_register[entry->arguments[0].value].value +
                           ir_register[entry->arguments[1].value].value;
                ir_register[entry->dest.value] = Value{ValueType::reference, sum};
                break;
            }
        case Instruction::sub:
            {
                // FIXME assumes we are using references
                auto sum = ir_register[entry->arguments[0].value].value -
                           ir_register[entry->arguments[1].value].value;
                ir_register[entry->dest.value] = Value{ValueType::reference, sum};
                break;
            }
        case Instruction::mul:
            {
                // FIXME assumes we are using references
                auto sum = ir_register[entry->arguments[0].value].value *
                           ir_register[entry->arguments[1].value].value;
                ir_register[entry->dest.value] = Value{ValueType::reference, sum};
                break;
            }
        case Instruction::div:
            {
                // FIXME assumes we are using references
                auto sum = ir_register[entry->arguments[0].value].value /
                           ir_register[entry->arguments[1].value].value;
                ir_register[entry->dest.value] = Value{ValueType::reference, sum};
                break;
            }
        default:
            return Value{};
    }
    return Value{};
}
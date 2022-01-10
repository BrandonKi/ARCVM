#include "IRInterpreter.h"

using namespace arcvm;

IRInterpreter::IRInterpreter(Module* module)
    : module_{module}, jump_table{}, function_table{}, entrypoint_name{}, ir_register{} {}

i32 IRInterpreter::run() {
    ARCVM_PROFILE();
    return run_module(module_);
}

void IRInterpreter::build_jump_table(Module* module) {
    ARCVM_PROFILE();
    for (auto* function : module->functions) {
        for (auto const& attribute : function->attributes) {
            if (attribute == Attribute::entrypoint)
                entrypoint_name = function->name;
        }
        for (size_t i = 0; i < function->block->blocks.size(); ++i) {
            auto* basicblock = function->block->blocks[i];
            jump_table.emplace(basicblock->label.name, basicblock);
            function_table.emplace(basicblock->label.name, function);
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
    // TODO pass command line arguments here
    IRValue ret_val = run_function(function_table.at(entrypoint_name), {});
    if (ret_val.type != IRValueType::none)
        return static_cast<i32>(ret_val.value);
    return 0;
}

// TODO be able to pass args to functions
IRValue IRInterpreter::run_function(Function* function, std::vector<IRValue> args) {
    ARCVM_PROFILE();
    // put args in after adding new register context

    ir_register.emplace_back();
    for(size_t i = 0; i < args.size(); ++i) {
        // FIXME
        // assumes it's a reference to the calling context
        // in theory it could be from any previous context
        if(args[i].type == IRValueType::reference) {
            auto& calling_context = ir_register.end()[-2];
            ir_register.back()[i] = calling_context[args[i].value];
        }
        else    // assumes immediate
            ir_register.back()[i] = args[i];
    }
    auto result = run_block(function->block);
    ir_register.pop_back();
    return result;
}

IRValue IRInterpreter::run_block(Block* block) {
    ARCVM_PROFILE();
    return run_basicblock(block->blocks[0]);
}

IRValue IRInterpreter::run_basicblock(BasicBlock* basicblock) {
    ARCVM_PROFILE();
    for (size_t i = 0; i < basicblock->entries.size(); ++i) {
        auto ret_val = run_entry(basicblock->entries[i]);
        if (ret_val.type != IRValueType::none)
            return ret_val;
    }
    return IRValue{IRValueType::none};
}

// TODO there are a lot of common patterns here that can be extracted into macros
//
// FIXME this is purposely inefficient
// fix at some point
IRValue IRInterpreter::run_entry(Entry* entry) {
    // ARCVM_PROFILE();
    switch (entry->instruction) {
        case Instruction::alloc: {
            auto num_bytes = type_size(entry->arguments[0].type_value);
            ir_register.back()[entry->dest.value] = IRValue(IRValueType::pointer, malloc(num_bytes));
            break;
        }
        case Instruction::load: {
            // FIXME wow this workaround is hideous
            // not much I can do though
            auto load = [&]<std::integral T>(T t) {
                auto* ptr = reinterpret_cast<T*>(ir_register.back()[entry->arguments[0].value].pointer_value);
                ir_register.back()[entry->dest.value] = IRValue(IRValueType::immediate, *ptr);
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
        case Instruction::store: {
            // another hideous workaround :)
            auto store = [&]<std::integral T>(T t) {
                if (entry->arguments[1].type == IRValueType::immediate) {
                    auto* ptr = reinterpret_cast<T*>(ir_register.back()[entry->arguments[0].value].pointer_value);
                    *ptr = static_cast<T>(entry->arguments[1].value);
                } else if (entry->arguments[1].type == IRValueType::reference) {
                    auto* ptr = reinterpret_cast<T*>(ir_register.back()[entry->arguments[0].value].pointer_value);
                    *ptr = static_cast<T>(ir_register.back()[entry->arguments[1].value].value);
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
        case Instruction::call: {
            auto args = entry->arguments;
            // remove function name
            args.erase(args.begin(), args.begin()+1);
            // remove return type
            args.pop_back();
            ir_register.back()[entry->dest.value] = run_function(function_table.at(*(entry->arguments[0].str_value)), args);
            break;
        }
        case Instruction::ret: {
            if(entry->arguments[0].type == IRValueType::immediate)
                return entry->arguments[0].value;
            // assumes you are returning an index
            return ir_register.back()[entry->arguments[0].value];
        }
        case Instruction::br: {
            auto* label_name = entry->arguments[0].str_value;
            return run_basicblock(jump_table[*label_name]);
        }
        case Instruction::brz: {
            auto val = ir_register.back()[entry->arguments[0].value].value;
            auto* label_name = entry->arguments[1].str_value;
            auto* label_name2 = entry->arguments[2].str_value;
            if(val == 0)
                return run_basicblock(jump_table[*label_name]);
            else
                return run_basicblock(jump_table[*label_name2]);
        }
        case Instruction::brnz: {
            auto val = ir_register.back()[entry->arguments[0].value].value;
            auto* label_name = entry->arguments[1].str_value;
            auto* label_name2 = entry->arguments[2].str_value;
            if(val != 0)
                return run_basicblock(jump_table[*label_name]);
            else
                return run_basicblock(jump_table[*label_name2]);
        }
        case Instruction::phi: {
            assert(false);
            break;
        }
        case Instruction::dup: {
            ir_register.back()[entry->dest.value] = unpack(entry->arguments[0]);
            break;
        }
        case Instruction::index: {
            // TODO clean this up
            auto* ptr = reinterpret_cast<i8*>(ir_register.back()[entry->arguments[0].value].pointer_value);
            if(entry->arguments[1].type == IRValueType::immediate) {
                ptr += entry->arguments[1].value;
            }
            else if(entry->arguments[1].type == IRValueType::reference) {
                ptr += ir_register.back()[entry->arguments[1].value].value;
            }
            ir_register.back()[entry->dest.value].pointer_value = (uintptr_t)ptr;
            break;
        }
        case Instruction::add: {
            auto result = unpack(entry->arguments[0]) + unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::sub: {
            auto result = unpack(entry->arguments[0]) - unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::mul: {
            auto result = unpack(entry->arguments[0]) * unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::div: {
            auto result = unpack(entry->arguments[0]) / unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::mod: {
            auto result = unpack(entry->arguments[0]) % unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::bin_or: {
            auto result = unpack(entry->arguments[0]) | unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::bin_and: {
            auto result = unpack(entry->arguments[0]) & unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::bin_xor: {
            auto result = unpack(entry->arguments[0]) ^ unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::lshift: {
            auto result = unpack(entry->arguments[0]) << unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::rshift: {
            auto result = unpack(entry->arguments[0]) >> unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::lt: {
            auto result = unpack(entry->arguments[0]) < unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::gt: {
            auto result = unpack(entry->arguments[0]) > unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::lte: {
            auto result = unpack(entry->arguments[0]) <= unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::gte: {
            auto result = unpack(entry->arguments[0]) >= unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::eq: {
            auto result = unpack(entry->arguments[0]) == unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::neq: {
            auto result = unpack(entry->arguments[0]) != unpack(entry->arguments[1]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        case Instruction::neg: {
            auto result = -unpack(entry->arguments[0]);
            ir_register.back()[entry->dest.value] = IRValue{IRValueType::immediate, result};
            break;
        }
        default:
            assert(false);
            return IRValue{};
    }
    return IRValue{};
}

i64 IRInterpreter::unpack(IRValue value) {
    if(value.type == IRValueType::reference)
        return ir_register.back()[value.value].value;
    else if(value.type == IRValueType::immediate)
        return value.value;
    else
        assert(false);
    return -1;
}
#include "IRInterpreter.h"

IRInterpreter::IRInterpreter(Module* module)
    : module_{module}, jump_table{}, function_table{}, entrypoint_name{}, ir_register{} {}

i32 IRInterpreter::run() {
    return run_module(module_);
}

void IRInterpreter::build_jump_table(Module* module) {
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
    build_jump_table(module);
    return run_entry_function();
}

i32 IRInterpreter::run_entry_function() {
    Value ret_val = run_function(function_table.at(entrypoint_name));
    if (ret_val.type != ValueType::none)
        return static_cast<i32>(ret_val.value);
    return 0;
}

Value IRInterpreter::run_function(Function* function) {
    return run_block(&function->block);
}

Value IRInterpreter::run_block(Block* block) {
    for (size_t i = 0; i < block->blocks.size(); ++i) {
        auto ret_val = run_basicblock(&block->blocks[i]);
        if (ret_val.type != ValueType::none)
            return ret_val;
    }
    return Value{ValueType::none};
}

Value IRInterpreter::run_basicblock(BasicBlock* basicblock) {
    for (size_t i = 0; i < basicblock->entries.size(); ++i) {
        auto ret_val = run_entry(&basicblock->entries[i]);
        if (ret_val.type != ValueType::none)
            return ret_val;
    }
    return Value{ValueType::none};
}

// FIXME this is purposely inefficient
// fix at some point
Value IRInterpreter::run_entry(Entry* entry) {

    switch (entry->instruction) {
        case Instruction::alloc:
            {
                auto num_bytes = type_size(entry->arguments[0].type_value);
                ir_register[entry->dest.value] = Value(ValueType::pointer, malloc(num_bytes));
                break;
            }
        case Instruction::load:
            {
                auto* ptr = reinterpret_cast<i64*>(ir_register[entry->arguments[0].value].pointer_value);
                ir_register[entry->dest.value] = Value(ValueType::immediate, *ptr);
                break;
            }
        case Instruction::store:
            {
                if (entry->arguments[1].type == ValueType::immediate) {
                    auto* ptr = reinterpret_cast<i64*>(ir_register[entry->arguments[0].value].pointer_value);
                    *ptr = entry->arguments[1].value;
                } else if (entry->arguments[1].type == ValueType::reference) {
                    auto* ptr = reinterpret_cast<i64*>(ir_register[entry->arguments[0].value].pointer_value);
                    *ptr = ir_register[entry->arguments[1].value].value;
                }
                break;
            }
        case Instruction::call:
            break;
        case Instruction::ret:
            {
                return ir_register[entry->arguments[0].value];
            }
            break;
        case Instruction::index:
            break;
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
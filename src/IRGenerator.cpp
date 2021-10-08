#include "IRGenerator.h"

using namespace arcvm;

IRGenerator::IRGenerator() {}

// TODO use allocator
Module* IRGenerator::create_module() {
    return new Module{};
}

// FIXME URGENT this returns a pointer to memory owned by vector
// that means the pointer to it will get invalidated on resize
Function* Module::gen_function_def(std::string name, std::vector<Type> parameters, Type return_type) {
    ARCVM_PROFILE();
    auto func = Function{name, true, std::move(parameters), return_type, {}};
    auto* block = func.get_block();
    block->new_basic_block(name);
    functions.push_back(std::move(func));
    return &functions.back();
}

// Function* gen_aggregate_def(std::string, std::vector<Type>);

// TODO use allocator
// same issue as the function above :(
BasicBlock* Block::new_basic_block(std::string label_name) {
    ARCVM_PROFILE();
    blocks.emplace_back(std::move(label_name), std::vector<Entry>{}, var_name);
    return &(blocks.back());
}

// TODO implement this
Value Function::get_param(i32) {
    ARCVM_PROFILE();
    return Value{ValueType::none};
}

Value BasicBlock::gen_inst(Instruction instruction, Value value) {
    ARCVM_PROFILE();
    return gen_inst(instruction, std::vector{value});
}

Value BasicBlock::gen_inst(Instruction instruction, std::vector<Value> values) {
    ARCVM_PROFILE();
    switch(instruction) {
        case Instruction::add:
        case Instruction::sub:
        case Instruction::mul:
        case Instruction::div:
        case Instruction::mod:
        case Instruction::bin_or:
        case Instruction::bin_and:
        case Instruction::bin_xor:
        case Instruction::lshift:
        case Instruction::rshift:
        case Instruction::lt:
        case Instruction::gt:
        case Instruction::lte:
        case Instruction::gte:
        case Instruction::log_or:
        case Instruction::log_and:
        case Instruction::log_xor:
            entries.emplace_back(Value{ValueType::reference, var_name}, instruction, values);
            ++var_name;
            return entries.back().dest;
        case Instruction::branch:
            entries.emplace_back(Value{ValueType::none}, instruction, values);
            return entries.back().dest;
        case Instruction::index:
            entries.emplace_back(Value{ValueType::pointer, var_name}, instruction, values);
            ++var_name;
            return entries.back().dest;
        case Instruction::call:
            // FIXME return type is set to none
            entries.emplace_back(Value{ValueType::immediate, var_name}, instruction, values);
            ++var_name;
            return entries.back().dest;
        case Instruction::ret:
            entries.emplace_back(Value{ValueType::none}, instruction, values);
            return entries.back().dest;
        case Instruction::alloc:
            entries.emplace_back(Value{ValueType::pointer, var_name}, instruction, values);
            ++var_name;
            return entries.back().dest;
        case Instruction::load:
            entries.emplace_back(Value{ValueType::reference, var_name}, instruction, values);
            ++var_name;
            return entries.back().dest;
        case Instruction::store:
            entries.emplace_back(Value{ValueType::none}, instruction, values);
            return entries.back().dest;
        default:
            return Value{ValueType::none};
    }
}
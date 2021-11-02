#include "IRGenerator.h"

using namespace arcvm;

IRGenerator::IRGenerator() {}

// TODO use allocator
Module* IRGenerator::create_module() {
    return new Module{};
}

// TODO use allocator
Function* Module::gen_function_def(std::string name, std::vector<Type> parameters, Type return_type) {
    ARCVM_PROFILE();
    auto* func = new Function{name, std::move(parameters), return_type, {}};
    auto* block = func->get_block();
    block->new_basic_block(name);
    functions.push_back(func);
    return functions.back();
}

// Function* gen_aggregate_def(std::string, std::vector<Type>);

void Block::set_insertion_point(BasicBlock* bb) {
    for(i32 i = 0; i < blocks.size(); ++i)
        if(blocks[i] == bb)
            insertion_point = i;
}

void Block::set_insertion_point(std::string label) {
    for(i32 i = 0; i < blocks.size(); ++i)
        if(blocks[i]->label.name == label)
            insertion_point = i;
}

void Block::set_insertion_point(i32 new_point) {
    insertion_point = new_point;
}

BasicBlock* Block::new_basic_block() {
    ARCVM_PROFILE();
    auto* new_block = new_basic_block(std::to_string(label_name));
    ++label_name;
    return new_block;
}

// TODO use allocator
BasicBlock* Block::new_basic_block(std::string label_name) {
    ARCVM_PROFILE();
    auto* new_block = new BasicBlock(std::move(label_name), std::vector<Entry*>{}, var_name);
    ++insertion_point;
    if(blocks.empty())
        blocks.push_back(new_block);
    else
        blocks.insert(blocks.cbegin() + insertion_point, new_block);
    return new_block;
}

// make sure we are done writing to if_block and else_block
// this generates the final jump out of the block
void Block::gen_if(Value cond, BasicBlock* if_block, BasicBlock* else_block, BasicBlock* then_block) {
    ARCVM_PROFILE();
    auto* bblock = blocks[insertion_point];
    auto if_block_name = Value{ValueType::label, new std::string(if_block->label.name)};
    auto else_block_name = Value{ValueType::label, new std::string(else_block->label.name)};
    bblock->gen_inst(Instruction::brnz, {cond,if_block_name,else_block_name});
    auto then_block_name = Value{ValueType::label, new std::string(then_block->label.name)};
    if_block->gen_inst(Instruction::br, {then_block_name});
    else_block->gen_inst(Instruction::br, {then_block_name});
}

// TODO implement this
Value Function::get_param(i32 index) {
    ARCVM_PROFILE();
    return Value{ValueType::reference, index};
}

Value BasicBlock::gen_inst(Instruction instruction, Value value) {
    ARCVM_PROFILE();
    return gen_inst(instruction, std::vector{value});
}

// FIXME there is a lot of code duplication here
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
        case Instruction::eq:
        case Instruction::log_or:
        case Instruction::log_and:
        case Instruction::log_xor:
            entries.push_back(new Entry{Value{ValueType::reference, var_name}, instruction, values});
            ++var_name;
            return entries.back()->dest;
        case Instruction::index:
            entries.push_back(new Entry{Value{ValueType::pointer, var_name}, instruction, values});
            ++var_name;
            return entries.back()->dest;
        case Instruction::call:
            // FIXME return type is set to none
            entries.push_back(new Entry{Value{ValueType::immediate, var_name}, instruction, values});
            ++var_name;
            return entries.back()->dest;
        case Instruction::ret:
            entries.push_back(new Entry{Value{ValueType::none}, instruction, values});
            return entries.back()->dest;
        case Instruction::alloc:
            entries.push_back(new Entry{Value{ValueType::pointer, var_name}, instruction, values});
            ++var_name;
            return entries.back()->dest;
        case Instruction::load:
            entries.push_back(new Entry{Value{ValueType::reference, var_name}, instruction, values});
            ++var_name;
            return entries.back()->dest;
        case Instruction::store:
            entries.push_back(new Entry{Value{ValueType::none}, instruction, values});
            return entries.back()->dest;
        case Instruction::br:
            entries.push_back(new Entry{Value{ValueType::none}, instruction, values});
            return entries.back()->dest;
        case Instruction::brz:
            entries.push_back(new Entry{Value{ValueType::none}, instruction, values});
            return entries.back()->dest;
        case Instruction::brnz:
            entries.push_back(new Entry{Value{ValueType::none}, instruction, values});
            return entries.back()->dest;
        default:
            return Value{ValueType::none};
    }
}
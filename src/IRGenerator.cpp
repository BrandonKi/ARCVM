#include "IRGenerator.h"

IRGenerator::IRGenerator() : modules_{} {}

// TODO use allocator
Module* IRGenerator::create_module() {
    return new Module{};
}

void link_modules() {}

Function* Module::gen_function_def(std::string name, std::vector<Type> parameters, Type return_type) {
    auto* fn = new Function{name, true, std::move(parameters), return_type, {}};
    fn->gen_label("start");
    return fn;
}

// Function* gen_aggregate_def(std::string, std::vector<Type>);
// Function* gen_function_decl(std::string, std::vector<Type>, Type);
// Function* gen_aggregate_decl(std::string, std::vector<Type>);

// TODO implement this
Label* Function::gen_label(std::string name) {
    return block.gen_label(std::move(name));
}

Value Function::gen_inst(Instruction instruction, Value value) {
    return block.gen_inst(instruction, value);
}

Value Function::gen_inst(Instruction instruction, std::vector<Value> values) {
    return block.gen_inst(instruction, values);
}

Value Function::get_param(i32) {
    return Value{ValueType::none};
}

// TODO use allocator
Label* Block::gen_label(std::string name) {
    blocks.emplace_back(Label{std::move(name)}, std::vector<Entry>{});
    return &blocks.back().label;
}

Value Block::gen_inst(Instruction instruction, Value value) {
    return blocks.back().gen_inst(instruction, value, var_name);
}

Value Block::gen_inst(Instruction instruction, std::vector<Value> values) {
    return blocks.back().gen_inst(instruction, values, var_name);
}

Value BasicBlock::gen_inst(Instruction instruction, Value value, i32& var_name) {
    return gen_inst(instruction, std::vector{value}, var_name);
}

Value BasicBlock::gen_inst(Instruction instruction, std::vector<Value> values, i32& var_name) {
    switch(instruction) {
        case Instruction::add:
        case Instruction::index:
        case Instruction::call:
        case Instruction::ret:
            entries.emplace_back(Value{ValueType::none}, instruction, values);
            return entries.back().dest;
        case Instruction::alloc:
            entries.emplace_back(Value{ValueType::pointer, var_name}, instruction, values);
            ++var_name;
            return entries.back().dest;
        case Instruction::load:
        case Instruction::store:
            entries.emplace_back(Value{ValueType::none}, instruction, values);
            return entries.back().dest;
        default:
            return Value{ValueType::none};
    }
}

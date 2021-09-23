#include "IRGenerator.h"

IRGenerator::IRGenerator() : modules_{} {}

// TODO use allocator
Module* IRGenerator::create_module() {
    return new Module{};
}

void link_modules() {}

Function* Module::gen_function_def(std::string name, std::vector<Type> parameters,
                                   Type return_type) {
    auto* fn = new Function{name, true, std::move(parameters), return_type, {}};
    fn->gen_label("fn_start");
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

Value Function::gen_inst(Instruction, std::vector<Value>) {
    return Value{ValueType::none};
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
    return blocks.back().gen_inst(instruction, value);
}

Value BasicBlock::gen_inst(Instruction instruction, Value value) {
    entries.emplace_back(-1, instruction, std::vector{value});
    return Value{ValueType::none};
}

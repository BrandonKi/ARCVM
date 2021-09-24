#include "IRGenerator.h"
#include "IRPrinter.h"

// e2e tests always pass and just print their IR result at the moment 
bool test0() {
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {Type::none}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto val = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {val, Value{ValueType::immediate, 1}});
    main->gen_inst(Instruction::ret, {val});

    IRPrinter::print(main);
    return true;
}

int main() {
    test0();
}
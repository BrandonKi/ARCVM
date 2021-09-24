#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "IRPrinter.h"

#include <color_print.h>

#include <string>
#include <string_view>
#include <concepts>

static bool noisy = false;
static i32 passed_tests = 0;
static i32 failed_tests = 0;

void print_report() {
    std::cout << passed_tests << "/" << (passed_tests + failed_tests) << " tests passed\n";
}

template <std::invocable T>
void run_named_test(std::string name, T test) {
    if(test()) {
        std::cout << name << '\t' << cprint::fmt("pass", cprint::GREEN) << '\n';
        ++passed_tests;
    }
    else {
        std::cout << name << '\t' << cprint::fmt("fail", cprint::RED) << '\n';
        ++failed_tests;
    }
}

#define run_test(name) run_named_test(#name, name)

static bool test0() {
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {Type::none}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto val = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {val, Value{ValueType::immediate, 10}});
    main->gen_inst(Instruction::ret, {val});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 10;
}

static bool test1() {
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {Type::none}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1 = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2 = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1, Value{ValueType::immediate, 10}});
    main->gen_inst(Instruction::store, {op2, Value{ValueType::immediate, 10}});
    auto tmp = main->gen_inst(Instruction::add, {op1, op2});
    main->gen_inst(Instruction::store, {sum, tmp});
    main->gen_inst(Instruction::ret, {sum});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 20;
}

using namespace std::literals;

int main(int argc, char *argv[]) {
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;

    run_test(test0);
    run_test(test1);

    print_report();
}
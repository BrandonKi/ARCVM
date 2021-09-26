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
    ARCVM_PROFILE();
    if(test()) {
        std::cout << name << '\t' << cprint::fmt("pass", cprint::BRIGHT_GREEN) << '\n';
        ++passed_tests;
    }
    else {
        std::cout << name << '\t' << cprint::fmt("fail", cprint::BRIGHT_RED) << '\n';
        ++failed_tests;
    }
}

#define run_test(name) run_named_test(#name, name)

inline static bool create_var() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto val_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 10}});
    auto val = main->gen_inst(Instruction::load, {val_ptr});
    main->gen_inst(Instruction::ret, {val});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 10;
}

inline static bool add_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 10}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 10}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::add, {op1, op2});
    main->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = main->gen_inst(Instruction::load, {sum_ptr});
    main->gen_inst(Instruction::ret, {sum});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 20;
}

inline static bool sub_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 10}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::sub, {op1, op2});
    main->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = main->gen_inst(Instruction::load, {sum_ptr});
    main->gen_inst(Instruction::ret, {sum});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 90;
}

inline static bool mul_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 5}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 10}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::mul, {op1, op2});
    main->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = main->gen_inst(Instruction::load, {sum_ptr});
    main->gen_inst(Instruction::ret, {sum});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 50;
}

inline static bool div_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 70}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::div, {op1, op2});
    main->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = main->gen_inst(Instruction::load, {sum_ptr});
    main->gen_inst(Instruction::ret, {sum});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 10;
}

inline static bool index_stack_buffer() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i64}});
    auto op1_ptr = main->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 0}});
    auto op2_ptr = main->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 4}});
    auto sum_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr, Value{Type::ir_i32}});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr, Value{Type::ir_i32}});
    auto tmp = main->gen_inst(Instruction::add, {op1, op2});
    main->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = main->gen_inst(Instruction::load, {sum_ptr});
    main->gen_inst(Instruction::ret, {sum});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 200;
}

inline static bool index_stack_buffer2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i64}});
    auto op1_ptr = main->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 0}});
    auto op2_ptr = main->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 4}});
    auto sum_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr, Value{Type::ir_i32}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr, Value{Type::ir_i32}});
    auto tmp = main->gen_inst(Instruction::add, {op1, op2});
    main->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = main->gen_inst(Instruction::load, {sum_ptr});
    main->gen_inst(Instruction::ret, {sum});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 200;
}

using namespace std::literals;

int main(int argc, char *argv[]) {
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;

    run_test(create_var);
    run_test(add_vars);
    run_test(sub_vars);
    run_test(mul_vars);
    run_test(div_vars);
    run_test(index_stack_buffer);
    run_test(index_stack_buffer2);

    print_report();
}
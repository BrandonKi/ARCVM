#include "Common.h"
#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "IRPrinter.h"
#include "Arcvm.h"

using namespace arcvm;

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
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 70}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::div, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 10;
}

inline static bool mod_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 76}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::mod, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 6;
}

inline static bool bin_or_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 2}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::bin_or, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 3;
}

inline static bool bin_and_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 3}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::bin_and, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 3;
}

inline static bool bin_xor_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 3}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::bin_xor, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 4;
}

inline static bool lshift_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 3}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 2}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::lshift, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 12;
}

inline static bool rshift_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 7}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::rshift, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 3;
}

inline static bool lt_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::lt, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool gt_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::gt, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool lte_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::lte, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool lte_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::lte, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool lte_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::lte, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool gte_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::gte, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool gte_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::gte, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool gte_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto op1_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    main->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = main->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = main->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = main->gen_inst(Instruction::gte, {op1, op2});
    main->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = main->gen_inst(Instruction::load, {result_ptr});
    main->gen_inst(Instruction::ret, {result});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool index_stack_buffer1() {
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

inline static bool no_arg_function_call() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto ret = main->gen_inst(Instruction::call, {Value{ValueType::table_index, 0}, Value{Type::ir_i32}});
    main->gen_inst(Instruction::ret, {ret});

    auto* func = main_module->gen_function_def("func", {}, Type::ir_i32);
    auto val_ptr = func->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    func->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 70}});
    auto val = func->gen_inst(Instruction::load, {val_ptr});

    func->gen_inst(Instruction::ret, {val});


    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    return interp.run() == 70;
}

inline static bool arcvm_api_test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto val_ptr = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 0}});
    auto val = main->gen_inst(Instruction::load, {val_ptr});
    main->gen_inst(Instruction::ret, {val});

    Arcvm vm;
    vm.load_module(main_module);

    return vm.run() == 0;
}

using namespace std::literals;

// TODO use thread pool for running tests
int main(int argc, char *argv[]) {
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;

    run_test(create_var);
    run_test(add_vars);
    run_test(sub_vars);
    run_test(mul_vars);
    run_test(div_vars);
    run_test(mod_vars);
    run_test(bin_or_vars);
    run_test(bin_and_vars);
    run_test(bin_xor_vars);
    run_test(lshift_vars);
    run_test(rshift_vars);
    run_test(lt_vars);
    run_test(gt_vars);
    run_test(lte_vars1);
    run_test(lte_vars2);
    run_test(lte_vars3);
    run_test(gte_vars1);
    run_test(gte_vars2);
    run_test(gte_vars3);
    run_test(index_stack_buffer1);
    run_test(index_stack_buffer2);
    run_test(no_arg_function_call);
    run_test(arcvm_api_test);

    print_report();
}


// this design would allow for easier multithreaded IR generation
// since the IRGenerators have no dependency on the vm or each other
/*
IRGenerator gen;

...

IRModule m1 = gen.create_module();
gen.clear();

...

IRModule m2 = gen.create_module();

Arcvm vm;
vm.load_module(m1);
vm.load_module(m2);

// interpret
i32 ec = vm.run();

// create a binary file
vm.compile();

// jit compile and run
vm.jit();


*/
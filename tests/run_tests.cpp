#include "Common.h"
#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "IRPrinter.h"
#include "Arcvm.h"

#include <mutex>

using namespace arcvm;

static bool noisy = false;
static std::atomic<i32> passed_tests = 0;
static std::atomic<i32> failed_tests = 0;
static std::mutex cout_mutex;

void print_report() {
    std::cout << passed_tests << "/" << (passed_tests + failed_tests) << " tests passed\n";
}

template <std::invocable T>
void run_named_test(std::string_view name, T test) {
    ARCVM_PROFILE();
    if(test()) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        std::cout << name << '\t' << cprint::fmt("pass", cprint::BRIGHT_GREEN) << '\n';
        ++passed_tests;
    }
    else {
        std::unique_lock<std::mutex> lock(cout_mutex);
        std::cout << name << '\t' << cprint::fmt("fail", cprint::BRIGHT_RED) << '\n';
        ++failed_tests;
    }
}

#define run_test(name) test_thread_pool.push_work([=]{run_named_test(#name, name);})

inline static bool create_var() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 10}});
    auto val = bblock->gen_inst(Instruction::load, {val_ptr});
    bblock->gen_inst(Instruction::ret, {val});
    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 10;
}

inline static bool add_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 10}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 10}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::add, {op1, op2});
    bblock->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = bblock->gen_inst(Instruction::load, {sum_ptr});
    bblock->gen_inst(Instruction::ret, {sum});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 20;
}

inline static bool sub_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 10}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::sub, {op1, op2});
    bblock->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = bblock->gen_inst(Instruction::load, {sum_ptr});
    bblock->gen_inst(Instruction::ret, {sum});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 90;
}

inline static bool mul_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 5}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 10}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::mul, {op1, op2});
    bblock->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = bblock->gen_inst(Instruction::load, {sum_ptr});
    bblock->gen_inst(Instruction::ret, {sum});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 50;
}

inline static bool div_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 70}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::div, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 10;
}

inline static bool mod_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 76}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::mod, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 6;
}

inline static bool bin_or_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 2}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::bin_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 3;
}

inline static bool bin_and_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 3}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::bin_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 3;
}

inline static bool bin_xor_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 3}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::bin_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 4;
}

inline static bool lshift_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 3}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 2}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lshift, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 12;
}

inline static bool rshift_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 7}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::rshift, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 3;
}

inline static bool lt_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lt, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool gt_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gt, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool lte_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool lte_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool lte_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool gte_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool gte_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool gte_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool log_or_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool log_or_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 333}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool log_or_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool log_or_vars4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool log_and_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool log_and_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 333}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool log_and_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool log_and_vars4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool log_xor_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool log_xor_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 333}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool log_xor_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 1;
}

inline static bool log_xor_vars4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 0;
}

inline static bool index_stack_buffer1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i64}});
    auto op1_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 0}});
    auto op2_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 4}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, Value{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, Value{Type::ir_i32}});
    auto tmp = bblock->gen_inst(Instruction::add, {op1, op2});
    bblock->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = bblock->gen_inst(Instruction::load, {sum_ptr});
    bblock->gen_inst(Instruction::ret, {sum});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 200;
}

inline static bool index_stack_buffer2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_op2_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i64}});
    auto op1_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 0}});
    auto op2_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, Value{ValueType::immediate, 4}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, Value{ValueType::immediate, 100}, Value{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, Value{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, Value{Type::ir_i32}});
    auto tmp = bblock->gen_inst(Instruction::add, {op1, op2});
    bblock->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = bblock->gen_inst(Instruction::load, {sum_ptr});
    bblock->gen_inst(Instruction::ret, {sum});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 200;
}

inline static bool no_arg_function_call() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body1 = main->get_block();
    auto* bblock1 = fn_body1->get_bblock();
    auto ret = bblock1->gen_inst(Instruction::call, {Value{ValueType::table_index, 0}, Value{Type::ir_i32}});
    bblock1->gen_inst(Instruction::ret, {ret});

    auto* func = main_module->gen_function_def("func", {}, Type::ir_i32);
    auto* fn_body2 = main->get_block();
    auto* bblock2 = fn_body2->get_bblock();
    auto val_ptr = bblock2->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock2->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 70}});
    auto val = bblock2->gen_inst(Instruction::load, {val_ptr});

    bblock2->gen_inst(Instruction::ret, {val});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 70;
}

inline static bool arcvm_api_test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 0}});
    auto val = bblock->gen_inst(Instruction::load, {val_ptr});
    bblock->gen_inst(Instruction::ret, {val});

    Arcvm vm;
    vm.load_module(main_module);

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    return vm.run() == 0;
}

using namespace std::literals;

// TODO use thread pool for running tests
int main(int argc, char *argv[]) {
    ARCVM_PROFILE();
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;

    ThreadPool test_thread_pool;

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
    run_test(log_or_vars1);
    run_test(log_or_vars2);
    run_test(log_or_vars3);
    run_test(log_or_vars4);
    run_test(log_and_vars1);
    run_test(log_and_vars2);
    run_test(log_and_vars3);
    run_test(log_and_vars4);
    run_test(log_xor_vars1);
    run_test(log_xor_vars2);
    run_test(log_xor_vars3);
    run_test(log_xor_vars4);
    run_test(index_stack_buffer1);
    run_test(index_stack_buffer2);
    run_test(no_arg_function_call);
    run_test(arcvm_api_test);
    test_thread_pool.~ThreadPool();
    print_report();
}


//~ this design would allow for easier multithreaded IR generation
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

//~ currently there is no way to refer to basic blocks
// this is an issue because it makes generating control flow structures impossible
// for example statements like if-elseif-else-then, for, while, etc.
//
// maybe only let labels/basicblocks be created at function level
// then whatever code is calling a gen_inst will have to create explicity create a basic block
// this will then give them a way to reference the block
// problem solved
// then higher-level contructs can take basicblock pointers as arugments
// as long as the basicblocks are local to the current block there should be no issues
//
// this will also make phi nodes possible to implement
//
// something to think about:
// phi nodes can somewhat be "replaced" by pointers in some cases
// what do I want to do about this????
// obviously there will be a canonicalization pass
// however do I want to generate this instead of phi nodes by default?
// it could potentially make IR gen easier for the front end
// hmmm, actually a language without pointers may have a harder time trying to do this
// let's start with having users generate phi nodes
// it will allow for easier optimization in the backend and it's less to think about for the frontend

/*
// new function gen api
// this should handle label creation
create_basicblock()
// allow pushing/popping/reordering elements in the list???
get_basic_block_list()


// I don't see much of a reason to provide a public gen api for blocks
// at the moment it is purely used as a function body, so
// the function api can handle it

// new basic block api
basicblock->gen_inst(...);
// basicblock->gen_if(if, else, then);
basicblock->gen_if(BasicBlock, BasicBlock, BasicBlock);
// basicblock->gen_loop(header, body, then);
basicblock->gen_loop(BasicBlock, BasicBlock, BasicBlock);
*/

//~ an aritfact from the old design is the gen_function_def() function
// at this point it should just take a Block as a parameter
// at the moment it just handles the header stuff and let's
// the user generate the code directly

//~
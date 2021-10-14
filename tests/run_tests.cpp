#include "Common.h"
#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "IRPrinter.h"
#include "Arcvm.h"

#include <mutex>

using namespace arcvm;

//#define POOL

#ifdef POOL
#define run_test(name) test_thread_pool.push_work([=]{run_named_test(#name, name);})
#else
#define run_test(name) run_named_test(#name, name)
#endif

static bool noisy = false;
static std::atomic<i32> passed_tests = 0;
static std::atomic<i32> failed_tests = 0;
static std::mutex cout_mutex;

void print_report() {
    std::cout << passed_tests << "/" << (passed_tests + failed_tests) << " tests passed\n";
}

// there's a lot of lock contention here, not much I can do about it though
// the locks are already held for as little time as possible
template <std::invocable T>
void run_named_test(std::string_view name, T test) {
    ARCVM_PROFILE();
    if(test()) {
        static auto str = cprint::fmt("pass", cprint::BRIGHT_GREEN);
        std::unique_lock<std::mutex> lock(cout_mutex);
        std::cout << name << '\t' << str << '\n';
        lock.unlock();
        ++passed_tests;
    }
    else {
        static auto str = cprint::fmt("fail", cprint::BRIGHT_RED);
        std::unique_lock<std::mutex> lock(cout_mutex);
        std::cout << name << '\t' << str  << '\n';
        lock.unlock();
        ++failed_tests;
    }
}


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


inline static bool manual_branch() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto cond_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    auto cond_val = bblock->gen_inst(Instruction::load, {cond_ptr});
    bblock->gen_inst(Instruction::store, {cond_ptr, Value{ValueType::immediate, 1}});
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    bblock->gen_inst(Instruction::brz, {cond_val,{Value{new std::string("if_block")}},{Value{new std::string("else_block")}}});
    auto* if_block = fn_body->new_basic_block("if_block");
    if_block->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 1}});
    if_block->gen_inst(Instruction::br, {Value{new std::string("then_block")}});
    auto* else_block = fn_body->new_basic_block("else_block");
    else_block->gen_inst(Instruction::store, {val_ptr, Value{ValueType::immediate, 2}});
    else_block->gen_inst(Instruction::br, {Value{new std::string("then_block")}});
    auto* then_block = fn_body->new_basic_block("then_block");
    auto val = then_block->gen_inst(Instruction::load, {val_ptr});
    then_block->gen_inst(Instruction::ret, {val});

    if(noisy) {
        std::unique_lock<std::mutex> lock(cout_mutex);
        IRPrinter::print(main_module);
    }

    IRInterpreter interp(main_module);
    return interp.run() == 2;
    return false;
}


using namespace std::literals;

int main(int argc, char *argv[]) {
    ARCVM_PROFILE();
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;

    #ifdef POOL
    ThreadPool test_thread_pool;
    #endif

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
    run_test(manual_branch);

    #ifdef POOL
    test_thread_pool.~ThreadPool();
    #endif

    print_report();
}
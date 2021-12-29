#include "Common.h"
#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "IRPrinter.h"
#include "Arcvm.h"

#include <mutex>

using namespace arcvm;

//#define POOL
#define JIT_MODE

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
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {val_ptr, IRValue{IRValueType::immediate, 10}, IRValue{Type::ir_i32}});
    auto val = bblock->gen_inst(Instruction::load, {val_ptr, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {val});
    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 10;
#else
    return vm.run() == 10;
#endif
}

inline static bool add_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto tmp = bblock->gen_inst(Instruction::add, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {tmp});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 20;
#else
    return vm.run() == 20;
#endif
}

inline static bool sub_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto tmp = bblock->gen_inst(Instruction::sub, {op1, op2});
    bblock->gen_inst(Instruction::ret, {tmp});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 90;
#else
    return vm.run() == 90;
#endif
}

inline static bool mul_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{5}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto tmp = bblock->gen_inst(Instruction::mul, {op1, op2});
    bblock->gen_inst(Instruction::ret, {tmp});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 50;
#else
    return vm.run() == 50;
#endif
}

inline static bool div_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{70}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::div, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 10;
#else
    return vm.run() == 10;
#endif
}

inline static bool mod_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{76}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::mod, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 6;
#else
    return vm.run() == 6;
#endif
}

inline static bool bin_or_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{2}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::bin_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 3;
#else
    return vm.run() == 3;
#endif
}

inline static bool bin_and_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{3}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::bin_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 3;
#else
    return vm.run() == 3;
#endif
}

inline static bool bin_xor_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{3}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{7}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::bin_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 4;
#else
    return vm.run() == 4;
#endif
}

inline static bool lshift_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{3}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{2}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lshift, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 12;
#else
    return vm.run() == 12;
#endif
}

inline static bool rshift_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{7}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::rshift, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 3;
#else
    return vm.run() == 3;
#endif
}

inline static bool lt_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lt, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool gt_vars() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gt, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool lte_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool lte_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool lte_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool gte_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool gte_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{25}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool gte_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{25}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool log_or_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool log_or_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{333}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool log_or_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool log_or_vars4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_or, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool log_and_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool log_and_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{333}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool log_and_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool log_and_vars4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_and, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool log_xor_vars1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{1}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool log_xor_vars2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{333}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool log_xor_vars3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{1}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }
    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool log_xor_vars4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto result_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{0}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{0}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr});
    auto tmp = bblock->gen_inst(Instruction::log_xor, {op1, op2});
    bblock->gen_inst(Instruction::store, {result_ptr, tmp});
    auto result = bblock->gen_inst(Instruction::load, {result_ptr});
    bblock->gen_inst(Instruction::ret, {result});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}

inline static bool index_stack_buffer1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i64}});
    auto op1_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, IRValue{ 0}});
    auto op2_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, IRValue{4}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto tmp = bblock->gen_inst(Instruction::add, {op1, op2});
    bblock->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = bblock->gen_inst(Instruction::load, {sum_ptr});
    bblock->gen_inst(Instruction::ret, {sum});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 200;
#else
    return vm.run() == 200;
#endif
}

inline static bool index_stack_buffer2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i64}});
    auto op1_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, IRValue{0}});
    auto op2_ptr = bblock->gen_inst(Instruction::index, {op1_op2_ptr, IRValue{4}});
    auto sum_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto tmp = bblock->gen_inst(Instruction::add, {op1, op2});
    bblock->gen_inst(Instruction::store, {sum_ptr, tmp});
    auto sum = bblock->gen_inst(Instruction::load, {sum_ptr});
    bblock->gen_inst(Instruction::ret, {sum});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 200;
#else
    return vm.run() == 200;
#endif
}

inline static bool arcvm_api_test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {val_ptr, IRValue{0}});
    auto val = bblock->gen_inst(Instruction::load, {val_ptr});
    bblock->gen_inst(Instruction::ret, {val});


    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 0;
#else
    return vm.run() == 0;
#endif
}


inline static bool brz_test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto cond_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto cond_val = bblock->gen_inst(Instruction::load, {cond_ptr});
    bblock->gen_inst(Instruction::store, {cond_ptr, IRValue{1}});
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::brz, {cond_val,{IRValue{IRValueType::label, new std::string("if_block")}},{IRValue{IRValueType::label, new std::string("else_block")}}});
    auto* if_block = fn_body->new_basic_block("if_block");
    if_block->gen_inst(Instruction::store, {val_ptr, IRValue{1}});
    if_block->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string("then_block")}});
    auto* else_block = fn_body->new_basic_block("else_block");
    else_block->gen_inst(Instruction::store, {val_ptr, IRValue{2}});
    else_block->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string("then_block")}});
    auto* then_block = fn_body->new_basic_block("then_block");
    auto val = then_block->gen_inst(Instruction::load, {val_ptr});
    then_block->gen_inst(Instruction::ret, {val});

    if(noisy) {
        #ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
        #endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 2;
#else
    return vm.run() == 2;
#endif
}


inline static bool brnz_test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto cond_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto cond_val = bblock->gen_inst(Instruction::load, {cond_ptr});
    bblock->gen_inst(Instruction::store, {cond_ptr, IRValue{1}});
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::brnz, {cond_val,{IRValue{IRValueType::label, new std::string("if_block")}},{IRValue{IRValueType::label, new std::string("else_block")}}});
    auto* if_block = fn_body->new_basic_block("if_block");
    if_block->gen_inst(Instruction::store, {val_ptr, IRValue{1}});
    if_block->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string("then_block")}});
    auto* else_block = fn_body->new_basic_block("else_block");
    else_block->gen_inst(Instruction::store, {val_ptr, IRValue{2}});
    else_block->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string("then_block")}});
    auto* then_block = fn_body->new_basic_block("then_block");
    auto val = then_block->gen_inst(Instruction::load, {val_ptr});
    then_block->gen_inst(Instruction::ret, {val});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool branch_api() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto cond_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {cond_ptr, IRValue{1}});
    auto cond_val = bblock->gen_inst(Instruction::load, {cond_ptr});
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});

    auto* if_block = fn_body->new_basic_block("if_block");
    auto* else_block = fn_body->new_basic_block("else_block");
    auto* then_block = fn_body->new_basic_block("then_block");

    // if
    if_block->gen_inst(Instruction::store, {val_ptr, IRValue{1}});
    // else
    else_block->gen_inst(Instruction::store, {val_ptr, IRValue{2}});
    // then
    auto val = then_block->gen_inst(Instruction::load, {val_ptr});

    fn_body->set_insertion_point(bblock);
    fn_body->gen_if(cond_val, if_block, else_block, then_block);
    then_block->gen_inst(Instruction::ret, {val});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}


inline static bool branch_and_insertion_point_test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto cond_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto cond_val = bblock->gen_inst(Instruction::load, {cond_ptr});
    bblock->gen_inst(Instruction::store, {cond_ptr, IRValue{1}});
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto* if_block = fn_body->new_basic_block("if_block");
    auto* else_block = fn_body->new_basic_block("else_block");
    auto* then_block = fn_body->new_basic_block("then_block");

    fn_body->set_insertion_point(if_block);
    auto* i_block = fn_body->get_bblock();
    i_block->gen_inst(Instruction::store, {val_ptr, IRValue{1}});

    fn_body->set_insertion_point(else_block);
    auto* e_block = fn_body->get_bblock();
    e_block->gen_inst(Instruction::store, {val_ptr, IRValue{2}});

    fn_body->set_insertion_point(bblock);
    fn_body->gen_if(cond_val, if_block, else_block, then_block);

    fn_body->set_insertion_point(then_block);
    auto* t_block = fn_body->get_bblock();
    auto val = t_block->gen_inst(Instruction::load, {val_ptr});
    t_block->gen_inst(Instruction::ret, {val});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}


inline static bool no_arg_function_call() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body1 = main->get_block();
    auto* bblock1 = fn_body1->get_bblock();
    auto ret = bblock1->gen_inst(Instruction::call, {IRValue{IRValueType::fn_name, new std::string("func")}, IRValue{Type::ir_i32}});
    bblock1->gen_inst(Instruction::ret, {ret});

    auto* func = main_module->gen_function_def("func", {}, Type::ir_i32);
    auto* fn_body2 = func->get_block();
    auto* bblock2 = fn_body2->get_bblock();
    auto val_ptr = bblock2->gen_inst(Instruction::alloc, {IRValue{IRValueType::type, Type::ir_i32}});
    bblock2->gen_inst(Instruction::store, {val_ptr, IRValue{IRValueType::immediate, 70}});

    auto val = bblock2->gen_inst(Instruction::load, {val_ptr});

    bblock2->gen_inst(Instruction::ret, {val});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 70;
#else
    return vm.run() == 70;
#endif
}


inline static bool function_call_with_args_by_value() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body1 = main->get_block();
    auto* bblock1 = fn_body1->get_bblock();
    auto ret = bblock1->gen_inst(Instruction::call, {IRValue{IRValueType::fn_name, new std::string("add")}, IRValue{10}, IRValue{10}, IRValue{Type::ir_i32}});
    bblock1->gen_inst(Instruction::ret, {ret});

    auto* func = main_module->gen_function_def("add", {Type::ir_i32, Type::ir_i32}, Type::ir_i32);
    auto* fn_body2 = func->get_block();
    auto* bblock2 = fn_body2->get_bblock();
    auto val_ptr = bblock2->gen_inst(Instruction::alloc, {IRValue{IRValueType::type, Type::ir_i32}});
    auto sum = bblock2->gen_inst(Instruction::add, {func->get_param(0), func->get_param(1)});
    bblock2->gen_inst(Instruction::store, {val_ptr, sum});
    auto val = bblock2->gen_inst(Instruction::load, {val_ptr});

    bblock2->gen_inst(Instruction::ret, {val});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 20;
#else
    return vm.run() == 20;
#endif
}


inline static bool function_call_with_args_by_ref() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body1 = main->get_block();
    auto* bblock1 = fn_body1->get_bblock();
    auto lhs_ptr = bblock1->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock1->gen_inst(Instruction::store, {lhs_ptr, IRValue{IRValueType::immediate, 10}});
    auto lhs = bblock1->gen_inst(Instruction::load, {lhs_ptr});
    auto rhs_ptr = bblock1->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock1->gen_inst(Instruction::store, {rhs_ptr, IRValue{IRValueType::immediate, 10}});
    auto rhs = bblock1->gen_inst(Instruction::load, {rhs_ptr});

    auto ret = bblock1->gen_inst(Instruction::call, {IRValue{IRValueType::fn_name, new std::string("add")}, lhs, rhs, IRValue{Type::ir_i32}});
    bblock1->gen_inst(Instruction::ret, {ret});

    auto* func = main_module->gen_function_def("add", {Type::ir_i32, Type::ir_i32}, Type::ir_i32);
    auto* fn_body2 = func->get_block();
    auto* bblock2 = fn_body2->get_bblock();
    auto val_ptr = bblock2->gen_inst(Instruction::alloc, {IRValue{IRValueType::type, Type::ir_i32}});
    auto sum = bblock2->gen_inst(Instruction::add, {func->get_param(0), func->get_param(1)});
    bblock2->gen_inst(Instruction::store, {val_ptr, sum});
    auto val = bblock2->gen_inst(Instruction::load, {val_ptr});

    bblock2->gen_inst(Instruction::ret, {val});

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 20;
#else
    return vm.run() == 20;
#endif
}

inline static bool CF_cleanup_test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto cond_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto cond_val = bblock->gen_inst(Instruction::load, {cond_ptr});
    bblock->gen_inst(Instruction::store, {cond_ptr, IRValue{1}});
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto* test_block = fn_body->new_basic_block("test");
    auto* if_block = fn_body->new_basic_block("if_block");
    auto* else_block = fn_body->new_basic_block("else_block");
    auto* then_block = fn_body->new_basic_block("then_block");

    fn_body->set_insertion_point(if_block);
    auto* i_block = fn_body->get_bblock();
    i_block->gen_inst(Instruction::store, {val_ptr, IRValue{1}});
    auto val1 = i_block->gen_inst(Instruction::load, {val_ptr});
    i_block->gen_inst(Instruction::ret, {val1});

    fn_body->set_insertion_point(else_block);
    auto* e_block = fn_body->get_bblock();
    e_block->gen_inst(Instruction::store, {val_ptr, IRValue{2}});
    auto val2 = e_block->gen_inst(Instruction::load, {val_ptr});
    e_block->gen_inst(Instruction::ret, {val2});

    fn_body->set_insertion_point(bblock);
    fn_body->gen_if(cond_val, if_block, else_block, then_block);

    fn_body->set_insertion_point(then_block);
    auto* t_block = fn_body->get_bblock();
    auto val = t_block->gen_inst(Instruction::load, {val_ptr});
    t_block->gen_inst(Instruction::ret, {val});

    Arcvm vm;
    vm.load_module(main_module);
    vm.run_pass_on_module<CFResolutionPass>(main_module);

    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

#ifdef JIT_MODE
    return vm.jit() == 1;
#else
    return vm.run() == 1;
#endif
}

inline static bool test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {val_ptr, IRValue{IRValueType::immediate, 10}, IRValue{Type::ir_i32}});
    auto val = bblock->gen_inst(Instruction::load, {val_ptr, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {val});
    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }

    Arcvm vm;
    vm.load_module(main_module);
#ifdef JIT_MODE
    return vm.jit() == 10;
#else
    return vm.run() == 10;
#endif
}

using namespace std::literals;

int main(int argc, char *argv[]) {
    ARCVM_PROFILE();
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;

    #ifdef POOL
    ThreadPool test_thread_pool;
    #endif

    run_test(test);

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
    run_test(arcvm_api_test);
    run_test(brz_test);
    run_test(brnz_test);
    run_test(branch_api);
    run_test(branch_and_insertion_point_test);
    run_test(no_arg_function_call);
    run_test(function_call_with_args_by_value);
    run_test(function_call_with_args_by_ref);
    run_test(CF_cleanup_test);

    #ifdef POOL
    test_thread_pool.~ThreadPool();
    #endif

    print_report();
}
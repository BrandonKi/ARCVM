#include "Common.h"
#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "IRPrinter.h"
#include "Arcvm.h"

#include <mutex>

using namespace arcvm;

//#define POOL
// #define JIT_MODE

#ifdef POOL
    #define run_test(name) test_thread_pool.push_work([=]{run_named_test(#name, name);})
#else
    #define run_test(name) run_named_test(#name, name)
#endif

static bool noisy = false;
static std::atomic<i32> passed_tests = 0;
static std::atomic<i32> failed_tests = 0;
static std::mutex cout_mutex;

inline static void print_module_if_noisy(Module* main_module) {
    if(noisy) {
#ifdef POOL
        std::unique_lock<std::mutex> lock(cout_mutex);
#endif
        IRPrinter::print(main_module);
    }
}

inline static void run_passes(Arcvm& vm) {
    // TODO add options to enable optimizations for all tests
    // vm.run_canonicalization_passes();
    vm.optimize();
}

inline static int execute(Arcvm& vm) {
#ifdef JIT_MODE
    return vm.jit();
#else
    return vm.run();
#endif
}

void print_report() {
    std::cout << passed_tests << "/" << (passed_tests + failed_tests) <<
        " (" << ((float)passed_tests / (passed_tests + failed_tests) * 100) << "%) tests passed\n";
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

inline static bool g_1() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    print_module_if_noisy(main_module);

    return execute(vm) == 10;
}

inline static bool g_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i64}});
    bblock->gen_inst(Instruction::store, {val_ptr, IRValue{IRValueType::immediate, 512}, IRValue{Type::ir_i64}});
    auto val = bblock->gen_inst(Instruction::load, {val_ptr, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {val});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 0;
}

inline static bool g_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i64}});
    bblock->gen_inst(Instruction::store, {val_ptr, IRValue{IRValueType::immediate, 513}, IRValue{Type::ir_i64}});
    auto val = bblock->gen_inst(Instruction::load, {val_ptr, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {val});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool add_1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::add, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 20;
}

inline static bool add_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::add, {op1, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 25;
}

inline static bool add_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::add, {90, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 105;
}

inline static bool add_4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::add, {100, 29, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == -127;    // NOTE pedantic test
}

inline static bool sub_1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::sub, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 90;
}

inline static bool sub_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::sub, {op1, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == -5;
}

inline static bool sub_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::sub, {90, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 75;
}

inline static bool sub_4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::sub, {-100, 29, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 127;    // NOTE pedantic test
}

inline static bool mul_1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::mul, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1000;
}

inline static bool mul_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::mul, {op1, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 150;
}

inline static bool mul_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::mul, {90, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1350;
}

inline static bool mul_4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::mul, {3, 43, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == -127;    // NOTE pedantic test
}

inline static bool div_1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::div, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 10;
}

inline static bool div_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{15}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::div, {op1, 10, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool div_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::div, {90, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 6;
}

inline static bool div_4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::div, {-9, 4, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == -2;    // NOTE pedantic test
}

inline static bool mod_1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{109}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::mod, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 9;
}

inline static bool mod_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{10}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::mod, {op1, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 10;
}

inline static bool mod_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::mod, {98, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 8;
}

inline static bool mod_4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::mod, {9, -3, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 0;
}

inline static bool bin_or_1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{77}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{16}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::bin_or, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 93;
}

inline static bool bin_or_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{11}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::bin_or, {op1, 16, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 27;
}

inline static bool bin_or_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::bin_or, {98, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 111;
}

inline static bool bin_or_4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::bin_or, {2, -3, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == -1;
}

inline static bool bin_and_1() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    auto op2_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{77}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{121}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::bin_and, {op1, op2, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 73;
}

inline static bool bin_and_2() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto op1_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{11}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::bin_and, {op1, 19, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 3;
}

inline static bool bin_and_3() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::bin_and, {98, 15, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 2;
}

inline static bool bin_and_4() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto result = bblock->gen_inst(Instruction::bin_and, {67, -23, IRValue{Type::ir_i8}});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 65;
}

inline static bool bin_xor_1() {
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
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{3}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{7}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto result = bblock->gen_inst(Instruction::bin_xor, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 4;
}

inline static bool lshift_1() {
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
    auto result = bblock->gen_inst(Instruction::lshift, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 12;
}

inline static bool rshift_1() {
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
    auto result = bblock->gen_inst(Instruction::rshift, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 3;
}

inline static bool lt_1() {
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
    auto result = bblock->gen_inst(Instruction::lt, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 0;
}

inline static bool gt_1() {
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
    auto result = bblock->gen_inst(Instruction::gt, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool lte_1() {
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
    auto result = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool lte_2() {
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
    auto result = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 0;
}

inline static bool lte_3() {
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
    auto result = bblock->gen_inst(Instruction::lte, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool gte_1() {
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
    auto result = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool gte_2() {
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
    auto result = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool gte_3() {
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
    auto result = bblock->gen_inst(Instruction::gte, {op1, op2});
    bblock->gen_inst(Instruction::ret, {result});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 0;
}

inline static bool index_1() {
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
    bblock->gen_inst(Instruction::store, {op1_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    bblock->gen_inst(Instruction::store, {op2_ptr, IRValue{100}, IRValue{Type::ir_i32}});
    auto op1 = bblock->gen_inst(Instruction::load, {op1_ptr, IRValue{Type::ir_i32}});
    auto op2 = bblock->gen_inst(Instruction::load, {op2_ptr, IRValue{Type::ir_i32}});
    auto sum = bblock->gen_inst(Instruction::add, {op1, op2});
    bblock->gen_inst(Instruction::ret, {sum});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 200;
}

inline static bool index_2() {
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
    auto sum = bblock->gen_inst(Instruction::add, {op1, op2});
    bblock->gen_inst(Instruction::ret, {sum});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 200;
}

inline static bool arcvm_api_1() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 0;
}


inline static bool brz_1() {
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
    bblock->gen_inst(Instruction::brz, {cond_val,{IRValue{IRValueType::label, new std::string("if_block")}}, {IRValue{IRValueType::label, new std::string("else_block")}}});
    auto* if_block = fn_body->new_basic_block("if_block");
    if_block->gen_inst(Instruction::store, {val_ptr, IRValue{1}});
    if_block->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string("then_block")}});
    auto* else_block = fn_body->new_basic_block("else_block");
    else_block->gen_inst(Instruction::store, {val_ptr, IRValue{2}});
    else_block->gen_inst(Instruction::br, {IRValue{IRValueType::label, new std::string("then_block")}});
    auto* then_block = fn_body->new_basic_block("then_block");
    auto val = then_block->gen_inst(Instruction::load, {val_ptr});
    then_block->gen_inst(Instruction::ret, {val});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 2;
}


inline static bool brnz_1() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool branch_1() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}


inline static bool branch_2() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}


inline static bool function_call_1() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 70;
}


inline static bool function_call_2() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 20;
}


inline static bool function_call_3() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 20;
}

inline static bool CF_cleanup_1() {
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

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 1;
}

inline static bool negate_1() {
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
    auto neg_val = bblock->gen_inst(Instruction::neg, {val});
    auto sum = bblock->gen_inst(Instruction::add, {neg_val, 15});
    bblock->gen_inst(Instruction::ret, {sum});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    return execute(vm) == 5;
}

inline static bool dup_1() {
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
    auto val2 = bblock->gen_inst(Instruction::dup, {val});
    auto sum  = bblock->gen_inst(Instruction::add, {val, val2});
    bblock->gen_inst(Instruction::ret, {sum});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    run_passes(vm);
    print_module_if_noisy(main_module);
    return execute(vm) == 20;
}

// inline static bool test() {
//     ARCVM_PROFILE();
//     IRGenerator gen;
//     auto* main_module = gen.create_module();
//     auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
//     main->add_attribute(Attribute::entrypoint);
//     auto* fn_body = main->get_block();
//     auto* bblock = fn_body->get_bblock();
//     auto val_ptr = bblock->gen_inst(Instruction::alloc, {IRValue{Type::ir_i32}});
//     bblock->gen_inst(Instruction::store, {val_ptr, IRValue{IRValueType::immediate, 10}, IRValue{Type::ir_i32}});
//     auto val = bblock->gen_inst(Instruction::load, {val_ptr, IRValue{Type::ir_i32}});
//     auto val2 = bblock->gen_inst(Instruction::dup, {val});
//     auto sum  = bblock->gen_inst(Instruction::add, {val, val2});
//     bblock->gen_inst(Instruction::ret, {sum});

//     print_module_if_noisy(main_module);

//     Arcvm vm;
//     vm.load_module(main_module);
//     run_passes(vm);
//     return execute(vm) == 20;
// }


inline static bool test() {
    ARCVM_PROFILE();
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto* fn_body = main->get_block();
    auto* bblock = fn_body->get_bblock();
    auto val1 = bblock->gen_inst(Instruction::dup, {{IRValueType::immediate, 10}});
    auto val2 = bblock->gen_inst(Instruction::dup, {{IRValueType::immediate, 20}});
    auto sum1  = bblock->gen_inst(Instruction::add, {val1, val2});
    auto sum2  = bblock->gen_inst(Instruction::add, {sum1, val2});
    auto product  = bblock->gen_inst(Instruction::mul, {sum1, sum2});
    auto quotient  = bblock->gen_inst(Instruction::div, {product, IRValue{IRValueType::immediate, 10}});
    auto i_result1  = bblock->gen_inst(Instruction::lshift, {quotient, IRValue{IRValueType::immediate, 4}});
    auto i_result2  = bblock->gen_inst(Instruction::mod, {i_result1, IRValue{IRValueType::immediate, 7}});
    bblock->gen_inst(Instruction::ret, {i_result2});

    print_module_if_noisy(main_module);

    Arcvm vm;
    vm.load_module(main_module);
    // run_passes(vm);
    vm.optimize();
    print_module_if_noisy(main_module);
    return execute(vm) == 6;
}

using namespace std::literals;

int main(int argc, char *argv[]) {
    ARCVM_PROFILE();
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;

#ifdef POOL
    ThreadPool test_thread_pool;
#endif

    // run_test(test);

// TODO create tests for combinations of immediates and references in instruction operands
// most of these special cases aren't handled properly at the moment

    run_test(g_1);    // p
    run_test(g_2);
    run_test(g_3);
    run_test(add_1);    // p
    run_test(add_2);
    run_test(add_3);
    run_test(add_4);
    run_test(sub_1);    // p
    run_test(sub_2);
    run_test(sub_3);
    run_test(sub_4);
    run_test(mul_1);    // p
    run_test(mul_2);
    run_test(mul_3);
    run_test(mul_4);
    run_test(div_1);
    run_test(div_2);
    run_test(div_3);
    run_test(div_4);
    run_test(mod_1);
    run_test(mod_2);
    run_test(mod_3);
    run_test(mod_4);
    run_test(bin_or_1);     // p
    run_test(bin_or_2);
    run_test(bin_or_3);
    run_test(bin_or_4);
    run_test(bin_and_1);    // p
    run_test(bin_and_2);
    run_test(bin_and_3);
    run_test(bin_and_4);
    run_test(bin_xor_1);    // p
    run_test(lshift_1);
    run_test(rshift_1);
    run_test(lt_1);
    run_test(gt_1);
    run_test(lte_1);
    run_test(lte_2);
    run_test(lte_3);
    run_test(gte_1);
    run_test(gte_2);
    run_test(gte_3);
    run_test(index_1);
    run_test(index_2);
    run_test(arcvm_api_1);
    run_test(brz_1);
    run_test(brnz_1);
    run_test(branch_1);
    run_test(branch_2);
    run_test(function_call_1);
    run_test(function_call_2);
    run_test(function_call_3);
    run_test(CF_cleanup_1);
    run_test(negate_1);
    run_test(dup_1);
/*
*/

#ifdef POOL
    test_thread_pool.~ThreadPool();
#endif

    print_report();

    // TODO repl-like CLI with basic QOL functionality
    // view failed tests
    // rerun individual tests
    // rerun all failed tests
    // print IR for a specific test
}
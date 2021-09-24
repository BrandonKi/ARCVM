#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "IRPrinter.h"

#include <color_print.h>

#include <string>
#include <string_view>

template <typename T>
void run_test(std::string name, T test) {
    if(test())
        std::cout << name << '\t' << cprint::fmt("pass", cprint::GREEN) << '\n';
    else
        std::cout << name << '\t' << cprint::fmt("fail", cprint::RED) << '\n';
}

static bool noisy = false;

bool test0() {
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {Type::none}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    auto val = main->gen_inst(Instruction::alloc, {Value{ValueType::type, Type::ir_i32}});
    main->gen_inst(Instruction::store, {val, Value{ValueType::immediate, 1}});
    main->gen_inst(Instruction::ret, {val});

    if(noisy)
        IRPrinter::print(main);

    IRInterpreter interp(main_module);
    auto ret = interp.run();

    return ret == 0;
}

using namespace std::literals;

int main(int argc, char *argv[]) {
    if(argc > 1 && "-noisy"sv == argv[1])
        noisy = true;
    run_test("test0", test0);
}
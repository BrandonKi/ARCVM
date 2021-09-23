// CLI for calling ARCVM directly

#include "Arcvm.h"
#include "IRGenerator.h"
#include "IRPrinter.h"

#include <iostream>
#include <string>
#include <vector>

Args get_args(int argc, char *argv[]) {
    Args args;
    // set defaults
    args.opt_level = OptimizationLevel::zero;
    args.mode = Mode::text;
    args.output_file_name = "a.exe";

    for(std::string_view string : std::vector<std::string_view>(argv + 1, argv + argc)) {
        switch(string[0]) {
            case '-':
                switch(string[1]) {
                    case 'O':
                        errno = 0;
                        args.opt_level = static_cast<OptimizationLevel>(std::strtol(string.substr(2).data(), nullptr, 10));
                        if (errno != 0) {
                            std::cerr << "Invalid argument given to '-O'\n";
                            args.opt_level = OptimizationLevel::zero;
                        }
                        break;
                    case 'o':
                        args.output_file_name = string.substr(2);
                        break;
                    case 'b':
                        args.mode = Mode::binary;
                        break;
                    case 't':
                        args.mode = Mode::text;
                        break;
                }
                break;
            // if an argument doesn't start with '-' then assume it's an input file name
            default:
                args.input_files.emplace_back(string);
        }
    }
    return args;
}

int main(int argc, char *argv[]) {
    // Args args = get_args(argc, argv);
    // Arcvm vm(args);
    IRGenerator gen;
    auto* main_module = gen.create_module();
    auto* main = main_module->gen_function_def("main", {}, Type::ir_i32);
    main->add_attribute(Attribute::entrypoint);
    main->gen_inst(Instruction::ret, Value{ValueType::immediate, 1});

    IRPrinter::print(main);
}
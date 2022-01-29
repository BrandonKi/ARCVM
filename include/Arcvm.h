#ifndef ARCVM_ARCVM_H
#define ARCVM_ARCVM_H

#include "Common.h"

#include "IRGenerator.h"
#include "IRInterpreter.h"

#include "Backends/x86_64_Backend.h"

#include "Passes/PassManager.h"
#include "Passes/CFResolutionPass.h"
#include "Passes/ConstantPropogation.h"
#include "Passes/ImmediateCanonicalization.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace arcvm {

template <typename T> concept Stringable = requires(T t) {
    std::string(t);
};

struct File {
    std::string file_name;
    std::string file_data;

    template <Stringable T> File(T name) : file_name(std::move(name)) {}
};

enum class OptimizationLevel : i8 { custom = -1, zero = 0, one = 1, two = 2, three = 3 };

enum class Mode : i8 { binary = 0, text = 1 };

struct Args {
    OptimizationLevel opt_level;
    Mode mode;
    std::vector<File> input_files;
    std::string output_file_name;

    void debug_print() {
        std::cout << "OptimizationLevel: " << i32(opt_level) << '\n';
        std::cout << "Mode: " << i32(mode) << '\n';
        std::cout << "Output File: " << output_file_name << '\n';
        std::cout << "Input Files: \n";
        for (auto const& [name, data] : input_files)
            std::cout << '\t' << name << '\n';
    }
};

class Arcvm {
  public:
    Arcvm();
    Arcvm(Args);

    void load_module(Module*);

    void optimize();
    void optimize_module(Module*);

    void run_canonicalization_passes();

    i32 run();
    i32 jit();
    i32 compile();

    template <typename P>
    void run_pass_on_module(Module* module) {
        P pass;
        pass.module_pass(module);
    }

    void write_file();

  private:
    Args args_;
    std::vector<Module*> modules_;
    std::vector<CompiledModule*> compiled_modules_;
};

};

#endif
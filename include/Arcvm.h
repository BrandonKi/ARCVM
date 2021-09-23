#ifndef ARCVM_ARCVM_H
#define ARCVM_ARCVM_H

#include "IRGenerator.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

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
    Arcvm(Args);

  private:
    Args args_;
};

#endif
#include "IRPrinter.h"

void IRPrinter::print(Module* module, i32 indent) {}

void IRPrinter::print(Function* function, i32 indent) {
    IRPrinter::print(function->attributes);
    std::cout << "define function " << function->name;
    IRPrinter::print(function->parameters);
    std::cout << " -> " << to_string(function->return_type);
    if (function->is_complete) {
        std::cout << " {\n";
        IRPrinter::print(&function->block, indent + 2);
    } else {
        std::cout << ";\n";
    }
}

void IRPrinter::print(std::vector<Type>& parameters, i32 indent) {
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    if (!parameters.empty()) {
        print_indent();
        std::cout << '(';
        for (size_t i = 0; i < parameters.size() - 1; ++i)
            std::cout << to_string(parameters[i]) << ", ";
        std::cout << to_string(parameters.back()) << ")";
    } else {
        std::cout << "()";
    }
}

void IRPrinter::print(std::vector<Attribute>& attributes, i32 indent) {
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    if (!attributes.empty()) {
        print_indent();
        std::cout << '[';
        for (size_t i = 0; i < attributes.size() - 1; ++i)
            std::cout << to_string(attributes[i]) << ", ";
        std::cout << to_string(attributes.back()) << "]\n";
    }
}

void IRPrinter::print(Block* block, i32 indent) {
    for (auto basic_block : block->blocks) {
        IRPrinter::print(&basic_block, indent);
    }
}

void IRPrinter::print(BasicBlock* basic_block, i32 indent) {
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    print_indent();
    std::cout << '#' << basic_block->label.name << '\n';
    for (auto entry : basic_block->entries) {
        IRPrinter::print(&entry, indent + 2);
    }
}

void IRPrinter::print(Entry* entry, i32 indent) {
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    print_indent();
    switch (entry->instruction) {
        case Instruction::ret:
            std::cout << "ret " << entry->arguments[0].value << '\n';
            break;

        default:
            break;
    }
}
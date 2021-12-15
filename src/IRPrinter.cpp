#include "IRPrinter.h"

using namespace arcvm;

void IRPrinter::print(Module* module, i32 indent) {
    ARCVM_PROFILE();
    for(size_t i = 0; i < module->functions.size(); ++i) {
        IRPrinter::print(module->functions[i]);
    }
}

void IRPrinter::print(Function* function, i32 var_name, i32 indent) {
    ARCVM_PROFILE();
    IRPrinter::print(function->attributes);
    std::cout << "define function " << function->name;
    IRPrinter::print(function->parameters, var_name);
    std::cout << " -> " << to_string(function->return_type);
    std::cout << " {\n";
    IRPrinter::print(function->block, var_name, indent + 2);
    std::cout << "}\n";
}

void IRPrinter::print(std::vector<Type>& parameters, i32& var_name, i32 indent) {
    ARCVM_PROFILE();
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    if (!parameters.empty()) {
        print_indent();
        std::cout << '(';
        for (size_t i = 0; i < parameters.size() - 1; ++i)
            std::cout << to_string(parameters[i]) << " %" << var_name++ << ", ";
        std::cout << to_string(parameters.back()) << " %" << var_name++ << ")";
    } else {
        std::cout << "()";
    }
}

void IRPrinter::print(std::vector<Attribute>& attributes, i32 indent) {
    ARCVM_PROFILE();
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    if (!attributes.empty()) {
        print_indent();
        std::cout << '[';
        for (size_t i = 0; i < attributes.size() - 1; ++i)
            std::cout << to_string(attributes[i]) << ", ";
        std::cout << to_string(attributes.back()) << "]\n";
    }
}

void IRPrinter::print(Block* block, i32& var_name, i32 indent) {
    ARCVM_PROFILE();
    for (auto basic_block : block->blocks) {
        IRPrinter::print(basic_block, var_name, indent);
    }
}

void IRPrinter::print(BasicBlock* basic_block, i32& var_name, i32 indent) {
    ARCVM_PROFILE();
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    print_indent();
    std::cout << '#' << basic_block->label.name << '\n';
    for (auto entry : basic_block->entries) {
        IRPrinter::print(entry, var_name, indent + 2);
    }
}

void IRPrinter::print(Entry* entry, i32& var_name, i32 indent) {
    ARCVM_PROFILE();
    auto print_indent = [=]() { std::cout << std::string(indent, ' '); };

    print_indent();
    if(entry->dest.type != IRValueType::none)
        std::cout << '%' << var_name++ << " = ";
    std::cout << to_string(entry->instruction) << ' ';

    for(size_t i = 0; i < entry->arguments.size() - 1; ++i) {
        IRPrinter::print(&entry->arguments[i]);
        std::cout << ", ";
    }
    IRPrinter::print(&entry->arguments.back());
    std::cout << '\n';
}

void IRPrinter::print(IRValue* value, i32 indent) {
    ARCVM_PROFILE();
    switch(value->type) {
        case IRValueType::none:
            std::cout << "none";
            break;
        case IRValueType::immediate:
            std::cout << value->value;
            break;
        case IRValueType::reference:
        case IRValueType::pointer:
            std::cout << "%" << value->value;
            break;
        case IRValueType::type:
            std::cout << to_string(value->type_value);
            break;
        case IRValueType::label:
            std::cout << "#" << *(value->str_value);
            break;
        case IRValueType::fn_name:
            std::cout << "@" << *(value->str_value);
            break;
        default:
            break;
    }
}
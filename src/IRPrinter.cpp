#include "IRPrinter.h"

void IRPrinter::print(Module* module) {

}

void IRPrinter::print(Function* function) {
    // TODO move into print attribute list function
    if(!function->attributes.empty()) {
        std::cout << '[';
        for (auto const& attribute : function->attributes) {
            std::cout << to_string(attribute) << ", ";
        }
        std::cout << "]\n";
    }
    std::cout << "define function " << function->name << "(";
    // TODO move into print fn args function
    for(auto const& parameter : function->parameters) {
        std::cout << to_string(parameter) << ", ";
    }
    std::cout << ")";
    if(function->is_complete) {
        std::cout << " {\n";
        IRPrinter::print(&function->block);
    }
    else {
        std::cout << ";\n";
    }
}

void IRPrinter::print(Block* block) {
    for(auto basic_block : block->blocks) {
        IRPrinter::print(&basic_block);
    }
}

void IRPrinter::print(BasicBlock* basic_block) {
    std::cout << '#' << basic_block->label.name << '\n';
    for(auto entry : basic_block->entries) {
        IRPrinter::print(&entry);
    }
}

void IRPrinter::print(Entry* entry) {
    switch (entry->instruction) {
    case Instruction::ret:
        std::cout << "ret " << entry->arguments[0].value << '\n';
        break;
    
    default:
        break;
    }
}
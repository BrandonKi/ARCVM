#ifndef ARCVM_IRPRINTER_H
#define ARCVM_IRPRINTER_H

#include "Common.h"

#include <iostream>

namespace arcvm {

namespace IRPrinter {

void print(Module*, i32 indent = 0);
void print(Function*, i32 = 0, i32 indent = 0);
void print(std::vector<Type>&, i32&, i32 indent = 0);
void print(std::vector<Attribute>&, i32 indent = 0);
void print(Block*, i32&, i32 indent = 0);
void print(BasicBlock*, i32&, i32 indent = 0);
void print(Entry*, i32&, i32 indent = 0);
void print(Value* value, i32 indent = 0);

} // namespace IRPrinter

};

#endif
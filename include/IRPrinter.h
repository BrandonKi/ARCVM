#ifndef ARCVM_IRPRINTER_H
#define ARCVM_IRPRINTER_H

// FIXME only includes this for the types
// move them into a different header
#include "IRGenerator.h"

#include <iostream>

namespace IRPrinter {

void print(Module*, i32 indent = 0);
void print(Function*, i32 indent = 0);
void print(std::vector<Type>&, i32 indent = 0);
void print(std::vector<Attribute>&, i32 indent = 0);
void print(Block*, i32 indent = 0);
void print(BasicBlock*, i32 indent = 0);
void print(Entry*, i32 indent = 0);

} // namespace IRPrinter

#endif
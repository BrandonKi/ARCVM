#ifndef ARCVM_IRPRINTER_H
#define ARCVM_IRPRINTER_H

// FIXME only includes this for the types
// move them into a different header
#include "IRGenerator.h"

#include <iostream>

namespace IRPrinter {
    void print(Module*);
    void print(Function*);
    void print(Block*);
    void print(BasicBlock*);
    void print(Entry*);
}

#endif
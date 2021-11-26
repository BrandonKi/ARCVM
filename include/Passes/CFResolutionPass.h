#ifndef ARCVM_C_F_RESOLUTION_PASS_H
#define ARCVM_C_F_RESOLUTION_PASS_H

// basic control flow resolution and canonicalization

#include "Pass.h"
#include "Common.h"

namespace arcvm {

class CFResolutionPass {
  public:
    void block_pass(Block* block);
    void function_pass(Function* function);
    void module_pass(Module* module);

  private:
    void process_function(Function*);
    void process_block(Block*);
    void remove_dead_br(BasicBlock*);
    void add_explicit_fallthrough(BasicBlock*, BasicBlock*);
};

};

#endif //ARCVM_C_F_RESOLUTION_PASS_H

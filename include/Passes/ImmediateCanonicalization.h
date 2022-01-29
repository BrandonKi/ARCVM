// TODO
#ifndef ARCVM_IMMEDIATE_CANONICALIZATION_H
#define ARCVM_IMMEDIATE_CANONICALIZATION_H

// transforms instructions that operate only on immediates to a 'dup' instruction
//

#include "Pass.h"
#include "Common.h"

namespace arcvm {

    class ImmediateCanonicalization {
        public:
        void block_pass(Block* block);
        void function_pass(Function* function);
        void module_pass(Module* module);

        private:
        void process_function(Function*);
        void process_block(Block*);
    };

};

#endif //ARCVM_IMMEDIATE_CANONICALIZATION_H
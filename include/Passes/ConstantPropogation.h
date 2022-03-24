// TODO

#ifndef ARCVM_CONSTANT_PROPOGATION_H
#define ARCVM_CONSTANT_PROPOGATION_H

// constant folding and propogation

#include "Pass.h"
#include "Common.h"

namespace arcvm {

    struct WrappedIRValue {
        IRValue value;
        bool    is_constant;
    };

    class ConstantPropogation {
        public:
            void block_pass(Block* block);
            void function_pass(Function* function);
            void module_pass(Module* module);

        private:
            void process_function(Function*);
            void process_block(Block*);

            void propogate_constants(BasicBlock*);

            bool isConstant(WrappedIRValue value) {
                return value.is_constant;
            }

            bool isImmediate(IRValue value) {
                return value.type == IRValueType::immediate;
            }

            bool isReference(IRValue value) {
                return value.type == IRValueType::reference;
            }
    };

};

#endif //ARCVM_CONSTANT_PROPOGATION_H
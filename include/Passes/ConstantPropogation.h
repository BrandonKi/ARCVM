// TODO

#ifndef ARCVM_CONSTANT_PROPOGATION_H
#define ARCVM_CONSTANT_PROPOGATION_H

// constant folding and propogation

// TODO get rid of the weird *.value.value pattern

#include "Pass.h"
#include "Common.h"

namespace arcvm {

    struct WrappedIRValue {
        IRValue value;
        bool    is_constant = false;
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

            // TODO remove *.value.value pattern
            bool isValidValue(WrappedIRValue value) {
                return value.value.type != IRValueType::none;
            }
    };

};

#endif //ARCVM_CONSTANT_PROPOGATION_H
#ifndef ARCVM_IRINTERPRETER_H
#define ARCVM_IRINTERPRETER_H

#include "Common.h"

#include <unordered_map>
#include <array>

namespace arcvm {

struct IRLocation {
    Block* block;
    size_t basicblock_index;
};

class IRInterpreter {
  public:
    IRInterpreter(Module*);

    i32 run();

    void build_jump_table(Module*);
    i32 run_module(Module*);
    i32 run_entry_function();
    IRValue run_function(Function*, std::vector<IRValue>);
    IRValue run_block(Block*);
    IRValue run_basicblock(BasicBlock*);
    IRValue run_entry(Entry*);

  private:
    Module* module_;
    std::unordered_map<std::string, BasicBlock*> jump_table;
    std::unordered_map<std::string, Function*> function_table;
    std::vector<std::string> predecessor_stack;
    std::string current_block_name;
    std::string entrypoint_name;

    std::vector<std::array<IRValue, 100>> ir_register;

    i64 unpack(IRValue);

    inline std::string predecessor() {
        return predecessor_stack.back();
    }

    inline void add_predecessor(std::string name) {
        predecessor_stack.push_back(std::move(name));
    }

    inline void remove_predecessor() {
        if(!predecessor_stack.empty())
            predecessor_stack.pop_back();
    }
};

};

#endif
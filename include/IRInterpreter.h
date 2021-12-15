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

  private:
    Module* module_;
    std::unordered_map<std::string, BasicBlock*> jump_table;
    std::unordered_map<std::string, Function*> function_table;
    std::string entrypoint_name;

    std::vector<std::array<IRValue, 100>> ir_register;

    void build_jump_table(Module*);
    i32 run_module(Module*);
    i32 run_entry_function();
    IRValue run_function(Function*, std::vector<IRValue>);
    IRValue run_block(Block*);
    IRValue run_basicblock(BasicBlock*);
    IRValue run_entry(Entry*);
};

};

#endif
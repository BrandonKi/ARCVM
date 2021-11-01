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

    std::vector<std::array<Value, 100>> ir_register;

    void build_jump_table(Module*);
    i32 run_module(Module*);
    i32 run_entry_function();
    Value run_function(Function*, std::vector<Value>);
    Value run_block(Block*);
    Value run_basicblock(BasicBlock*);
    Value run_entry(Entry*);
};

};

#endif
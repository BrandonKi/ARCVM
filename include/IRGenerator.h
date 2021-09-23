#ifndef ARCVM_IRGENERATOR_H
#define ARCVM_IRGENERATOR_H

#include <vector>
#include <string>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

enum class Instruction : i8 {
    add,
    index,
    call,
    ret,
    alloc,
    load,
    store
};

enum class Type : i8 {
    none,
    ir_i8,
    ir_i16,
    ir_i32,
    ir_i64,
    ir_u8,
    ir_u16,
    ir_u32,
    ir_u64,
    ir_b1,
    ir_b8
};

enum class ValueType : i8 {
    none,
    pointer,
    reference,
    immediate
};

// FIXME does not work with float immediate values
struct Value {
    ValueType type;
    i64 value;    
};

struct Entry {
    i32 dest;
    Instruction instruction;
    std::vector<Value> arguments;
};

struct Label {
    std::string name;
};

struct BasicBlock {
    Label label;
    std::vector<Entry> entries;

    Value gen_inst(Instruction, Value);
    Value gen_inst(Instruction, std::vector<Value>);
};

struct Block {
    std::vector<BasicBlock> blocks;

    Label* gen_label(std::string);
    Value gen_inst(Instruction, Value);
    Value gen_inst(Instruction, std::vector<Value>);
};

enum class Attribute : i8 {
    entrypoint
};

struct Function {
    bool is_complete;
    std::vector<Type> parameters;
    Type return_type;
    std::vector<Attribute> attributes;

    Block block;

    void add_attribute(Attribute attribute) {
        attributes.push_back(attribute);
    }

    Label* gen_label(std::string);
    Value gen_inst(Instruction, Value);
    Value gen_inst(Instruction, std::vector<Value>);
    Value get_param(i32);
};

struct Module {
    std::vector<Function> functions;

    Function* gen_function_def(std::vector<Type>, Type);
    Function* gen_aggregate_def(std::vector<Type>);
    Function* gen_function_decl(std::vector<Type>, Type);
    Function* gen_aggregate_decl(std::vector<Type>);
};

// TODO make generating code from multiple threads more friendly
class IRGenerator {
  public:
    IRGenerator();

    Module* create_module();
    void link_modules();

  private:
    std::vector<Module> modules_;
};

#endif
#ifndef ARCVM_COMMON_H
#define ARCVM_COMMON_H

#define NOMINMAX
// FIXME this somehow breaks stuff
//#define WIN32_LEAN_AND_MEAN

#include <small_profiler.h>
#include <color_print.h>
#include <thread_pool.h>

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <concepts>
#include <cassert>


namespace arcvm {

// #define NO_PROFILE

// renaming makes replacing/disabling the profiler easier in the future if needed
#ifdef NO_PROFILE
#define ARCVM_PROFILE() (void)0
#define ARCVM_PROFILE_SCOPE(x) (void)0
#else
#define ARCVM_PROFILE() PROFILE()
#define ARCVM_PROFILE_SCOPE(x) PROFILE_SCOPE(x)
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

enum class Instruction : i8 {
    alloc,
    load,
    store,

    call,
    ret,

    phi,
    dup,
    index,

    br,
    brz,
    brnz,

    add,
    sub,
    mul,
    div,
    mod,
    bin_or,
    bin_and,
    bin_xor,
    lshift,
    rshift,
    lt,
    gt,
    lte,
    gte,
    eq,
    neq,

    neg,
};

static std::string to_string(Instruction instruction) {
    switch(instruction) {
        case Instruction::alloc:
            return "alloc";
        case Instruction::load:
            return "load";
        case Instruction::store:
            return "store";
        case Instruction::call:
            return "call";
        case Instruction::ret:
            return "ret";
        case Instruction::phi:
            return "phi";
        case Instruction::dup:
            return "dup";
        case Instruction::index:
            return "index";
        case Instruction::br:
            return "br";
        case Instruction::brz:
            return "brz";
        case Instruction::brnz:
            return "brnz";
        case Instruction::add:
            return "add";
        case Instruction::sub:
            return "sub";
        case Instruction::mul:
            return "mul";
        case Instruction::div:
            return "div";
        case Instruction::mod:
            return "mod";
        case Instruction::bin_or:
            return "bin_or";
        case Instruction::bin_and:
            return "bin_and";
        case Instruction::bin_xor:
            return "bin_xor";
        case Instruction::lshift:
            return "lshift";
        case Instruction::rshift:
            return "rshift";
        case Instruction::lt:
            return "lt";
        case Instruction::gt:
            return "gt";
        case Instruction::lte:
            return "lte";
        case Instruction::gte:
            return "gte";
        case Instruction::eq:
            return "eq";
        case Instruction::neq:
            return "neq";
        case Instruction::neg:
            return "neg";
        default:
            return "";
    }
}

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

static std::string to_string(Type type) {
    switch (type) {
        case Type::none:
            return "none";
        case Type::ir_i8:
            return "i8";
        case Type::ir_i16:
            return "i16";
        case Type::ir_i32:
            return "i32";
        case Type::ir_i64:
            return "i64";
        case Type::ir_u8:
            return "u8";
        case Type::ir_u16:
            return "u16";
        case Type::ir_u32:
            return "u32";
        case Type::ir_u64:
            return "u64";
        case Type::ir_b1:
            return "b1";
        case Type::ir_b8:
            return "b8";
        default:
            return "";
    }
}

// TODO overload for aggregate types
static i32 type_size(Type type) {
    switch (type) {
        case Type::none:
            return 0;
        case Type::ir_i8:
            return 1;
        case Type::ir_i16:
            return 2;
        case Type::ir_i32:
            return 4;
        case Type::ir_i64:
            return 8;
        case Type::ir_u8:
            return 1;
        case Type::ir_u16:
            return 2;
        case Type::ir_u32:
            return 4;
        case Type::ir_u64:
            return 8;
        case Type::ir_b1:
            return 1;
        case Type::ir_b8:
            return 1;
        default:
            return 0;
    }
}

enum class IRValueType : i8 { none, pointer, reference, immediate, type, fn_name, label };

// FIXME does not work with float immediate values
struct IRValue {
    IRValueType type;
    union {
        i64 value;
        uintptr_t pointer_value;
        Type type_value;
        std::string* str_value;
    };

    IRValue(): type{IRValueType::none} {}
    IRValue(IRValueType type): type(type) {}
    template <std::integral T>
    IRValue(IRValueType type, T value): type(type), value(value) {}
    template <std::integral T>
    IRValue(T value): type(IRValueType::immediate), value(value) {}
    IRValue(IRValueType type, void* ptr): type(type), pointer_value((uintptr_t)ptr) {}
    IRValue(IRValueType type, Type type_value): type(type), type_value(type_value) {}
    IRValue(Type type_value): type(IRValueType::type), type_value(type_value) {}
    IRValue(std::string* str): type(IRValueType::label), str_value(str) {}
};

struct Entry {
    IRValue dest;
    Instruction instruction;
    std::vector<IRValue> arguments;
};

struct Label {
    std::string name;
};
// TODO maybe have a pointer to the parent block?
struct BasicBlock {
    Label label;
    std::vector<Entry*> entries;
    i32& var_name;

    BasicBlock(std::string label_name, std::vector<Entry*> entries_, i32& var_name_):
        label{label_name}, entries{entries_}, var_name{var_name_} {}

    IRValue gen_inst(Instruction, IRValue);
    IRValue gen_inst(Instruction, std::vector<IRValue>);
};

struct Block {
    std::vector<BasicBlock*> blocks;
    i32 var_name = 0;
    i32 label_name = 0;
    i32 insertion_point = -1;

    void set_insertion_point(BasicBlock*);
    void set_insertion_point(std::string);
    void set_insertion_point(i32);
    BasicBlock* new_basic_block();
    BasicBlock* new_basic_block(std::string);
    BasicBlock* get_bblock() { return blocks[insertion_point]; }
    void gen_if(IRValue, BasicBlock*, BasicBlock*, BasicBlock*);
};

enum class Attribute : i8 { entrypoint };

static std::string to_string(Attribute attribute) {
    switch (attribute) {
        case Attribute::entrypoint:
            return "entrypoint";
        default:
            return "";
    }
}

struct Function {
    std::string name;
    std::vector<Type> parameters;
    Type return_type;
    std::vector<Attribute> attributes;

    //FIXME write constructors
    Block* block{new Block{{}, (i32)parameters.size()}};

    Block* get_block() { return block; }
    void add_attribute(Attribute attribute) { attributes.push_back(attribute); }
    IRValue get_param(i32);
};

struct Module {
    std::vector<Function*> functions;

    Function* gen_function_def(std::string, std::vector<Type>, Type);
    Function* gen_aggregate_def(std::string, std::vector<Type>);
};

struct CompiledModule {

};

// TODO use allocator
inline void replace_entry(std::vector<Entry*>& entries, size_t index, Entry new_entry) {
    delete entries[index];
    entries[index] = new Entry{new_entry};
}

inline void remove_entry(std::vector<Entry*>& entries, size_t index) {
    entries.erase(entries.begin() + index);
}


};


#endif
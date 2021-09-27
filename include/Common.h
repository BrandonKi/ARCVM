#ifndef ARCVM_COMMON_H
#define ARCVM_COMMON_H

// #include "IRGenerator.h"

#include <small_profiler.h>
#include <color_print.h>

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <concepts>
#include <cassert>

// renaming makes replacing the profiler easier in the future if needed
#define ARCVM_PROFILE() PROFILE()
#define ARCVM_PROFILE_SCOPE(x) PROFILE_SCOPE(x)

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

    index,

    add,
    sub,
    mul,
    div,
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
        case Instruction::index:
            return "index";
        case Instruction::add:
            return "add";
        case Instruction::sub:
            return "sub";
        case Instruction::mul:
            return "mul";
        case Instruction::div:
            return "div";
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

enum class ValueType : i8 { none, pointer, reference, immediate, type, table_index };

// FIXME does not work with float immediate values
struct Value {
    ValueType type;
    union {
        i64 value;
        uintptr_t pointer_value;
        Type type_value;
    };

    Value(): type{ValueType::none} {}
    Value(ValueType type): type(type) {}
    template <std::integral T>
    Value(ValueType type, T value): type(type), value(value) {}
    template <std::integral T>
    Value(T value): type(ValueType::immediate), value(value) {}
    Value(ValueType type, void* ptr): type(type), pointer_value((uintptr_t)ptr) {}
    Value(ValueType type, Type type_value): type(type), type_value(type_value) {}
    Value(Type type_value): type(ValueType::type), type_value(type_value) {}
};

struct Entry {
    Value dest;
    Instruction instruction;
    std::vector<Value> arguments;
};

struct Label {
    std::string name;
};

struct BasicBlock {
    Label label;
    std::vector<Entry> entries;

    Value gen_inst(Instruction, Value, i32&);
    Value gen_inst(Instruction, std::vector<Value>, i32&);
};

struct Block {
    std::vector<BasicBlock> blocks;
    i32 var_name = 0;

    Label* gen_label(std::string);
    Value gen_inst(Instruction, Value);
    Value gen_inst(Instruction, std::vector<Value>);
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
    bool is_complete;
    std::vector<Type> parameters;
    Type return_type;
    std::vector<Attribute> attributes;

    //FIXME write constructors
    Block block{{}, (i32)parameters.size()};

    void add_attribute(Attribute attribute) { attributes.push_back(attribute); }

    Label* gen_label(std::string);
    Value gen_inst(Instruction, Value);
    Value gen_inst(Instruction, std::vector<Value>);
    Value get_param(i32);
};

struct Module {
    std::vector<Function> functions;

    Function* gen_function_def(std::string, std::vector<Type>, Type);
    Function* gen_aggregate_def(std::string, std::vector<Type>);
    Function* gen_function_decl(std::string, std::vector<Type>, Type);
    Function* gen_aggregate_decl(std::string, std::vector<Type>);
};


#endif
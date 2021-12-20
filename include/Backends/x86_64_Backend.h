#ifndef ARCVM_x86_64_BACKEND_H
#define ARCVM_x86_64_BACKEND_H

#include "Common.h"

#include "Backend.h"

namespace arcvm {

namespace x86_64 {

using byte = u8;

enum class Opcode {
    mov,
    lea,
    add,
    sub,
    div,
    idiv,
    mul,
    imul,
    ret,
    call,
    cmp,
    je,
    jne,
    jmp,
    test,
    xor_,
    nop,
    push,
    pop,
    int3
};

enum class Register : byte {
    rax,
    rcx,
    rdx,
    rbx,
    rbp,
    rsp,
    rsi,
    rdi,
    r8,
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15
};

struct Displacement {
    i32 val;
};

struct Immediate {
    i32 val;
};

enum class ValueType : i8 {
    NONE,
    DISPLACEMENT,
    REGISTER,
    IMMEDIATE
};

struct Value {
    using enum ValueType;

    ValueType type;
    union {
        i32 disp;
        Register reg;
        i32 imm;
    };

    Value(): type(NONE), disp(0) {};
    Value(i32 disp): type(DISPLACEMENT), disp(disp) {}
    Value(Register reg): type(REGISTER), reg(reg) {}
    Value(ValueType type, i32 imm): type(REGISTER), imm(imm) {}
};

}

constexpr byte rex_prefix = 0x40;
constexpr byte rex_w = 0x48;
constexpr byte rex_r = 0x44;
constexpr byte rex_x = 0x42;
constexpr byte rex_b = 0x41;

class x86_64_Backend {

  public:
    void compile_module(Module*);
    void compile_function(Function*);
    void compile_block(Block*);
    void compile_basicblock(BasicBlock*);
    int compile_entry(Entry*);


  private:
    // TODO use registers
    //std::vector<Register>

    // TODO need a vector/list/stack of these
    std::array<x86_64::Value, 100> val_table;
    std::vector<byte> output;

    void emit_mov(x86_64::Displacement, x86_64::Immediate, i8);
    void emit_mov(x86_64::Register, x86_64::Displacement, i8);
    void emit_mov(x86_64::Register, x86_64::Register, i8);


    void emit_lea();

    void emit_add();

    void emit_sub();
    void emit_div();
    void emit_idiv();
    void emit_mul();
    void emit_imul();

    void emit_ret();

    void emit_call();
    void emit_cmp();
    void emit_je();
    void emit_jne();
    void emit_jmp();
    void emit_test();
    void emit_xor();
    void emit_nop();
    void emit_push();
    void emit_pop();

    void emit_int3();

    byte rex(bool w, bool r, bool x, bool b);
    byte modrm(byte, byte, byte);

    // TODO use concepts
    template<typename T__, typename std::enable_if<
        std::is_integral<T__>::value || std::is_pointer<T__>::value || std::is_floating_point<T__>::value,
        bool
    >::type = true>
    inline void emit(const T__ val_) {

        using T_ =
            typename std::conditional<
                std::is_same<T__, bool>::value,
                uint8_t,
                typename std::conditional<
                    std::is_pointer<T__>::value,
                    uintptr_t,
                    T__
                >::type
            >::type;

        T_ val;
        if constexpr (std::is_pointer<T__>::value)
            val = reinterpret_cast<T_>(val_);
        else
            val = static_cast<T_>(val_);

        using type =
            typename std::conditional<
                std::is_signed<T_>::value,
                typename std::conditional<
                    std::is_floating_point<T_>::value,
                    typename std::conditional<
                        std::is_same<float, T_>::value && sizeof(float) == 4,
                        uint32_t,
                        typename std::conditional<
                            std::is_same<double, T_>::value && sizeof(double) == 8,
                            uint64_t,
                            void
                        >::type
                    >::type,
                    T_
                >::type,
                T_
            >::type;

            static_assert(!std::is_same<void, type>::value, "Only 4 and 8 byte floating point types are supported");
            auto raw_val = std::bit_cast<type>(val);
            // now that we are past all the template stuff we get to the actual code
            // just pushing bytes into a vector
            for (size_t i = 0; i < sizeof(type); ++i)
                output.push_back(static_cast<unsigned char>((raw_val >> (i * 8)) & 0xff));
        }
};

}

#endif // ARCVM_x86_64_BACKEND_H
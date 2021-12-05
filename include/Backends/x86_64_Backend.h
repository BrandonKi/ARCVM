#ifndef ARCVM_x86_64_BACKEND_H
#define ARCVM_x86_64_BACKEND_H

#include "Common.h"

#include "Backend.h"

namespace arcvm {

namespace x86_64 {

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

enum class Register {
    rax,
    rbx,
    rcx,
    rdx,
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

}

class x86_64_Backend {

  using byte = u8;

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
    std::array<int, 100> disp_table;
    std::vector<byte> output;

    void emit_mov(i8 disp, i32 immediate, i8 bits) {
        switch(bits) {
            case 8:
                emit_mov8(disp, immediate);
                break;
            case 16:
                emit_mov16(disp, immediate);
                break;
            case 32:
                emit_mov32(disp, immediate);
                break;
            case 64:
                emit_mov64(disp, immediate);
                break;
            default:
                assert(false);
        }
    }

    void emit_mov8(i8 disp, i32 immediate) {
        emit<byte>(0xc6);
        emit<byte>(modrm(1, 5, 0));
        emit<i8>(disp);
        emit<i8>(0);
    }

    void emit_mov16(i8 disp, i32 immediate) {
        emit<byte>(0x66);
        emit<byte>(modrm(1, 5, 0));
        emit<i8>(disp);
        emit<i16>(0);
    }

    void emit_mov32(i8 disp, i32 immediate) {
        emit<byte>(0xc7);
        emit<byte>(modrm(1, 5, 0));
        emit<i8>(disp);
        emit<i32>(0);
    }

    void emit_mov64(i8 disp, i32 immediate) {
        emit<byte>(rex(1, 0, 0, 0));
        emit<byte>(0xc7);
        emit<byte>(modrm(1, 5, 0));
        emit<i8>(disp);
        emit<i32>(0);
    }

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
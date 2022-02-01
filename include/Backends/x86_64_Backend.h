#ifndef ARCVM_x86_64_BACKEND_H
#define ARCVM_x86_64_BACKEND_H

#include "Common.h"

#include "Backend.h"

#include "x86_64.h"
#include "ABI.h"

namespace arcvm {

constexpr byte rex_prefix = 0x40;
constexpr byte rex_w = 0x48;
constexpr byte rex_r = 0x44;
constexpr byte rex_x = 0x42;
constexpr byte rex_b = 0x41;

class x86_64_Backend {

  public:
    x86_64_Backend(x86_64::ABIType abi_type): abi{abi_type},
        free_volatile_registers{abi.volatile_register_list()},
        free_nonvolatile_registers{abi.nonvolatile_register_list()}
    {}

    i32 run();
    void write_file(x86_64::FileFormat);

    void compile_module(Module*);
    void compile_function(Function*);
    void compile_block(Block*);
    void compile_basicblock(BasicBlock*);
    int compile_entry(Entry*);


  private:
    x86_64::ABI abi;
    std::vector<x86_64::RegisterName> free_volatile_registers;
    std::vector<x86_64::RegisterName> free_nonvolatile_registers;

    // TODO need a vector/list/stack of these
    std::array<x86_64::Value, 100> val_table;
    std::vector<i32> disp_list;
    std::vector<byte> output;

    x86_64::RegisterName get_fvr() {
        if(free_volatile_registers.empty())
            assert(false);
        auto reg = free_volatile_registers.back();
        free_volatile_registers.pop_back();
        return reg;
    }

    x86_64::RegisterName get_fnvr() {
        if(free_nonvolatile_registers.empty())
            assert(false);
        auto reg = free_nonvolatile_registers.back();
        free_nonvolatile_registers.pop_back();
        return reg;
    }

    void put_fvr(x86_64::RegisterName reg) {
        free_volatile_registers.push_back(reg);
    }

    void put_fnvr(x86_64::RegisterName reg) {
        free_nonvolatile_registers.push_back(reg);
    }


    void emit_mov(x86_64::Displacement, x86_64::Immediate, i8);
    void emit_mov(x86_64::Register, x86_64::Immediate, i8);
    void emit_mov(x86_64::Register, x86_64::Displacement, i8);
    void emit_mov(x86_64::Register, x86_64::Register, i8);
    void emit_mov(x86_64::Displacement, x86_64::Register, i8);


    void emit_lea();

    void emit_add(x86_64::Displacement, x86_64::Displacement, i8);
    void emit_add(x86_64::Register, x86_64::Register, i8);
    void emit_add(x86_64::Register, x86_64::Immediate, i8);

    void emit_sub(x86_64::Displacement, x86_64::Displacement, i8);
    void emit_sub(x86_64::Register, x86_64::Register, i8);
    void emit_sub(x86_64::Register, x86_64::Immediate, i8);

    void emit_div();
    void emit_idiv();
    void emit_mul();
    void emit_imul(x86_64::Register, x86_64::Register, i8);

    void emit_ret();

    void emit_call();
    void emit_cmp();
    void emit_je();
    void emit_jne();
    void emit_jmp();
    void emit_test();
    void emit_or(x86_64::Register, x86_64::Register, i8);
    void emit_and(x86_64::Register, x86_64::Register, i8);
    void emit_xor(x86_64::Register, x86_64::Register, i8);
    void emit_nop();
    void emit_push(x86_64::Register);
    void emit_pop(x86_64::Register);

    void emit_int3();

    void emit_neg(x86_64::Register, i8);

    byte rex(bool w, bool r, bool x, bool b);
    byte modrm(byte, byte, byte);
    byte SIB(byte, byte, byte);

    i8 calc_op_size(x86_64::Register r1, x86_64::Register r2) {
        auto temp = std::max(r1.size, r2.size);
        return temp >= 32 ? temp : 32;
    }

    i8 calc_op_size(x86_64::Register r1) {
        return r1.size >= 32 ? r1.size : 32;
    }

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
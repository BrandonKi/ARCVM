#ifndef ARCVM_X86_64_H
#define ARCVM_X86_64_H

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

enum class RegisterName : byte {
    rax,
    rcx,
    rdx,
    rbx,
    rsp,
    rbp,
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

struct Register {
    RegisterName name;
    i8 size;

    Register(RegisterName name): name{name}, size{0} {}
    Register(RegisterName name, i8 size): name{name}, size{size} {}
};

inline byte encode(Register reg) {
    return byte(reg.name);
}

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
    Value(ValueType type, i32 imm): type(type), imm(imm) {}
};

enum class FileFormat : byte {
    raw_binary,
    pe,
    elf
};

}

}

#endif // ARCVM_X86_64_H

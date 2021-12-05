#include "Backends/x86_64_Backend.h"

#include <iomanip>

using namespace arcvm;
using namespace x86_64;
using byte = u8;

void x86_64_Backend::compile_module(Module* module) {
    for (auto* function : module->functions)
        compile_function(function);


    std::cout << std::hex;
    for(byte b: output)
        std::cout << std::setw(2) << std::setfill('0') << (int)b << " ";
    std::cout << std::dec << "\n";
}

void x86_64_Backend::compile_function(Function* function) {
    compile_block(function->block);
}

void x86_64_Backend::compile_block(Block* block) {
    for(auto* basicblock : block->blocks)
        compile_basicblock(basicblock);
}

void x86_64_Backend::compile_basicblock(BasicBlock* basicblock) {
    for(auto* entry : basicblock->entries)
        compile_entry(entry);
}

int x86_64_Backend::compile_entry(Entry* entry) {
    switch (entry->instruction) {
        case Instruction::alloc: {
            auto size = type_size(entry->arguments[0].type_value);
            auto disp = -size;
            disp_table[entry->dest.value] = disp;
            auto bits = size * 8;
            emit_mov(disp, 0, bits); // zero initialize I guess *shrug*
            break;
        }
        case Instruction::load: {
            break;
        }
        case Instruction::store: {
            break;
        }
        case Instruction::call: {
            break;
        }
        case Instruction::ret: {
            break;
        }
        case Instruction::br: {
            break;
        }
        case Instruction::brz: {
            break;
        }
        case Instruction::brnz: {
            break;
        }
        case Instruction::index: {
            break;
        }
        case Instruction::add: {
            break;
        }
        case Instruction::sub: {
            break;
        }
        case Instruction::mul: {
            break;
        }
        case Instruction::div: {
            break;
        }
        case Instruction::mod: {
            break;
        }
        case Instruction::bin_or: {
            break;
        }
        case Instruction::bin_and: {
            break;
        }
        case Instruction::bin_xor: {
            break;
        }
        case Instruction::lshift: {
            break;
        }
        case Instruction::rshift: {
            break;
        }
        case Instruction::lt: {
            break;
        }
        case Instruction::gt: {
            break;
        }
        case Instruction::lte: {
            break;
        }
        case Instruction::gte: {
            break;
        }
        case Instruction::eq: {
            break;
        }
        case Instruction::neq: {
            break;
        }
        case Instruction::log_or: {
            break;
        }
        case Instruction::log_and: {
            break;
        }
        case Instruction::log_xor: {
            break;
        }
        default:
            break;
    }
    return 0;
}

void x86_64_Backend::emit_lea() {

}

void x86_64_Backend::emit_add() {

}

void x86_64_Backend::emit_sub() {

}

void x86_64_Backend::emit_div() {

}

void x86_64_Backend::emit_idiv() {

}

void x86_64_Backend::emit_mul() {

}

void x86_64_Backend::emit_imul() {

}

void x86_64_Backend::emit_ret() {

}

void x86_64_Backend::emit_call() {

}

void x86_64_Backend::emit_cmp() {

}

void x86_64_Backend::emit_je() {

}

void x86_64_Backend::emit_jne() {

}

void x86_64_Backend::emit_jmp() {

}

void x86_64_Backend::emit_test() {

}

void x86_64_Backend::emit_xor() {

}

void x86_64_Backend::emit_nop() {

}

void x86_64_Backend::emit_push() {

}

void x86_64_Backend::emit_pop() {

}

void x86_64_Backend::emit_int3() {

}

byte x86_64_Backend::rex(bool w, bool r, bool x, bool b) {
    return 0b01000000 | (w << 3) | (r << 2) | (x << 1) | b;
}

byte x86_64_Backend::modrm(byte mod, byte rm, byte reg) {
    return u8(0) | ((mod & 3) << 6) | ((reg & 7) << 3) | (rm & 7);
}
#include "Backends/x86_64_Backend.h"

#include <iomanip>

#define D(x) Displacement(x)
#define I(x) Immediate(x)
#define local_disp disp_list.back()


using namespace arcvm;
using namespace x86_64;
using enum ValueType;

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
    disp_list.emplace_back(0);
    emit_mov(Register::rbp, Register::rsp, 64);
    compile_block(function->block);
    disp_list.pop_back();
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
        // TODO maybe take an option to zero intialize??
        case Instruction::alloc: {
            auto size = type_size(entry->arguments[0].type_value);
            auto disp = -size + local_disp;
            local_disp = disp;

            val_table[entry->dest.value] = Value{disp};
            auto num_bits = size * 8;
            // emit_mov(D(disp), I(0), num_bits);
            break;
        }
        case Instruction::load: {
            auto val = val_table[entry->arguments[0].value];

            assert(val.type != REGISTER);

            i32 size = 8;
            if(entry->arguments.size() > 1) {
                size = type_size(entry->arguments[1].type_value);
            }

            auto num_bits = size * 8;
            // TODO use a free regsiter
            auto reg = Register::rax;
            emit_mov(reg, D(val.disp), num_bits);
            val_table[entry->dest.value] = reg;
            break;
        }
        case Instruction::store: {

            Value val;
            if (entry->arguments[1].type == IRValueType::immediate) {
                val = Value{IMMEDIATE, i32(entry->arguments[1].value)};
            } else if (entry->arguments[1].type == IRValueType::reference) {
                val = val_table[entry->arguments[1].value];
            }

            i32 size = 8;
            if(entry->arguments.size() > 2) {
                size = type_size(entry->arguments[2].type_value);
            }

            auto disp = val_table[entry->arguments[0].value].disp;
            // val_table[entry->arguments[0].value] = Value{disp};
            auto num_bits = size * 8;

            if(val.type == REGISTER)
                emit_mov(D(disp), val.reg, num_bits);
            else // if immediate
                emit_mov(D(disp), I(val.imm), num_bits);

            break;
        }
        case Instruction::call: {
            break;
        }
        case Instruction::ret: {
            Value val;
            if(entry->arguments[0].type == IRValueType::reference)
                val = val_table[entry->arguments[0].value];

            switch(val.type) {
                case DISPLACEMENT:
                    emit_mov(Register::rax, D(val.disp), 64);
                    break;
                case REGISTER:
                    emit_mov(Register::rax, val.reg, 64);
                    break;
                default:
                    assert(false);
            }

            emit_ret();
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



void x86_64_Backend::emit_mov(Displacement disp, Immediate immediate, i8 bits) {
    auto& dv = disp.val;
    auto& imm = immediate.val;
    switch(bits) {
        case 8:
            emit<byte>(0xc6);
            emit<byte>(modrm(1, 5, 0));
            emit<i8>(dv);
            emit<i8>(imm);
            break;
        case 16:
            emit<byte>(0x66);
            emit<byte>(0xc7);
            emit<byte>(modrm(1, 5, 0));
            emit<i8>(dv);
            emit<i16>(imm);
            break;
        case 32:
            emit<byte>(0xc7);
            emit<byte>(modrm(1, 5, 0));
            emit<i8>(dv);
            emit<i32>(imm);
            break;
        case 64:
            emit<byte>(rex_w);
            emit<byte>(0xc7);
            emit<byte>(modrm(1, 5, 0));
            emit<i8>(dv);
            emit<i32>(imm);
            break;
        default:
            assert(false);
    }
}

void x86_64_Backend::emit_mov(Register reg, Displacement disp, i8 bits) {
    auto& dv = disp.val;
    switch(bits) {
        case 8:
            emit<byte>(0x8A);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        case 16:
            emit<byte>(0x66);
            emit<byte>(0x8B);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        case 32:
            emit<byte>(0x8B);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        case 64:
            emit<byte>(rex_w);
            emit<byte>(0x8B);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        default:
            assert(false);
    }
}

void x86_64_Backend::emit_mov(Register dest_reg, Register src_reg, i8 bits) {
    switch(bits) {
        case 8:
            emit<byte>(0x88);
            emit<byte>(modrm(3, byte(dest_reg), byte(src_reg)));
            break;
        case 16:
            emit<byte>(0x66);
            emit<byte>(0x89);
            emit<byte>(modrm(3, byte(dest_reg), byte(src_reg)));
            break;
        case 32:
            emit<byte>(0x89);
            emit<byte>(modrm(3, byte(dest_reg), byte(src_reg)));
            break;
        case 64:
            emit<byte>(rex_w);
            emit<byte>(0x89);
            emit<byte>(modrm(3, byte(dest_reg), byte(src_reg)));
            break;
        default:
            assert(false);
    }
}


void x86_64_Backend::emit_mov(Displacement disp, Register reg, i8 bits) {
    auto& dv = disp.val;
    switch(bits) {
        case 8:
            emit<byte>(0x88);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        case 16:
            emit<byte>(0x66);
            emit<byte>(0x89);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        case 32:
            emit<byte>(0x89);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        case 64:
            emit<byte>(rex_w);
            emit<byte>(0x89);
            emit<byte>(modrm(1, 5, byte(reg)));
            emit<byte>(dv);
            break;
        default:
            assert(false);
    }
}

/*
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
*/
void x86_64_Backend::emit_ret() {
    emit<byte>(0xC3);
}
/*
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

*/

// TODO add support for r8-15
void x86_64_Backend::emit_push(Register reg) {
    emit<byte>(0x50 + byte(reg));
}

// TODO add support for r8-15
void x86_64_Backend::emit_pop(x86_64::Register reg) {
    emit<byte>(0x58 + byte(reg));
}

void x86_64_Backend::emit_int3() {
    emit<byte>(0xCC);
}

byte x86_64_Backend::rex(bool w, bool r, bool x, bool b) {
    return rex_prefix | (w << 3) | (r << 2) | (x << 1) | int(b);
}

byte x86_64_Backend::modrm(byte mod, byte rm, byte reg) {
    return u8(0) | ((mod & 3) << 6) | ((reg & 7) << 3) | (rm & 7);
}
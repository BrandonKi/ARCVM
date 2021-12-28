#ifndef ARCVM_ABI_H
#define ARCVM_ABI_H

#include "x86_64.h"

namespace arcvm {
namespace x86_64 {

enum class ABIType : byte {
    windows_x64,
    linux_x64,
};

class ABI {
  public:
    ABI(ABIType abi_type): abi_type{abi_type} {}


    std::vector<Register> volatile_register_list() {
        if(abi_type == ABIType::windows_x64) {
            return {Register::r11, Register::r10, Register::r9, Register::r8, Register::rdx, Register::rcx, Register::rax};
        }
    }

    std::vector<Register> nonvolatile_register_list() {
        if(abi_type == ABIType::windows_x64) {
            // TODO use rbp as general purpose
            return {/*Register::rbp,*/ Register::rbx, Register::rdi, Register::rsi, Register::r12, Register::r13, Register::r14, Register::r15};
            return {Register::r15, Register::r14, Register::r13, Register::r12, Register::rsi, Register::rdi, Register::rbx, /*Register::rbp*/};
        }
    }

  private:
    ABIType abi_type;
};

}
}

#endif // ARCVM_ABI_H

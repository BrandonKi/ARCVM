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


    std::vector<RegisterName> volatile_register_list() {
        if(abi_type == ABIType::windows_x64) {
            return {RegisterName::r11, RegisterName::r10, RegisterName::r9, RegisterName::r8, RegisterName::rdx, RegisterName::rcx, RegisterName::rax};
        }
    }

    std::vector<RegisterName> nonvolatile_register_list() {
        if(abi_type == ABIType::windows_x64) {
            // TODO use rbp as general purpose
            return {RegisterName::r15, RegisterName::r14, RegisterName::r13, RegisterName::r12, RegisterName::rsi, RegisterName::rdi, RegisterName::rbx, /*RegisterName::rbp*/};
        }
    }

  private:
    ABIType abi_type;
};

}
}

#endif // ARCVM_ABI_H

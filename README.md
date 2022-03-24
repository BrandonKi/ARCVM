# ARCVM

replacement for https://github.com/BrandonKi/ARCVM-old

Compiler backend for https://github.com/BrandonKi/ARCANE

Provides a C++ API for IR generation. The IR is LLVM inspired three-address-code in SSA form. Currently has two main backends one being a simple interpreter and the other being a x86_64 backend which only works on windows so far.

## IR Type System
There are four main IR types: pointer, reference, immediate, and label.

Pointers represent an integer value that points to a block of memory called a provenance. If the pointer leaves it's provenance then the program is invalid. This will be useful for alias analysis and other optimizations. To create a new pointer into an existing provenance it must be registered by the previous existing pointer.

References are the main type that will be used for the whole IR. Each reference is essentially a virtual register that will get properly renamed/mapped to physical registers or may be elided completely.

Immediates are relatively self-explanatory. They represent a constant value and can be put into any instruction.

Labels have identical functionality to labels in assembly languages. They represent a location that can be jumped/branched to.

## examples

### add two numbers
Uses stack-allocated variables instead of just adding immediates.
```
[entrypoint]
define function main() -> i32 {
  #main
    %0 = alloc i32
    %1 = alloc i32
    %2 = alloc i32
    store %0, 10
    store %1, 10
    %3 = load %0
    %4 = load %1
    %5 = add %3, %4
    store %2, %5
    %6 = load %2
    ret %6
}
```

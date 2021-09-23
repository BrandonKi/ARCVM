
enum class Instruction : i8 {
    add,
    index,
    call,
    ret,
    alloc,
    load,
    store
};

enum class ValueType : i8 {
    none,
    pointer,
    reference,
    immediate
};

// FIXME does not work with float immediate values
struct Value {
    ValueType type;
    i64 value;    
};

struct Entry {
    i32 dest;
    Instruction instruction;
    std::vector<Value> arguments;
};

struct Block {
    std::vector<Entry> entries;
};

struct Module {
    std::vector<Block> blocks;
};

// TODO make generating code from multiple threads more friendly
class IRGenerator {
  public:
    IRGenerator();

    Module create_module();
    void use_module();
    void link_modules();

    void gen_label();
    Value gen_inst(Instruction, std::vector<Value>);
    Value get_param(i32);
    void gen_def();
    void gen_decl();

  private:
    std::vector<Module> modules;
};


/*



*/
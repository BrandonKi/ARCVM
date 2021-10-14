
//~ this design would allow for easier multithreaded IR generation
// since the IRGenerators have no dependency on the vm or each other
/*
IRGenerator gen;

...

IRModule m1 = gen.create_module();
gen.clear();

...

IRModule m2 = gen.create_module();

Arcvm vm;
vm.load_module(m1);
vm.load_module(m2);

// interpret
i32 ec = vm.run();

// create a binary file
vm.compile();

// jit compile and run
vm.jit();


*/

//~ currently there is no way to refer to basic blocks
// this is an issue because it makes generating control flow structures impossible
// for example statements like if-elseif-else-then, for, while, etc.
//
// maybe only let labels/basicblocks be created at function level
// then whatever code is calling a gen_inst will have to create explicity create a basic block
// this will then give them a way to reference the block
// problem solved
// then higher-level contructs can take basicblock pointers as arugments
// as long as the basicblocks are local to the current block there should be no issues
//
// this will also make phi nodes possible to implement
//
// something to think about:
// phi nodes can somewhat be "replaced" by pointers in some cases
// what do I want to do about this????
// obviously there will be a canonicalization pass
// however do I want to generate this instead of phi nodes by default?
// it could potentially make IR gen easier for the front end
// hmmm, actually a language without pointers may have a harder time trying to do this
// let's start with having users generate phi nodes
// it will allow for easier optimization in the backend and it's less to think about for the frontend

/*
// new function gen api
// this should handle label creation
create_basicblock()
// allow pushing/popping/reordering elements in the list???
get_basic_block_list()


// I don't see much of a reason to provide a public gen api for blocks
// at the moment it is purely used as a function body, so
// the function api can handle it

// new basic block api
basicblock->gen_inst(...);
// basicblock->gen_if(if, else, then);
basicblock->gen_if(BasicBlock, BasicBlock, BasicBlock);
// basicblock->gen_loop(header, body, then);
basicblock->gen_loop(BasicBlock, BasicBlock, BasicBlock);
*/

//~ an aritfact from the old design is the gen_function_def() function
// at this point it should just take a Block as a parameter
// at the moment it just handles the header stuff and let's
// the user generate the code directly

//~ Block::gen_if
/*

Value cond, BasicBlock* if_block, BasicBlock* else_block, BasicBlock* then_block

cond: codegen is inserted into the preceding BasicBlock

if_block: is jumped to when the cond is non-zero

else_block: is jumped to when cond is zero

then_block: is jumped to after if_block or else_block is executed

       fn(...)
       #fn
       ...
       #bb1
       ...
+---+--brz cond, #if_block1, #else_block
|   +->#if_block
|      ...
|  +---br #then_block1
+--+-->#else_block1
   |   ...
   | +-br #then_block1
   +-+>#then_block1
       ...
*/
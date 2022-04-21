#include "semantic.h"
#include "corelib.h"
#include "irruntime.h"
#include "list.h"

void builtin_symbol_init()
{
    // 内置指令read
    class_def read_def = {
        .func_def = {
            .type = class_int(),
            .params = new_list()}};
    class read_type = new_class(CLASS_FUNCTION, read_def);
    symbol read=new(struct symbol_t);
    read->type=read_type;
    read->flags.implemented=1;
    read->flags.builtin=BUILT_IN_READ;
    read->name="read";
    add_global_symbol(read);

    // 内置指令write
    class_def write_def = {
        .func_def = {
            .type = class_int(),
            .params = new_list_singleton(new_symbol(class_int(),"i",0,0))}};
    class write_type = new_class(CLASS_FUNCTION, write_def);
    symbol write=new(struct symbol_t);
    write->type=write_type;
    write->flags.implemented=1;
    write->flags.builtin=BUILT_IN_WRITE;
    write->name="write";
    add_global_symbol(write);
}
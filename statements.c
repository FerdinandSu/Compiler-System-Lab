#include "statements.h"

statement new_statement(statement_type type, list content)
{
    statement s = new (struct statement_t);
    s->content = content;
    s->type = type;
    return s;
}
statement_comp new_statement_comp(list content, symbol_table symbols){
    statement_comp sc=new(struct statement_comp_t);
    sc->type=STMT_COMP;
    sc->symbols=symbols;
    sc->content=content;
    return sc;
}
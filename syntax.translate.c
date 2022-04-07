#include "syntax.translate.h"
typedef struct Node synnode;
typedef synnode *nodeptr;

void translate_system_init()
{
    global_symbols = new_symbol_table();
}
symbol get_symbol(symbol_table slist, string name)
{
    size_t v_size;
    return ht_get(slist, name, strlen(name) + 1, &v_size);
}
symbol get_global_symbol(string name)
{
    return get_symbol(global_symbols, name);
}
int has_symbol(symbol_table slist, string name)
{
    return ht_contains(slist, name, strlen(name) + 1);
}
void add_symbol(symbol_table slist, symbol sym)
{
    size_t v_size;
    return ht_insert(slist, sym->name, strlen(sym->name) + 1,
                     sym, sizeof(symbol));
}
int has_global_symbol(string name)
{
    return has_symbol(global_symbols, name);
}
void add_global_symbol(symbol sym)
{
    add_symbol(global_symbols, sym);
}

symbol_type Specifier(nodeptr node)
{
    nodeptr head = node->children[0];
    // -> TYPE
    if (streql(head->type, "TYPE"))
    {
        return streql(head->value.strv, "int") ? type_int : type_float;
    }
    // -> StructSpecifier
    return StructSpecifier(head);
}

symbol_table DefList(nodeptr node)
{
    if (node->children_count == 1)
        return new_symbol_table();
    // Def DefList
    symbol_table r = DefList(node->children[1]);
    symbol_table sublist = Def(node->children[0]);
    symbol_table_enumerator e;
    for (e = create_symbol_table_enumerator(sublist);
         has_next_symbol_table_enumerator(e);
         move_next_symbol_table_enumerator(e))
    {
        symbol cur = get_current_symbol_table_enumerator(e);
        if (has_symbol(r, cur->name))
        {
            trans_err_x(3, node->line_num, cur->name, "Redefined variable");
        }
        else
        {
            add_symbol(r, cur);
        }
    }
    destroy_symbol_table_enumerator(e);
    destroy_symbol_table(sublist);

    return r;
}

symbol_table Def(nodeptr node)
{
    symbol_type type = Specifier(node->children[0]);
    nodeptr declist = node->children[1];
    return DecList(declist, type);
}

symbol_table DecList(nodeptr node, symbol_type type)
{
    symbol_table r = new_symbol_table();
    // -> Dec
    symbol dec = Dec(node->children[0], type);
    add_symbol(r, dec);
    // + COMMA DecList
    if (node->children_count == 3)
    {
        symbol_table sublist = DecList(node->children[2], type);
        symbol_table_enumerator e;
        for (e = create_symbol_table_enumerator(sublist);
             has_next_symbol_table_enumerator(e);
             move_next_symbol_table_enumerator(e))
        {
            symbol cur = get_current_symbol_table_enumerator(e);
            if (has_symbol(r, cur->name))
            {
                trans_err_x(3, node->line_num, cur->name, "Redefined variable");
            }
            else
            {
                add_symbol(r, cur);
            }
        }
        destroy_symbol_table_enumerator(e);
        destroy_symbol_table(sublist);
    }
    return r;
}

symbol Dec(nodeptr node, symbol_type type)
{
    // -> VarDec
    symbol var = VarDec(node->children[0], type);
    // + ASSIGNOP Exp
    if (node->children_count == 3)
    {
        expression rightValue = Exp(node->children[2]);
        if (!typeeql(type, rightValue->type))
            trans_err(5, node->line_num, "Type mismatched for assignment");
    }
    return var;
}

symbol VarDec(nodeptr node, symbol_type type)
{
    // -> ID
    if (node->children_count == 1)
    {
        return new_symbol(type,
                          node->children[0]->value.strv,
                          0, 0);
    }
    // -> VarDec LB INT RB
    symbol arr = VarDec(node->children[0], type);
    symbol_type subtype = typecpy(arr->type);
    symbol_type_def newtypedef = {
        .arr_def = {
            .size = node->children[2]->value.intv,
            .type = subtype}};
    arr->type = new_symbol_type(SYMBOL_SCHEMA_ARRAY, newtypedef);
    return arr;
}

symbol_type StructSpecifier(nodeptr node)
{
    // -> STRUCT OptTag LC DefList RC
    if (node->children_count == 5)
    {
        symbol_table studef = DefList(node->children[3]);
        symbol_type_def def = {.stu_def = studef};
        symbol_type r = new_symbol_type(
            SYMBOL_SCHEMA_STRUCT,
            def);
        int is_symbol = node->children[1] == empty_node;
        symbol stu = new_symbol(
            r,
            is_symbol ? strdup(node->children[1]->children[0]->value.strv) : empty_string,
            1, 1);
        if (is_symbol)
        {
            if (has_global_symbol(stu->name))
            {
                trans_err_x(16, node->line_num, stu->name, "Duplicated name");
            }
            else
            {
                add_global_symbol(stu);
            }
        }
        return r;
    }
    // STRUCT Tag
    string name = node->children[1]->children[0]->value.strv;
    if (!has_global_symbol(name))
    {
        trans_err_x(17, node->line_num, name, "Undefined structure");
        return NULL;
    }
    symbol origin = get_global_symbol(name);
    if (origin->type->schema != SYMBOL_SCHEMA_STRUCT)
    {
        trans_err_x(17, node->line_num, name, "Undefined structure");
        return NULL;
    }
    return origin->type;
}
expression Exp(nodeptr node)
{
}
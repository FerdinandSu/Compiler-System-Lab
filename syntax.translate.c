#include "syntax.translate.h"
typedef struct Node synnode;
typedef synnode *nodeptr;

void translate_system_init()
{
    global_symbols = new_symbol_table();
    scoped_symbols = new_stack();
    push_stack(scoped_symbols, global_symbols);
}
/**
 * @brief Get the global symbol object
 *
 * @param name
 * @return symbol
 */
symbol get_global_symbol(string name)
{
    return get_symbol(global_symbols, name);
}
/**
 * @brief
 *
 * @param name
 * @return int
 */
int has_global_symbol(string name)
{
    return has_symbol(global_symbols, name);
}
void add_global_symbol(symbol sym)
{
    add_symbol(global_symbols, sym);
}
/**
 * @brief Get the global symbol object
 *
 * @param name
 * @return symbol
 */
symbol get_global_symbol(string name)
{
    return get_symbol(global_symbols, name);
}
/**
 * @brief
 *
 * @param name
 * @return int
 */
int has_scoped_symbol(string name)
{
    return local_symbols && has_symbol(local_symbols, name);
}
symbol get_local_symbol(string name)
{
    return local_symbols ? get_symbol(local_symbols, name) : NULL;
}

int has_symbol_anywhere(string name)
{
    return has_scoped_symbol(name) ||
           has_global_symbol(name);
}
symbol get_scoped_symbol(string name)
{
    stack_enumerator e = create_stack_enumerator(scoped_symbols);
    symbol r;
    for (; has_next_stack_enumerator(e);
         move_next_stack_enumerator(e))
    {
        symbol_table cur = get_current_stack_enumerator(e);
        r = get_symbol(cur, name);
        if (r != NULL)
            break;
    }
    destroy_stack_enumerator(e);
    return r;
}
class Specifier(nodeptr node)
{
    nodeptr head = node->children[0];
    // -> TYPE
    if (streql(head->type, "TYPE"))
    {
        return streql(head->value.strv, "int") ? class_int : class_float;
    }
    // -> StructSpecifier
    return StructSpecifier(head);
}

statement_comp CompSt(nodeptr node){
    ASSERT_NOT_NULL(node);
}

statement Stmt(nodeptr node)
{
    ASSERT_NOT_NULL(node);
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
    class type = Specifier(node->children[0]);
    nodeptr declist = node->children[1];
    return DecList(declist, type);
}

symbol_table DecList(nodeptr node, class type)
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

symbol Dec(nodeptr node, class type)
{
    // -> VarDec
    symbol var = VarDec(node->children[0], type);
    // + ASSIGNOP Exp
    if (node->children_count == 3)
    {
        expression rightValue = Exp(node->children[2]);
        if (!clseql(type, rightValue->type))
            trans_err(5, node->line_num, "Type mismatched for assignment");
    }
    return var;
}

symbol VarDec(nodeptr node, class type)
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
    class subtype = clscpy(arr->type);
    class_def newtypedef = {
        .arr_def = {
            .size = node->children[2]->value.intv,
            .type = subtype}};
    arr->type = new_class(CLASS_ARRAY, newtypedef);
    return arr;
}

class StructSpecifier(nodeptr node)
{
    // -> STRUCT OptTag LC DefList RC
    if (node->children_count == 5)
    {
        symbol_table studef = DefList(node->children[3]);
        class_def def = {.stu_def = studef};
        class r = new_class(
            CLASS_STRUCT,
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
    if (origin->type->schema != CLASS_STRUCT)
    {
        trans_err_x(17, node->line_num, name, "Undefined structure");
        return NULL;
    }
    return origin->type;
}
typedef symbol (*symbol_table_accessor)(string);
expression ID(nodeptr node, symbol_table_accessor get_x_symbol)
{
    string idname = node->value.strv;
    symbol id = get_x_symbol(idname);
    if (id == NULL)
    {
        return NULL;
    }
    class type = id->type;
    return new_expression(
        OP_VAR, type, id->flags.readonly, new_list_singleton(id->name));
}
expression CONST(nodeptr node)
{
    return streql(node->type, "CONST_INT") ? new_const_expression(class_int(), &(node->value.intv)) : new_const_expression(class_float(), &(node->value.fltv));
}
expression parse_exp_single(nodeptr node)
{
    nodeptr head = node->children[0];
    // -> ID
    if (streql(head->type, "ID"))
    {
        expression id = ID(head, get_scoped_symbol);
        if (id == NULL)
        {
            trans_err_x(1, node->line_num, head->value.strv, "Undefined variable");
            return NULL;
        }
    }
    // -> CONST_INT | CONST_FLOAT
    return CONST(node);
}
expression parse_exp_tuple(nodeptr node)
{
    // -> NEG Exp | NOT Exp
    expression subex = Exp(node->children[1]);
    if (subex == NULL)
        return NULL;
    return new_normal_unary_expression(
        streql(node->children[0]->type, "NEG") ? OP_NEG : OP_NOT,
        subex->type, new_list_singleton(subex));
}
operator_type parse_operator(string name)
{
    if (streql(name, "ADD"))
        return OP_ADD;
    if (streql(name, "SUB"))
        return OP_SUB;
    if (streql(name, "MUL"))
        return OP_MUL;
    if (streql(name, "DIV"))
        return OP_DIV;
    if (streql(name, "AND"))
        return OP_AND;
    if (streql(name, "OR"))
        return OP_OR;
    if (streql(name, "GT"))
        return OP_GT;
    if (streql(name, "GE"))
        return OP_GE;
    if (streql(name, "LT"))
        return OP_LT;
    if (streql(name, "LE"))
        return OP_LE;
    if (streql(name, "EQ"))
        return OP_SUB;
    if (streql(name, "NEQ"))
        return OP_SUB;
}
class merge_type(operator_type op, class t1, class t2)
{
    switch (op)
    {
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_ASSIGN:
        return clseql(t1, t2) ? t1 : NULL;
    case OP_AND:
    case OP_OR:
        return clseql(t1, class_int()) && clseql(t2, class_int()) ? t1 : NULL;
    case OP_GT:
    case OP_LT:
    case OP_GE:
    case OP_LE:
    case OP_EQ:
    case OP_NEQ:
        return clseql(t1, t2) ? class_int() : NULL;
    default:
        break;
    }
}
/**
 * @brief 各种二元运算符 和 无参数调用、结构体访问、括号
 *
 * @param node
 * @return expression?
 */
expression parse_exp_triple(nodeptr node)
{
    nodeptr n1 = node->children[0];
    nodeptr n2 = node->children[1];
    nodeptr n3 = node->children[2];
    // -> LP Exp RP
    if (streql(n1->type, "LP"))
        return Exp(n2);
    // -> ID LP RP
    if (streql(n2->type, "LP"))
    {
        expression func_id = ID(n1, get_global_symbol);
        if (func_id == NULL)
        {
            trans_err_x(2, node->line_num, n1->value.strv, "Undefined function");
            return NULL;
        }
        if (func_id->type->schema != CLASS_FUNCTION)
        {
            trans_err_x(11, node->line_num, n1->value.strv, "Not a function");
            return NULL;
        }
        if (func_id->type->def.func_def.params->count != 0)
        {
            trans_err(9, n1->line_num, "ParamList not matched");
            return NULL;
        }
        return new_expression(OP_CALL, func_id->type->def.func_def.type, new_list_singleton(func_id));
    }
    expression lv = Exp(n1);
    if (lv == NULL)
        return NULL;
    if (streql(n2->type, "DOT"))
    {
        if (lv->type->schema != CLASS_STRUCT)
        {
            trans_err(13, n1->line_num, "Illegal use of \".\"");
            return NULL;
        }
        symbol_table origin_scope = local_symbols;
        local_symbols = lv->type->def.stu_def;
        expression stu_member_id = ID(n3, get_local_symbol);
        if (stu_member_id == NULL)
        {
            trans_err_x(14, n1->line_num, n3->value.strv, "Undefined field");
            return NULL;
        }
        // 恢复符号表
        local_symbols = origin_scope;
        return new_expression(
            OP_DOT, stu_member_id->type, 0, new_list_of(2, lv, stu_member_id));
    }
    // -> Exp ASSIGNOP Exp
    expression rv = Exp(n3);
    if (rv == NULL)
        return NULL;
    if (streql(n2->type, "ASSIGNOP"))
    {
        if (lv->flags.readonly)
        {
            trans_err(6, n2->line_num, "The left-hand side of an assignment must be a variable");
            return NULL;
        }
        class merged_type = merge_type(OP_ASSIGN, lv->type, rv->type);
        if (merged_type == NULL)
        {
            trans_err(5, n2->line_num, "Type mismatched for assignment");
            return NULL;
        }
        return new_normal_binary_expression(OP_ASSIGN, merged_type, lv, rv);
    }
    // 加减乘除，关系，逻辑运算符
    operator_type op = parse_operator(n2->type);
    class merged_type2 = merge_type(op, lv->type, rv->type);
    if (merged_type2 == NULL)
    {
        trans_err(7, n2->line_num, "Type mismatched for operands");
        return NULL;
    }
    return new_normal_binary_expression(op, merged_type2, lv, rv);
}
/**
 * @brief
 *
 * @param node
 * @return list<expression>?
 */
list Args(nodeptr node)
{

    list l = new_list();
    nodeptr node_it = node;
    for (; node_it->children_count == 3; node_it = node_it->children[2])
    {
        expression exp_it = Exp(node_it->children[0]);
        ASSERT_NOT_NULL(exp_it);
        add_list(l, exp_it);
    }
    expression exp = Exp(node_it->children[0]);
    ASSERT_NOT_NULL(exp);
    add_list(l, exp);
    return l;
}
int func_param_eql(list parameters, list arguments)
{
    if (parameters->count != arguments->count)
        return 0;
    list_enumerator params;
    list_enumerator args;
    int r = 1;
    for (
        params = create_enumerator(parameters),
       args = create_enumerator(arguments);
        has_next_enumerator(params);
        move_next_enumerator(params),
       move_next_enumerator(args))
    {
        symbol param = get_current_enumerator(params);
        expression arg = get_current_enumerator(args);
        if (!clseql(param->type, arg->type))
        {
            r = 0;
            break;
        }
    }
    destroy_enumerator(params);
    destroy_enumerator(args);
    return r;
}
/**
 * @brief 数组和有参数调用
 *
 * @param node
 * @return expression
 * 函数调用子式：expression<id> + list<expression>
 * 数组访问子式：expression + expression
 */
expression parse_exp_quadruple(nodeptr node)
{
    nodeptr n1 = node->children[0];
    nodeptr n2 = node->children[1];
    nodeptr n3 = node->children[2];
    // -> ID LP Args RP
    if (streql(n2->type, "LP"))
    {
        expression func_id = ID(n1, get_global_symbol);
        if (func_id == NULL)
        {
            trans_err_x(2, node->line_num, n1->value.strv, "Undefined function");
            return NULL;
        }
        if (func_id->type->schema != CLASS_FUNCTION)
        {
            trans_err_x(11, node->line_num, n1->value.strv, "Not a function");
            return NULL;
        }
        list args = Args(n2);
        ASSERT_NOT_NULL(args);
        if (!func_param_eql(func_id->type->def.func_def.params, args))
        {
            trans_err(9, n1->line_num, "ParamList not matched");
            return NULL;
        }

        return new_expression(OP_CALL, func_id->type->def.func_def.type, 1, new_list_of(2, func_id, args));
    }
    // -> Exp LB Exp RB
    expression arr = Exp(n1);
    ASSERT_NOT_NULL(arr);
    if (arr->type->schema != CLASS_ARRAY)
    {
        trans_err(10, n1->line_num, "Not an array");
        return NULL;
    }
    expression idx = Exp(n3);
    ASSERT_NOT_NULL(idx);
    if (!clseql(idx->type, class_int()))
    {
        trans_err(12, n3->line_num, "Array index is not an integer");
        return NULL;
    }
    return new_expression(OP_ARR, arr->type->def.arr_def.type, 0, new_list_of(arr, idx));
}
expression Exp(nodeptr node)
{
    switch (node->children_count)
    {
    case 1:
        // 常量和标识符
        return parse_exp_single(node);
    case 2:
        // 各种一元运算符
        return parse_exp_triple(node);
    case 3:
        // 各种二元运算符 和 无参数调用、结构体访问、括号
        return parse_exp_triple(node);
    case 4:
        // 数组和有参数调用
        return parse_exp_quadruple(node);
    default:
        trans_err(-1, node->line_num, "Unknown Expression.");
        return NULL;
    }
}
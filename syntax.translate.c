#include "syntax.translate.h"
#include "utils/stack.h"
#include "utils/list.h"
#include "utils/expressions.h"
#include "utils/symbols.h"
#include "utils/statements.h"
#include "utils/corelib.h"

extern struct Node *empty_node;

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

symbol get_scoped_symbol(string name)
{
    stack_enumerator e = create_stack_enumerator(scoped_symbols);
    symbol r;
    for (; has_next_stack_enumerator(e);
         move_next_stack_enumerator(e))
    {
        symbol_table cur = (symbol_table)get_current_stack_enumerator(e);
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
        return streql(head->value.strv, "int") ? class_int() : class_float();
    }
    // -> StructSpecifier
    return StructSpecifier(head);
}
list StmtList(nodeptr node)
{
    list l = new_list();
    for (nodeptr node_it = node;
         node_it->children_count == 2;
         node_it = node_it->children[1])
    {
        statement stmt = Stmt(node_it->children[0]);
        ASSERT_NOT_NULL(stmt);
        add_list(l, stmt);
    }
    return l;
}
statement_comp CompSt(nodeptr node, symbol_table base_symbols)
{
    ASSERT_NOT_NULL(node);
    symbol_table comp_symbols = DefList(node->children[1], base_symbols);
    ASSERT_NOT_NULL(comp_symbols);
    push_stack(scoped_symbols, comp_symbols);
    list stmtlist = StmtList(node->children[2]);
    pop_stack(scoped_symbols);
    ASSERT_NOT_NULL(stmtlist);
    return new_statement_comp(stmtlist, comp_symbols);
}
/**
 * @brief
 *
 * @param node
 * @return list<symbol>
 */
symbol FunDec(nodeptr node, class ret_type)
{

    list varlist =
        node->children_count == 4 ? VarList(node->children[2]) : new_list();
    ASSERT_NOT_NULL(varlist);
    class_def cdef = {
        .func_def = {
            .params = varlist,
            .type = ret_type}};
    class fclass = new_class(CLASS_FUNCTION, cdef);
    return new_symbol(fclass, node->children[0]->value.strv, 1, 0);
}
/**
 * @brief
 *
 * @param node
 * @return list<symbol>
 */
list VarList(nodeptr node)
{
    list l = new_list();
    symbol_table st = new_symbol_table();
    nodeptr node_it = node;
    for (;
         node_it->children_count == 3;
         node_it = node_it->children[2])
    {
        symbol param = ParamDec(node_it->children[0]);
        ASSERT_NOT_NULL(param);
        if (has_symbol(st, param->name))
        {
            trans_err_x(3, node_it->line_num, param->name);
            return NULL;
        }
        add_symbol(st, param);
        add_list(l, param);
    }
    symbol param = ParamDec(node_it->children[0]);
    ASSERT_NOT_NULL(param);
    if (has_symbol(st, param->name))
    {
        trans_err_x(3, node_it->line_num, param->name);
        return NULL;
    }
    add_symbol(st, param);
    add_list(l, param);
    destroy_symbol_table(st);
    return l;
}
void Program(nodeptr node)
{
    ExtDefList(node->children[0]);
}
void ExtDefList(nodeptr node)
{
    if (node == empty_node)
        return;
    ExtDef(node->children[0]);
    ExtDefList(node->children[1]);
}
int ExtDef(nodeptr node)
{
    class s = Specifier(node->children[0]);
    ASSERT_NOT_NULL(s);
    // -> Specifier SEM
    if (node->children_count == 2)
    {
        return 1;
    }
    // -> Specifier ExtDecList SEM
    if (streql(node->children[1]->type, "ExtDecList"))
    {
        ExtDecList(node->children[1], s);
        return 1;
    }
    // -> Specifier FunDec CompSt|SEM
    symbol func = FunDec(node->children[1], s);
    ASSERT_NOT_NULL(func);
    symbol origin = get_global_symbol(func->name);
    int is_implement = !streql(node->children[2]->type, "SEM");
    if (origin != NULL && (origin->flags.implemented || !is_implement))
    {
        trans_err_x(4, node->line_num, func->name);
        return NULL;
    }

    if (is_implement)
    {
        
        if (origin == NULL)
        {
            add_global_symbol(func);
            origin = get_global_symbol(func->name);
        }

        return_type = s;
        symbol_table args = new_symbol_table_from_list(func->type->def.func_def.params);
        statement_comp compst = CompSt(node->children[2], args);
        return_type = NULL;
        origin->flags.implemented = 1;
        origin->reference.func = compst;
    }
    else
    {
        func->reference.origin=node;
        add_global_symbol(func);
    }
    return 1;
}
void ExtDecList(nodeptr node, class s)
{
    nodeptr node_it = node;
    for (; node_it->children_count == 3; node_it = node_it->children[2])
    {
        ExtDec(node_it->children[0], s);
    }
    ExtDec(node_it->children[0], s);
}
int ExtDec(nodeptr node, class s)
{
    symbol sym = VarDec(node, s);
    ASSERT_NOT_NULL(sym);
    if (has_global_symbol(sym->name))
    {
        trans_err_x(3, node->line_num, sym->name);
        return NULL;
    }
    add_global_symbol(sym);
    return 1;
}
symbol ParamDec(nodeptr node)
{
    class type = Specifier(node->children[0]);
    ASSERT_NOT_NULL(type);
    return VarDec(node->children[1], type);
}
/**
 * @brief
 * 设定函数返回值类型。默认为空。
 * 使用时由调用Stmt()者维护。
 */
class return_type;
statement Stmt(nodeptr node)
{
    ASSERT_NOT_NULL(node);
    expression simple_exp;
    expression return_exp;
    switch (node->children_count)
    {
    case 1:
        // -> CompSt
        return (statement)CompSt(node->children[0], NULL);
    case 2:
        // -> Exp SEM
        simple_exp = Exp(node->children[0]);
        ASSERT_NOT_NULL(simple_exp);
        return new_statement(STMT_SIMPLE, new_list_singleton(simple_exp));
    case 3:
        // -> RETURN Exp SEM
        return_exp = Exp(node->children[1]);
        ASSERT_NOT_NULL(return_exp);
        if (return_type != NULL &&
            !clseql(return_type, return_exp->type))
        {
            trans_err(8, node->line_num);
            return NULL;
        }
        return new_statement(STMT_RETURN, new_list_singleton(return_exp));
    default:
        break;
    }
    // WHILE, IF
    expression cond_exp = Exp(node->children[2]);
    ASSERT_NOT_NULL(cond_exp);
    if (!clseql(cond_exp->type, class_int()))
    {
        trans_err(20, node->line_num);
        return NULL;
    }
    statement s1 = Stmt(node->children[4]);
    ASSERT_NOT_NULL(s1);
    if (node->children_count == 7)
    {
        statement s2 = Stmt(node->children[6]);
        ASSERT_NOT_NULL(s2);
        return new_statement(STMT_IF_ELSE, new_list_of(3, cond_exp, s1, s2));
    }
    if (streql(node->children[0]->type, "IF"))
    {
        return new_statement(STMT_IF, new_list_of(2, cond_exp, s1));
    }
    return new_statement(STMT_WHILE, new_list_of(2, cond_exp, s1));
}

symbol_table DefList(nodeptr node, symbol_table base_symbols)
{
    symbol_table r = base_symbols == NULL ? new_symbol_table() : base_symbols;
    if (node == empty_node)
        return r;
    nodeptr node_it;
    for (node_it = node;
         node_it != empty_node;
         node_it = node_it->children[1])
    {
        symbol_table sublist = Def(node_it->children[0]);
        ASSERT_NOT_NULL(sublist);
        symbol_table_enumerator e;
        for (e = create_symbol_table_enumerator(sublist);
             has_next_symbol_table_enumerator(e);
             move_next_symbol_table_enumerator(e))
        {

            symbol cur = get_current_symbol_table_enumerator(e);
            if (has_symbol(r, cur->name))
            {
                trans_err_x(3, node_it->line_num, cur->name);
                return NULL;
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

symbol_table Def(nodeptr node)
{
    class type = Specifier(node->children[0]);
    nodeptr declist = node->children[1];
    return DecList(declist, type);
}

symbol_table DecList(nodeptr node, class type)
{
    symbol_table r = new_symbol_table();
    nodeptr node_it = node;
    for (; node_it->children_count == 3; node_it = node_it->children[2])
    {
        symbol cur = Dec(node_it->children[0], type);
        ASSERT_NOT_NULL(cur);
        if (has_symbol(r, cur->name))
        {
            trans_err_x(3, node_it->line_num, cur->name);
        }
        else
        {
            add_symbol(r, cur);
        }
    }
    symbol cur = Dec(node_it->children[0], type);
    ASSERT_NOT_NULL(cur);
    if (has_symbol(r, cur->name))
    {
        trans_err_x(3, node->line_num, cur->name);
    }
    else
    {
        add_symbol(r, cur);
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
            trans_err(5, node->line_num);
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
        symbol_table studef = DefList(node->children[3], NULL);
        class_def def = {.stu_def = studef};
        class r = new_class(
            CLASS_STRUCT,
            def);
        int is_symbol = node->children[1] != empty_node;
        symbol stu = new_symbol(
            r,
            is_symbol ? strdup(node->children[1]->children[0]->value.strv) : "",
            1, 1);
        if (is_symbol)
        {
            if (has_global_symbol(stu->name))
            {
                trans_err_x(16, node->line_num, stu->name);
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
        trans_err_x(17, node->line_num, name);
        return NULL;
    }
    symbol origin = get_global_symbol(name);
    if (origin->type->schema != CLASS_STRUCT)
    {
        trans_err_x(17, node->line_num, name);
        return NULL;
    }
    return origin->type;
}
typedef symbol (*symbol_table_accessor)(string);
expression ExpID(nodeptr node, symbol_table_accessor get_x_symbol)
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
    // -> ID
    if (streql(node->type, "ID"))
    {
        expression id = ExpID(node, get_scoped_symbol);
        if (id == NULL)
        {
            trans_err_x(1, node->line_num, node->value.strv);
            return NULL;
        }
        return id;
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
        subex->type, subex);
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
    trans_err_x(0, 505, name);
    return OP_ADD;
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
        trans_err_x(0, 529, "merge_type");
        return NULL;
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
        expression func_id = ExpID(n1, get_scoped_symbol);
        if (func_id == NULL)
        {
            trans_err_x(2, node->line_num, n1->value.strv);
            return NULL;
        }
        if (func_id->type->schema != CLASS_FUNCTION)
        {
            trans_err_x(11, node->line_num, n1->value.strv);
            return NULL;
        }
        if (func_id->type->def.func_def.params->count != 0)
        {
            trans_err(9, n1->line_num);
            return NULL;
        }
        return new_expression(OP_CALL, func_id->type->def.func_def.type, 1, new_list_singleton(func_id));
    }
    expression lv = Exp(n1);
    if (lv == NULL)
        return NULL;
    if (streql(n2->type, "DOT"))
    {
        if (lv->type->schema != CLASS_STRUCT)
        {
            trans_err(13, n1->line_num);
            return NULL;
        }
        symbol_table origin_scope = local_symbols;
        local_symbols = lv->type->def.stu_def;
        expression stu_member_id = ExpID(n3, get_local_symbol);
        if (stu_member_id == NULL)
        {
            trans_err_x(14, n1->line_num, n3->value.strv);
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
            trans_err(6, n2->line_num);
            return NULL;
        }
        class merged_type = merge_type(OP_ASSIGN, lv->type, rv->type);
        if (merged_type == NULL)
        {
            trans_err(5, n2->line_num);
            return NULL;
        }
        return new_normal_binary_expression(OP_ASSIGN, merged_type, lv, rv);
    }
    // 加减乘除，关系，逻辑运算符
    operator_type op = parse_operator(n2->type);
    class merged_type2 = merge_type(op, lv->type, rv->type);
    if (merged_type2 == NULL)
    {
        trans_err(7, n2->line_num);
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
        expression func_id = ExpID(n1, get_scoped_symbol);
        if (func_id == NULL)
        {
            trans_err_x(2, node->line_num, n1->value.strv);
            return NULL;
        }
        if (func_id->type->schema != CLASS_FUNCTION)
        {
            trans_err_x(11, node->line_num, n1->value.strv);
            return NULL;
        }
        list args = Args(n3);
        ASSERT_NOT_NULL(args);
        if (!func_param_eql(func_id->type->def.func_def.params, args))
        {
            trans_err(9, n1->line_num);
            return NULL;
        }

        return new_expression(OP_CALL, func_id->type->def.func_def.type, 1, new_list_of(2, func_id, args));
    }
    // -> Exp LB Exp RB
    expression arr = Exp(n1);
    ASSERT_NOT_NULL(arr);
    if (arr->type->schema != CLASS_ARRAY)
    {
        trans_err(10, n1->line_num);
        return NULL;
    }
    expression idx = Exp(n3);
    ASSERT_NOT_NULL(idx);
    if (!clseql(idx->type, class_int()))
    {
        trans_err(12, n3->line_num);
        return NULL;
    }
    return new_expression(OP_ARR, arr->type->def.arr_def.type, 0, new_list_of(2, arr, idx));
}
expression Exp(nodeptr node)
{
    switch (node->children_count)
    {
    case 0:
        // 常量和标识符
        return parse_exp_single(node);
    case 1:
        // 常量和标识符
        return parse_exp_single(node->children[0]);
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
        trans_err(0, node->line_num);
        return NULL;
    }
}

void global_symbol_check()
{
    symbol_table_enumerator e;
    for (e = create_symbol_table_enumerator(global_symbols);
         has_next_symbol_table_enumerator(e);
         move_next_symbol_table_enumerator(e))
    {

        symbol cur = get_current_symbol_table_enumerator(e);
        if (cur->type->schema == CLASS_FUNCTION && cur->flags.implemented == 0)
        {
            trans_err_x(18, cur->reference.origin->line_num, cur->name);
        }
    }
    destroy_symbol_table_enumerator(e);
}
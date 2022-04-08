#pragma once

#include "mylib.h"

// typedef struct node_t
// {
//     char *type;
//     union
//     {
//         int intv;
//         double fltv;
//         char *strv;
//     } value;
//     int children_count;
//     struct node_t **children;
//     int line_num;
// } synnode;
typedef struct Node *nodeptr;

symbol_table global_symbols;
/**
 * @brief 默认为null，由调用者为其初始化和回收
 *
 */
symbol_table local_symbols;
stack scoped_symbols;
void translate_system_init();
void Program(nodeptr node);


class Specifier(nodeptr node);
list StmtList(nodeptr node);
statement_comp CompSt(nodeptr node, symbol_table base_symbols);
/**
 * @brief
 *
 * @param node
 * @return list<symbol>
 */
symbol FunDec(nodeptr node, class ret_type);
/**
 * @brief
 *
 * @param node
 * @return list<symbol>
 */
list VarList(nodeptr node);
void ExtDefList(nodeptr node);
int ExtDef(nodeptr node);
void ExtDecList(nodeptr node, class s);
int ExtDec(nodeptr node, class s);
symbol ParamDec(nodeptr node);
/**
 * @brief
 * 设定函数返回值类型。默认为空。
 * 使用时由调用Stmt()者维护。
 */
class return_type;
statement Stmt(nodeptr node);

symbol_table DefList(nodeptr node, symbol_table base_symbols);

symbol_table Def(nodeptr node);

symbol_table DecList(nodeptr node, class type);

symbol Dec(nodeptr node, class type);

symbol VarDec(nodeptr node, class type);

class StructSpecifier(nodeptr node);
typedef symbol (*symbol_table_accessor)(string);
expression ExpID(nodeptr node, symbol_table_accessor get_x_symbol);
expression CONST(nodeptr node);
expression parse_exp_single(nodeptr node);
expression parse_exp_tuple(nodeptr node);
operator_type parse_operator(string name);
class merge_type(operator_type op, class t1, class t2);

/**
 * @brief 各种二元运算符 和 无参数调用、结构体访问、括号
 *
 * @param node
 * @return expression?
 */
expression parse_exp_triple(nodeptr node);
/**
 * @brief
 *
 * @param node
 * @return list<expression>?
 */
list Args(nodeptr node);
int func_param_eql(list parameters, list arguments);
/**
 * @brief 数组和有参数调用
 *
 * @param node
 * @return expression
 * 函数调用子式：expression<id> + list<expression>
 * 数组访问子式：expression + expression
 */
expression parse_exp_quadruple(nodeptr node);
expression Exp(nodeptr node);
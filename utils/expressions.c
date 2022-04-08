#include "expressions.h"
#include <memory.h>
#include "list.h"
expression new_expression(
    operator_type op,
    class type,
    int readonly,
    list sub_expressions)
{
    expression ex = new (struct expression_t);
    if (readonly)
        ex->flags = readonly_flag();
    ex->op = op;
    ex->sub_expressions = sub_expressions;
    ex->type = type;
    return ex;
}

/**
 * @brief
 * 创建加减乘除，逻辑运算和关系运算的表达式；
 * 赋值表达式也可以如此创建
 *
 * @param op
 * @param e1
 * @param e2
 * @return expression
 */
expression new_normal_binary_expression(
    operator_type op,
    class type,
    expression e1, expression e2)
{
    list sub_ex = new_list_of(2, e1, e2);
    return new_expression(op, type, 1, sub_ex);
}
/**
 * @brief
 * 创建一元运算符的表达式
 * @param op
 * @param e
 * @return expression
 */
expression new_normal_unary_expression(
    operator_type op,
    class type,
    expression e)
{
    list sub_ex = new_list_singleton(e);
    return new_expression(op, type, 1, sub_ex);
}

expression new_const_expression(
    class type,
    object const_addr)
{
    list sub_ex = new_list_singleton(const_addr);
    return new_expression(OP_CONST, type, 1, sub_ex);
}
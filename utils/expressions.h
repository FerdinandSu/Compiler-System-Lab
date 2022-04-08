#pragma once

#include "list.h"
#include "corelib.h"

typedef struct element_t
{
	struct Node *origin;
	control_flags flags;
} element;

typedef enum operator_type_t
{
	OP_ASSIGN,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_AND,
	OP_OR,
	OP_GE,
	OP_LE,
	OP_GT,
	OP_LT,
	OP_EQ,
	OP_NEQ,
	/**
	 * @brief 括号
	 * 
	 */
	OP_PT,
	OP_NEG,
	OP_NOT,
	OP_CALL,
	OP_ARR,
	OP_DOT,
	OP_VAR,
	OP_CONST
} operator_type;

typedef struct expression_t
{
	operator_type op;
	class type;
	control_flags flags;
	/**
	 * @brief 子式，存的可能是常量、变量名或标识符。
	 *
	 */
	list sub_expressions;
} * expression;

expression new_expression(
	operator_type op,
	class type,
	int readonly,
	list sub_expressions);
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
	expression e1, expression e2);
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
	expression e);
expression new_const_expression(
	class type,
	object const_addr);
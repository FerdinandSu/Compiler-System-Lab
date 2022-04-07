#include "list.h"
#include "symbols.h"

typedef struct element_t
{
	struct Node *origin;
	int flags;
} element;

typedef enum operator_type_t
{
	OP_ASSIGN,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_OR,
	OP_GE,
	OP_LE,
	OP_GT,
	OP_LT,
	OP_EQ,
	OP_NEQ,
	OP_INP,
	OP_NEG,
	OP_NOT,
	OP_CALL,
	OP_ARR,
	OP_DOT,
	OP_NULL
} operator_type ;

typedef struct expression_t
{
	element e;
	operator_type op;
    symbol_type type;
	list sub_expressions;
} *expression;

expression new_expression();
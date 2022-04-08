#pragma once
#include "corelib.h"
#include "list.h"

typedef enum statement_type_t
{
    STMT_SIMPLE,
    STMT_COMP,
    STMT_RETURN,
    STMT_IF,
    STMT_IF_ELSE,
    STMT_WHILE
} statement_type;

typedef struct statement_t
{
    statement_type type;
    /**
     * @brief
     * list<statement|expression>
     */
    list content;
} * statement;
/**
 * @brief statement_comp
 *
 */
typedef struct statement_comp_t
{
    statement_type type;
    /**
     * @brief
     * list<statement>
     */
    list content;
    symbol_table symbols;
} * statement_comp;

statement new_statement(statement_type type, list content);
statement_comp new_statement_comp(list content, symbol_table symbols);

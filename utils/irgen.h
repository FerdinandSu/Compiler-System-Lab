#include "symbols.h"


/**
 * @brief
 *
 * @param st symbol_table symbol_table to generate intercode from
 * @return list<code>
 */
list ir_from_symbol_table(symbol_table st);

list ir_from_expression(expression exp);
/**
 * @brief 表达式求值(左值)
 * 值或地址会被置于exp->ref操作数中
 * @param exp
 * @return list
 */
list ir_from_expression_lv(expression exp);
/**
 * @brief 左值改右值
 * 值会被置于exp->ref操作数中
 * @param exp
 * @return list
 */
list ir_expression_lv_to_rv(list prepend, expression exp);
list ir_from_statement_comp(statement_comp block);
list ir_from_statement(statement s);
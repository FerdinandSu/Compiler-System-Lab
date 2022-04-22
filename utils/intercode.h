#pragma once

#include "corelib.h"
#include <stdlib.h>

#include <stdio.h>

/**
 * @brief 指令操作数
 *
 */
typedef enum code_oprand_type_t
{
    COT_VAR,
    /**
     * @brief 整数常量
     *
     */
     COT_CINT,
     COT_IRINT,
     /**
      * @brief 浮点常量
      *
      */
      COT_CFLT,
      COT_ADDR,
      COT_LABEL,
      COT_FUNC,
      COT_RELOP
} code_oprand_type;

/**
 * @brief 指令操作数
 *
 */
typedef union code_oprand_reference_t
{
    /**
     * @brief
     *
     */
     // symbol symbol_ref;
     /**
      * @brief 只有整形常量
      *
      */
    int int_const;
    char* name;
    int op;
    float flt_const;
} code_oprand_reference;

typedef struct code_oprand_t
{
    code_oprand_type type;
    code_oprand_reference ref;
} code_oprand_obj;

typedef code_oprand_obj* code_oprand;

code_oprand ir_op_one();

code_oprand ir_op_zero();

code_oprand ir_op_neq();

code_oprand ir_op_eq();

typedef enum code_type_t
{
    IR_LABEL,
    IR_FUNC,
    IR_ASN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    /**
     * @brief x <- &y
     *
     */
     IR_ADDR,
     /**
      * @brief x <- *y
      *
      */
      IR_GET,
      /**
       * @brief *x <- y
       *
       */
       IR_SET,
       IR_GOTO,
       IR_IF,
       IR_RET,
       IR_ALLOC,
       IR_ARG,
       IR_CALL,
       IR_PARAM,
       IR_READ,
       IR_WRITE,
       /**
        * @brief 空行...
        * 
        */
       IR_EMPTY
} code_type;

typedef struct code_t
{
    code_type type;
    code_oprand oprands[4];
} *code;
void fprint_code(FILE* f, code c);
//code_oprand ir_one();
code_oprand new_ir_var();
code_oprand new_user_var(string name);
code_oprand new_code_operand(code_oprand_type type, ...);
code_oprand new_ir_label();
code_oprand new_ir_ptr();
// code new_ir_func(string func_name);
// code new_ir_a
/**
 * @brief
 *
 * @param type
 * @param ... params code_oprand[]
 * @return code
 */
code new_code(code_type type, ...);

#include "intercode.h"
#include <stdarg.h>
#include "expressions.h"
#include <stdlib.h>
#include "mystring.h"

static code_oprand_obj ir_op_one_orgin = {
    .type = COT_CINT,
    .ref = {
        .int_const = 1
    }
};

code_oprand ir_op_one()
{
    return &ir_op_one_orgin;
}
static code_oprand_obj ir_op_zero_orgin = {
    .type = COT_CINT,
    .ref = {
        .int_const = 0
    }
};

code_oprand ir_op_zero()
{
    return &ir_op_zero_orgin;
}
static code_oprand_obj ir_op_neq_orgin = {
    .type = COT_RELOP,
    .ref = {
        .op = OP_NEQ
    }
};

code_oprand ir_op_neq()
{
    return &ir_op_neq_orgin;
}

static code_oprand_obj ir_op_eq_orgin = {
    .type = COT_RELOP,
    .ref = {
        .op = OP_EQ
    }
};

code_oprand ir_op_eq()
{
    return &ir_op_eq_orgin;
}

void fprint_code_operand(FILE* f, code_oprand op)
{
    if (f == NULL)
        f = stdout;
    switch (op->type)
    {
    case COT_CINT:
        fprintf(f, "#%d", op->ref.int_const);
        break;
    case COT_CFLT:
        fprintf(f, "#%f", op->ref.flt_const);
        break;
    case COT_IRINT:
        fprintf(f, "%d", op->ref.int_const);
        break;
    case COT_RELOP:
        switch (op->ref.op)
        {
        case OP_EQ:
            fprintf(f, "==");
            break;
        case OP_NEQ:
            fprintf(f, "!=");
            break;
        case OP_GT:
            fprintf(f, ">");
            break;
        case OP_GE:
            fprintf(f, ">=");
            break;
        case OP_LT:
            fprintf(f, "<");
            break;
        case OP_LE:
            fprintf(f, "<=");
            break;
        default:
            break;
        }
        break;
    default:
        fprintf(f, "%s", op->ref.name);
        break;
    }
}
void fprint_code(FILE* f, code c)
{
    if (f == NULL)
        f = stdout;

    switch (c->type)
    {
    case IR_LABEL:
        fprintf(f, "LABEL ");
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " :");
        break;
    case IR_FUNC:
        fprintf(f, "FUNCTION ");
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " :");
        break;
    case IR_ASN:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := ");
        fprint_code_operand(f, c->oprands[1]);
        break;
    case IR_ADD:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := ");
        fprint_code_operand(f, c->oprands[1]);
        fprintf(f, " + ");
        fprint_code_operand(f, c->oprands[2]);
        break;
    case IR_SUB:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := ");
        fprint_code_operand(f, c->oprands[1]);
        fprintf(f, " - ");
        fprint_code_operand(f, c->oprands[2]);
        break;
    case IR_MUL:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := ");
        fprint_code_operand(f, c->oprands[1]);
        fprintf(f, " * ");
        fprint_code_operand(f, c->oprands[2]);
        break;
    case IR_DIV:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := ");
        fprint_code_operand(f, c->oprands[1]);
        fprintf(f, " / ");
        fprint_code_operand(f, c->oprands[2]);
        break;
    case IR_ADDR:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := &");
        fprint_code_operand(f, c->oprands[1]);
        break;
    case IR_GET:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := *");
        fprint_code_operand(f, c->oprands[1]);
        break;
    case IR_SET:
        fprintf(f, "*");
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := ");
        fprint_code_operand(f, c->oprands[1]);
        break;
    case IR_GOTO:
        fprintf(f, "GOTO ");
        fprint_code_operand(f, c->oprands[0]);
        break;
    case IR_IF:
        fprintf(f, "IF ");
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " ");
        fprint_code_operand(f, c->oprands[1]);
        fprintf(f, " ");
        fprint_code_operand(f, c->oprands[2]);
        fprintf(f, " GOTO ");
        fprint_code_operand(f, c->oprands[3]);
        break;
    case IR_RET:
        fprintf(f, "RETURN ");
        fprint_code_operand(f, c->oprands[0]);
        break;
    case IR_ALLOC:
        fprintf(f, "DEC ");
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " ");
        fprint_code_operand(f, c->oprands[1]);
        break;
    case IR_ARG:
        fprintf(f, "ARG ");
        fprint_code_operand(f, c->oprands[0]);
        break;
    case IR_CALL:
        fprint_code_operand(f, c->oprands[0]);
        fprintf(f, " := CALL ");
        fprint_code_operand(f, c->oprands[1]);
        break;
    case IR_PARAM:
        fprintf(f, "PARAM ");
        fprint_code_operand(f, c->oprands[0]);
        break;
    case IR_READ:
        fprintf(f, "READ ");
        fprint_code_operand(f, c->oprands[0]);
        break;
    case IR_WRITE:
        fprintf(f, "WRITE ");
        fprint_code_operand(f, c->oprands[0]);
        break;
    default:
        break;
    }
    fprintf(f, "\n");
}
code_oprand new_code_operand(code_oprand_type type, ...)
{
    code_oprand op = new (struct code_oprand_t);
    op->type = type;
    va_list vaList;
    va_start(vaList, 1);
    switch (type)
    {
    case COT_CINT:
    case COT_IRINT:
        op->ref.int_const = va_arg(vaList, int);
        break;
    case COT_CFLT:
        op->ref.flt_const = (float)va_arg(vaList, double);
        break;
    case COT_RELOP:
        op->ref.op = va_arg(vaList, operator_type);
        break;
    default:
        op->ref.name = va_arg(vaList, char*);
        break;
    }
    return op;
}
code_oprand new_user_var(string name)
{
    string new_name = concat_string("u", name);
    return new_code_operand(COT_VAR, new_name);
}

int size_of_code(code_type type)
{
    switch (type)
    {
    case IR_EMPTY:
        return 0;
    case IR_LABEL:
    case IR_FUNC:
    case IR_GOTO:
    case IR_RET:
    case IR_ARG:
    case IR_PARAM:
    case IR_READ:
    case IR_WRITE:
        return 1;
    case IR_ASN:
    case IR_ADDR:
    case IR_GET:
    case IR_SET:
    case IR_ALLOC:
    case IR_CALL:
        return 2;
    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        return 3;
    case IR_IF:
    default:
        return 4;
    }
}
/**
 * @brief
 *
 * @param type
 * @param ... params code_oprand[]
 * @return code
 */
code new_code(code_type type, ...)
{
    code c = new (struct code_t);
    c->type = type;
    va_list vl;
    int count = size_of_code(type);
    va_start(vl, count);
    for (int i = 0; i < count; i++)
    {
        c->oprands[i] = va_arg(vl, code_oprand);
    }
    va_end(vl);
    // if (c->oprands[0]->type == COT_ADDR)
    // {
    //     //是不是所有给地址[赋值]的情况，
    //     TODO
    // }
    return c;
}

static int __ir_label_count = 0;

code_oprand new_ir_label()
{
    string name = new_string(4);
    sprintf(name, "L%d", __ir_label_count++);
    code_oprand op = new_code_operand(COT_LABEL, name);
    return op;
}

static int __ir_var_count = 0;

code_oprand new_ir_var()
{
    string name = new_string(5);
    sprintf(name, "v%d", __ir_var_count++);
    code_oprand op = new_code_operand(COT_VAR, name);
    return op;
}
static int __ir_ptr_count = 0;
code_oprand new_ir_ptr()
{
    string name = new_string(5);
    sprintf(name, "p%d", __ir_ptr_count++);
    code_oprand op = new_code_operand(COT_ADDR, name);
    return op;
}
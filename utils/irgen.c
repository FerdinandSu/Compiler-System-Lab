#include "symbols.h"
#include "irgen.h"
#include "list.h"
#include "intercode.h"
#include "irruntime.h"

/**
 * @brief 从变量声明生成中间代码
 *
 * @param var
 * @return list
 */
list ir_from_var_daclaration(symbol var)
{
    list l = var->type->schema == CLASS_STANDARD ? new_list() :
        new_list_singleton(
        new_code(IR_ALLOC,
        new_user_var(var->name),
        new_code_operand(COT_IRINT,
        size_of_class(var->type)))
        );
    if (var->reference.var != null)
    {
        concat_list(l, ir_from_expression(var->reference.var));
    }
    return l;
}
/**
 * @brief 从函数声明生成中间代码
 *
 * @param def
 * @return list
 */
list ir_from_function(symbol def)
{
    // 跳过内置函数
    if (def->flags.builtin)return new_list();
    list r = new_list();
    add_list(r, new_code(IR_FUNC,
             new_code_operand(COT_FUNC, def->name)));
    list_enumerator e = create_enumerator(def->type->def.func_def.params);

    for (;has_next_enumerator(e);
         move_next_enumerator(e))
    {
        symbol param = get_current_enumerator(e);
        add_list(r, new_code(IR_PARAM,
                 new_user_var(param->name)));
    }
    list function_body = ir_from_statement_comp(def->reference.func);
    r = concat_list(r, function_body);
    return append_list(r, new_code(IR_EMPTY));
}
/**
 * @brief 从一系列的符号生成中间代码快
 *
 * @param symbols
 * @return list
 */
list ir_from_symbol_list(list symbols)
{
    list l = new_list();
    list_enumerator e;
    for (e = create_enumerator(symbols);
         has_next_enumerator(e);
         move_next_enumerator(e))
    {
        symbol s = get_current_enumerator(e);
        switch (s->type->schema)
        {
        case CLASS_FUNCTION:
            l = concat_list(l, ir_from_function(s));
            continue;
        case CLASS_STRUCT:
            if (s->flags.readonly)
            {
                // 结构体定义
                continue;
            }
            /* !!! 这里故意没有 break; */
        default:
            l = concat_list(l, ir_from_var_daclaration(s));
            continue;
        }
    }
    destroy_enumerator(e);
    return l;
}

list ir_from_symbol_table(symbol_table st)
{
    return ir_from_symbol_list(
        symbol_table_to_list(st)
    );
}


static inline expression first_sub_ex(expression exp)
{
    return list_first(exp->sub_expressions);
}
static inline expression second_sub_ex(expression exp)
{
    return list_second(exp->sub_expressions);
}

static inline expression last_sub_ex(expression exp)
{
    return list_last(exp->sub_expressions);
}

list ir_from_expression_single(expression exp)
{
    object last = last_sub_ex(exp);
    switch (exp->op)
    {
    case OP_VAR:
        exp->ref = new_user_var(cast(string, last));
        break;
    default:
        exp->ref = exp->type->def.type == TYPE_INT ?
            new_code_operand(COT_CINT, *cast(int*, last)) :
            new_code_operand(COT_CFLT, *cast(float*, last));
        break;
    }
    // 自动内联变量和常量表达式
    return new_list();
}


list ir_from_expression_tuple(expression exp)
{
    expression sub_ex = first_sub_ex(exp);
    list sub_ex_code = ir_from_expression(sub_ex);
    int is_neg = exp->op == OP_NEG;
    // 内联常量
    if (sub_ex->op == OP_CONST)
    {
        exp->ref = is_neg ?
            new_code_operand(
            sub_ex->ref->type,
            -(sub_ex->ref->type == COT_CINT ?
            sub_ex->ref->ref.int_const :
            sub_ex->ref->ref.flt_const)) :
            (IS_TRUE(sub_ex->ref->ref.int_const) ?
             ir_op_zero() :
             ir_op_one());
        exp->flags.ir_inline = 1;
        return sub_ex_code;
    }

    code_oprand var = new_ir_var();
    exp->ref = var;
    exp->flags.ir_inline = 1;
    if (is_neg)
    {
        return append_list(
            sub_ex_code,
            new_code(IR_SUB, var, ir_op_zero(), sub_ex->ref
        ));
    }

    //OP_NOT
    code_oprand if_true = new_ir_label();
    code_oprand next = new_ir_label();
    return concat_list(
        sub_ex_code,
        new_list_of(6,
        new_code(IR_IF,
        sub_ex->ref, ir_op_neq(), ir_op_one(), if_true),
        new_code(IR_ASN, var, ir_op_one()),
        new_code(IR_GOTO, next),
        new_code(IR_LABEL, if_true),
        new_code(IR_ASN, var, ir_op_one()),
        new_code(IR_LABEL, next)
    ));
}
code_type code_type_cast_arithmetic_op(operator_type op)
{
    switch (op)
    {
    case OP_ADD:
        return IR_ADD;
    case OP_SUB:
        return IR_SUB;
    case OP_MUL:
        return IR_MUL;
    case OP_DIV:
        return IR_DIV;
    default:
        throw("Invalid Argument", "code_type_cast_arithmetic_op");
        return IR_ADD;
    }
}
/**
 * @brief 翻译左结合表达式，包含逻辑、关系和算术运算
 *
 * @param exp
 * @return list<code>
 */
list ir_from_expression_triple_lassoc(expression exp)
{
    expression left_xp = first_sub_ex(exp);
    expression right_xp = last_sub_ex(exp);
    list left = ir_from_expression(left_xp);
    list right = ir_from_expression(right_xp);
    code_oprand var = new_ir_var();
    exp->ref = var;
    exp->flags.ir_inline = 1;
    operator_type op_type = exp->op;
    left = concat_list(left, right);
    switch (op_type)
    {
    case OP_ADD:
    case OP_SUB:
    case OP_DIV:
    case OP_MUL:
        return append_list(
            left,
            new_code(code_type_cast_arithmetic_op(op_type),
            var,
            left_xp->ref,
            right_xp->ref)
        );
    case OP_EQ:
    case OP_NEQ:
    case OP_GT:
    case OP_GE:
    case OP_LT:
    case OP_LE:
    {
        code_oprand if_true = new_ir_label();
        code_oprand next = new_ir_label();
        return concat_list(
            left,
            new_list_of(6,
            new_code(
            IR_IF,
            left_xp->ref,
            new_code_operand(COT_RELOP, op_type),
            right_xp->ref, if_true),
            new_code(IR_ASN, var, ir_op_zero()),
            new_code(IR_GOTO, next),
            new_code(IR_LABEL, if_true),
            new_code(IR_ASN, var, ir_op_one()),
            new_code(IR_LABEL, next)
        ));
    }

    case OP_AND:
    {
        code_oprand if_first_true = new_ir_label();
        code_oprand if_second_true = new_ir_label();
        code_oprand if_false = new_ir_label();
        code_oprand next = new_ir_label();
        return concat_list(
            left,
            new_list_of(10,
            new_code(
            IR_IF,
            left_xp->ref,
            ir_op_neq(),// if lv is true
            ir_op_zero(), if_first_true), // goto if_first_true
            new_code(IR_LABEL, if_false),// if_false:
            new_code(IR_ASN, var, ir_op_zero()),// ret false
            new_code(IR_GOTO, next),// goto next
            new_code(IR_LABEL, if_first_true),// if_first_true:
            new_code(
            IR_IF,
            right_xp->ref,
            ir_op_neq(),// if rv is true
            ir_op_zero(), if_second_true), // goto if_second_true
            new_code(IR_GOTO, if_false), // else goto if_false
            new_code(IR_LABEL, if_second_true),// if_second_true:
            new_code(IR_ASN, var, ir_op_one()),// ret false
            new_code(IR_LABEL, next) // next:
        ));
    }

    case OP_OR:
    {
        code_oprand if_first_false = new_ir_label();
        code_oprand if_second_false = new_ir_label();
        code_oprand if_true = new_ir_label();
        code_oprand next = new_ir_label();
        return concat_list(
            left,
            new_list_of(10,
            new_code(
            IR_IF,
            left_xp->ref,
            ir_op_eq(),// if lv is false
            ir_op_zero(), if_first_false), // goto if_first_false
            new_code(IR_LABEL, if_true),// if_true:
            new_code(IR_ASN, var, ir_op_one()),// ret true
            new_code(IR_GOTO, next),// goto next
            new_code(IR_LABEL, if_first_false),// if_first_false:
            new_code(
            IR_IF,
            right_xp->ref,
            ir_op_eq(),// if rv is false
            ir_op_zero(), if_second_false), // goto if_second_false
            new_code(IR_GOTO, if_true), // else goto if_true
            new_code(IR_LABEL, if_second_false),// if_second_true:
            new_code(IR_ASN, var, ir_op_zero()),// ret false
            new_code(IR_LABEL, next) // next:
        ));
    }

    default:
        throw(NotSupportedException, "ir_from_expression_triple_lassoc");
        return new_list();
    }

}
/**
 * @brief 调用内置指令
 *
 * @param exp
 * @return list
 */
list ir_from_expression_call_builtin(expression exp)
{

    switch (exp->flags.builtin)
    {
    case BUILT_IN_READ:
    {
        code_oprand var = new_ir_var();
        exp->ref = var;
        exp->flags.ir_inline = 1;
        return new_list_singleton(
            new_code(IR_READ, var)
        );
    }

    case BUILT_IN_WRITE:
        exp->ref = ir_op_zero();
        exp->flags.ir_inline = 1;
        expression subex = last_sub_ex(exp);
        list subex_code = ir_from_expression(subex);
        return append_list(
            subex_code,
            new_code(IR_WRITE, subex->ref)
        );
    default:
        throw(NotSupportedException, "ir_from_expression_call_builtin");
        return new_list();
    }
}

list ir_from_expression_call(expression exp)
{
    if (exp->flags.builtin)
        return ir_from_expression_call_builtin(exp);
    expression func_id_exp = first_sub_ex(exp);
    string func_id = list_first(func_id_exp->sub_expressions);
    list args = list_last(exp->sub_expressions);
    /**
     * @brief 反向枚举器
     *
     */
    list_enumerator e = create_reverse_enumerator(args);
    list r = new_list();
    for (;has_next_enumerator(e);
         move_next_reverse_enumerator(e))
    {
        expression arg_ex = get_current_enumerator(e);
        list arg_code = ir_from_expression(arg_ex);
        r = append_list(
            concat_list(r, arg_code),
            new_code(IR_ARG, arg_ex->ref)
        );
    }
    destroy_enumerator(e);
    code_oprand return_value = new_ir_var();
    exp->flags.ir_inline = 1;
    exp->ref = return_value;
    return append_list(
        r,
        new_code(IR_CALL, return_value,
        new_code_operand(COT_FUNC, func_id))
    );
}
/**
 * @brief 解析数组索引，并返回其左值操作数引用(地址)
 *
 * @param exp
 * @return list
 */
list ir_from_expression_array(expression exp)
{
    expression array_exp = first_sub_ex(exp);
    expression offset_exp = last_sub_ex(exp);
    list array_code = ir_from_expression_lv(array_exp);
    list offset_code = ir_from_expression(offset_exp);
    code_oprand arr_ref = array_exp->ref;
    if (arr_ref->type == COT_VAR)
    {
        code_oprand base_addr = new_ir_ptr();
        add_list(array_code,
                 new_code(IR_ADDR, base_addr, arr_ref));
        arr_ref = base_addr;
    }
    exp->ref = arr_ref;
    code_oprand offset_var = new_ir_var();
    return append_list(append_list(
        concat_list(array_code, offset_code),
        new_code(IR_MUL, offset_var, offset_exp->ref,
        new_code_operand(COT_CINT, size_of_class(exp->type)))),
        new_code(IR_ADD, arr_ref, arr_ref, offset_var));
}
/**
 * @brief
 * 解析结构体成员表达式，并返回其左值操作数引用(地址)
 *
 * @param exp
 * @return list
 */
list ir_from_expression_struct(expression exp)
{
    expression stu = first_sub_ex(exp);
    expression member_id_exp = last_sub_ex(exp);
    string member_id = list_first(member_id_exp->sub_expressions);
    list left = ir_from_expression_lv(stu);
    symbol_table stu_def = stu->type->def.stu_def;
    // 计算偏移
    symbol member = get_symbol(stu_def, member_id);
    int sym_index = member->index;
    int offset = 0;
    symbol_table_enumerator e;
    for (e = create_symbol_table_enumerator(stu_def);
         has_next_symbol_table_enumerator(e);
         move_next_symbol_table_enumerator(e))
    {
        symbol s = get_current_symbol_table_enumerator(e);
        if (sym_index > s->index) { offset += size_of_class(s->type); }
    }
    destroy_symbol_table_enumerator(e);
    code_oprand sturef = stu->ref;
    if (sturef->type == COT_VAR)
    {
        code_oprand base_addr = new_ir_ptr();
        add_list(left,
                 new_code(IR_ADDR, base_addr, sturef));
        sturef = base_addr;
    }
    add_list(left,
             new_code(IR_ADD, sturef, sturef,
             new_code_operand(COT_CINT, offset)));
    exp->ref = sturef;
    return left;
}
list ir_from_expression_assign(expression exp)
{
    expression left_xp = first_sub_ex(exp);
    expression right_xp = last_sub_ex(exp);
    list right = ir_from_expression(right_xp);
    list left = ir_from_expression_lv(left_xp);
    left = concat_list(left, right);
    code_oprand rref = right_xp->ref;
    code_oprand lref = left_xp->ref;
    exp->ref = rref;
    // 只允许内联变量赋值
    int lref_is_var = lref->type == COT_VAR;
    if (lref_is_var && right_xp->flags.ir_inline)
    {
        // 消除右值变量的内联
        rref->ref = lref->ref;
        // 这段不必要，因为可内联的右值一定是变量
        // rref->type = lref->type; 
        return left;
    }

    // 不能内联的情况

    return append_list(
        left,
        new_code(
        lref_is_var ? IR_ASN : IR_SET,
        lref, rref)
    );
}
/**
 * @brief 表达式求值(右值)
 * 值会被置于exp->ref操作数中
 * @param exp
 * @return list 求值所需的代码
 */
list ir_from_expression(expression exp)
{
    switch (exp->op)
    {
    case OP_ASSIGN:
        return ir_from_expression_assign(exp);
    case OP_CONST:
    case OP_VAR:
        return ir_from_expression_single(exp);
    case OP_ARR:
    case OP_DOT:
        return ir_expression_lv_to_rv(
            ir_from_expression_lv(exp), exp
        );
    case OP_CALL:
        return ir_from_expression_call(exp);
    case OP_NOT:
    case OP_NEG:
        return ir_from_expression_tuple(exp);
    default:
        return ir_from_expression_triple_lassoc(exp);
    }
}

list ir_from_expression_lv(expression exp)
{
    switch (exp->op)
    {
    case OP_VAR:
        return ir_from_expression_single(exp);
    case OP_ARR:
        return ir_from_expression_array(exp);
    case OP_DOT:
        return ir_from_expression_struct(exp);
    default:
        throw(NotSupportedException, "ir_from_expression_lv");
        return new_list();
    }
}
/**
 * @brief 左值改右值
 * 值会被置于exp->ref操作数中
 * @param exp
 * @return list
 */
list ir_expression_lv_to_rv(list prepend, expression exp)
{
    code_oprand var = new_ir_var();
    code_oprand addr = exp->ref;
    exp->ref = var;
    exp->flags.ir_inline = 1;
    return append_list(
        prepend,
        new_code(IR_GET, var, addr)
    );
}

list ir_from_statement_comp(statement_comp block)
{
    list r = ir_from_symbol_table(block->symbols);
    list_enumerator e = create_enumerator(block->content);
    for (;has_next_enumerator(e);
         move_next_enumerator(e))
    {
        statement s = get_current_enumerator(e);
        r = concat_list(r, ir_from_statement(s));
    }
    destroy_enumerator(e);
    return r;
}

list ir_from_statement(statement s)
{
    statement_type styp = s->type;
    if (styp == STMT_COMP)
    {
        return ir_from_statement_comp(cast(statement_comp, s));
    }
    expression ex = list_first(s->content);
    list ex_code = ir_from_expression(ex);
    if (styp == STMT_SIMPLE)return ex_code;
    if (styp == STMT_RETURN)return append_list(
        ex_code,
        new_code(IR_RET, ex->ref)
    );
    code_oprand if_true = new_ir_label();
    code label_if_true = new_code(IR_LABEL, if_true);
    code if_true_goto_if_true =
        new_code(IR_IF, ex->ref, ir_op_neq(), ir_op_zero(), if_true);
    code_oprand next = new_ir_label();
    code goto_next = new_code(IR_GOTO, next);
    code label_next = new_code(IR_LABEL, next);
    list if_true_code = ir_from_statement(list_second(s->content));
    if (styp == STMT_IF)
    {
        return concat_list(
            ex_code,
            concat_list(
            new_list_of(3,
            if_true_goto_if_true,
            goto_next,
            label_if_true
        ),
            append_list(
            if_true_code,
            label_next))

        );
    }
    if (styp == STMT_WHILE)
    {
        code_oprand get_exp = new_ir_label();
        code label_get_exp = new_code(IR_LABEL, get_exp);
        code goto_get_exp = new_code(IR_GOTO, get_exp);

        return concat_list(
            prepend_list(label_get_exp, ex_code),
            concat_list(
            new_list_of(3,
            if_true_goto_if_true,
            goto_next,
            label_if_true
        ),
            concat_list(
            if_true_code,
            new_list_of(2,
            goto_get_exp,
            label_next)
        ))
        );
    }
    list else_code = ir_from_statement(list_last(s->content));
    return concat_list(
        ex_code,
        concat_list(
        prepend_list(
        if_true_goto_if_true,
        concat_list
        (
        else_code,
        new_list_of(2,
        goto_next,
        label_if_true))),
        append_list(
        if_true_code,
        label_next))
    );

}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "utils/semantic.h"
#include "utils/irruntime.h"
#include "utils/irgen.h"
#include "utils/intercode.h"
//#include "syntax.tab.c"

#define YYDEBUG 0
#define LEXDEBUG 0

char* trans_errors[] = {
    "Unknown Expression",
    "Undefined variable",
    "Undefined function",
    "Redefined variable",
    "Redefined function",
    "Type mismatched for assignment",
    "The left-hand side of an assignment must be a variable",
    "Type mismatched for operands",
    "Type mismatched for return",
    "Arguments are not matched with parameters",
    "Not an array",
    "Not a function",
    "Array index is not an integer",
    "Illegal use of \".\"",
    "Undefined field",
    "Redefined field",
    "Duplicated name",
    "Undefined structure",
    "Function not implemented",
    "Conflicted function declaration",
    "Condition expression is not an integer" };

extern int max_line_num;
extern int yylineno;
nodeptr root;
int err_count = 0;

int main(int argc, char** argv)
{
    if (argc <= 1)
        return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    FILE* fw = argc > 2 ? fopen(argv[2], "wt+") : NULL;
    if (!fw)
    {
        printf(YELLOW"Output file not specified/failed to open, using stdout.\n"CLEAN);
    }
    yyrestart(f);
    yyparse();
    if (root == NULL)
    {
        printf(RED "Syntax Error.\n" CLEAN);
        return 1;
    }
    // cprintf(0,root);
    translate_system_init();
    builtin_symbol_init();
    Program(root);
    global_symbol_check();
    if (err_count == 0)
    {
        printf(GREEN "Sematic analysis Completed Successfully.\n" CLEAN);
    }
    list codes = ir_from_symbol_table(global_symbols);
    list_enumerator e = create_enumerator(codes);
    for (; has_next_enumerator(e);move_next_enumerator(e))
    {
        code c = get_current_enumerator(e);
        fprint_code(fw, c);
    }
    return 0;
}
void cprintf(int lv, nodeptr node)
{
    if (err_count || node == NULL || !strcmp(node->type, "EMPTY"))
    {
        return;
    }
    for (int i = 0; i < lv; i++)
    {
        printf("  ");
    }
    switch (lv % 5)
    {
    case 0:
        printf(BLUE);
        break;
    case 1:
        printf(GREEN);
        break;
    case 2:
        printf(MAGENTA);
        break;
    case 3:
        printf(CYAN);
        break;
    default:
        printf(YELLOW);
        break;
    }
    printf("%s", node->type);
    if (!strcmp(node->type, "ID") || !strcmp(node->type, "TYPE"))
    {
        printf(": %s", node->value.strv);
    }

    else if (!strcmp(node->type, "CONST_INT"))
    {
        printf(" : %d", node->value.intv);
    }
    else if (!strcmp(node->type, "CONST_FLOAT"))
    {
        printf(" : %lf", node->value.fltv);
    }
    printf(" (%d)\n" CLEAN, node->line_num);
    for (int i = 0; i < node->children_count; i++)
    {
        cprintf(lv + 1, node->children[i]);
    }
}
void lex_err(int ln, char* desc)
{
    err_count++;
    printf(RED "\033[31mError type A at Line %d: %s.\n" CLEAN, ln, desc);
}
void lex_err_x(int ln, char* expr, char* desc)
{
    err_count++;
    printf(RED "Error type A at Line %d: %s \"%s\".\n" CLEAN, ln, desc, expr);
}
void trans_err(int type, int ln)
{
    err_count++;
    printf(RED "Error type %d at Line %d: %s.\n" CLEAN, type, ln, trans_errors[type]);
}
void trans_err_x(int type, int ln, char* expr)
{
    err_count++;
    printf(RED "Error type %d at Line %d: %s \"%s\".\n" CLEAN, type, ln, trans_errors[type], expr);
}
void yyerror(const char* s)
{
    err_count++;
    printf(RED "Error type B at Line %d: Syntax Error.\n" CLEAN, max_line_num > yylineno ? max_line_num : yylineno);
}

void lex_log(int ln, char* lex_unit)
{
    if (LEXDEBUG)
    {
        printf("[%d] %s\n", ln, lex_unit);
    }
}
void lex_log_x(int ln, char* lex_unit, char* expr)
{
    if (LEXDEBUG)
    {
        printf("[%d] %s: %s\n", ln, lex_unit, expr);
    }
}

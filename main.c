#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "syntax.tab.c"

#define YYDEBUG 0
#define LEXDEBUG 0

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define CLEAN "\033[0m"

extern int max_line_num;
extern int yylineno;
extern struct Node* root;
int err_count=0;

int main(int argc, char** argv)
{
    if(argc<=1)return 1;
    FILE* f=fopen(argv[1],"r");
    if(!f){
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    cprintf(0,root);
    return 0;
}
void cprintf(int lv,struct Node* node)
{
    if (err_count||node == NULL||!strcmp(node->type, "EMPTY"))
    {
        return;
    }
    for (int i = 0 ; i < lv; i++)
    {
        printf("  ");
    }
    switch (lv%5)
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
    printf(" (%d)\n"CLEAN, node->line_num);
    for (int i = 0; i < node->children_count; i++)
    {
        cprintf(lv + 1,node->children[i]);
    }
}
void lex_err(int ln,char* desc)
{
    err_count++;
    printf(RED"\033[31mError type A at Line %d: %s.\n"CLEAN,ln,desc);
}
void lex_err_x(int ln, char* expr,char* desc)
{
    err_count++;
    printf(RED"Error type A at Line %d: %s \"%s\".\n"CLEAN,ln,desc,expr);
}
void trans_err(int type, int ln, char* desc)
{
    err_count++;
    printf(RED"\033[31mError type %d at Line %d: %s.\n"CLEAN,type,ln,desc);
}
void trans_err_x(int type, int ln, char* expr, char* desc)
{
    err_count++;
    printf(RED"Error type %d at Line %d: %s \"%s\".\n"CLEAN,type,ln,desc,expr);
}
void yyerror(const char* s)
{
    err_count++;
    printf(RED"Error type B at Line %d: Syntax Error.\n"CLEAN, max_line_num>yylineno?max_line_num:yylineno);
}

void lex_log(int ln,char* lex_unit){
    if(LEXDEBUG){
        printf("%s\n",lex_unit);
    }
}
void lex_log_x(int ln,char* lex_unit, char* expr){
    if(LEXDEBUG){
        printf("%s: %s\n",lex_unit,expr);
    }
}

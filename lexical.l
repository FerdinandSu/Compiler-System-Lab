
%option noyywrap yylineno

%{
#include <stdio.h>   
#include <stdlib.h>
#include <string.h>
#include "syntax.tab.h"
#pragma once
void lex_err(int ln,char* desc);
void lex_err_x(int ln,char* expr,char* desc);
void lex_log(int ln,char* lex_unit);
void lex_log_x(int ln,char* lex_unit, char* expr);
int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno;\
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
int eof_error = 0;
int length = 0;
int last_line = 0;
%}

%x COMMENT


binary 0[bB][01]+
octal (0[0-7]+)
decimal ([1-9][0-9]*|0)
hex 0[xX][0-9a-fA-F]+
realnum ([0-9]*\.[0-9]+|[0-9]+\.)([eE][\+\-]?[0-9]+)?
id [_a-zA-Z][_0-9a-zA-Z]{0,31}

%%

<INITIAL>"int"	{ length += yyleng; lex_log(yylineno,"int"); return INT; }
<INITIAL>"float"	{ length += yyleng; lex_log(yylineno,"float"); return FLOAT; }

<INITIAL>"while"	{ length += yyleng; lex_log(yylineno,"while"); return WHILE; }
<INITIAL>"if"	{ length += yyleng; lex_log(yylineno,"if"); return IF; }
<INITIAL>"else"	{ length += yyleng; lex_log(yylineno,"else"); return ELSE; }
<INITIAL>"return"	{ length += yyleng; lex_log(yylineno,"return"); return RETURN; }
<INITIAL>"struct"	{ length += yyleng; lex_log(yylineno,"struct"); return STRUCT; }
<INITIAL>">="	{ length += yyleng; lex_log(yylineno,">="); return GE; }
<INITIAL>"<="	{ length += yyleng; lex_log(yylineno,"<="); return LE; }
<INITIAL>"=="	{ length += yyleng; lex_log(yylineno,"=="); return EQ; }
<INITIAL>"!="	{ length += yyleng; lex_log(yylineno,"!="); return NEQ; }
<INITIAL>"||"	{ length += yyleng; lex_log(yylineno,"||"); return OR; }
<INITIAL>"&&"	{ length += yyleng; lex_log(yylineno,"&&"); return AND; }
<INITIAL>"("	{ length += yyleng; lex_log(yylineno,"("); return LP; }
<INITIAL>")"	{ length += yyleng; lex_log(yylineno,")"); return RP; }
<INITIAL>"["	{ length += yyleng; lex_log(yylineno,"[");return LB; }
<INITIAL>"]"	{ length += yyleng; lex_log(yylineno,"]");return RB; }
<INITIAL>"."	{ length += yyleng; lex_log(yylineno,"."); return DOT; }
<INITIAL>"*"	{ length += yyleng; lex_log(yylineno,"*"); return MUL; }
<INITIAL>"+"	{ length += yyleng; lex_log(yylineno,"+"); return ADD; }
<INITIAL>"-"	{ length += yyleng; lex_log(yylineno,"-"); return SUB; }
<INITIAL>"!"	{ length += yyleng; lex_log(yylineno,"!"); return NOT; }
<INITIAL>"/"	{ length += yyleng; lex_log(yylineno,"/"); return DIV; }
<INITIAL>">"	{ length += yyleng; lex_log(yylineno,">"); return GT; }
<INITIAL>"<"	{ length += yyleng; lex_log(yylineno,"<"); return LT; }
<INITIAL>"="	{ length += yyleng; lex_log(yylineno,"="); return ASSIGNOP; }
<INITIAL>","	{ length += yyleng; lex_log(yylineno,","); return COMMA; }
<INITIAL>"{"	{ length += yyleng; lex_log(yylineno,"{"); return LC; }
<INITIAL>"}"	{ length += yyleng; lex_log(yylineno,"}"); return RC; }
<INITIAL>";"	{ length += yyleng; lex_log(yylineno,";"); return SEM; }

<INITIAL>{binary}	{ yylval.intv = strtol(yytext + 2, (char**)NULL, 2);  lex_log_x(yylineno,"int_const",yytext); length += yyleng; return CONST_INT; }
<INITIAL>{hex}|{octal}|{decimal}			{ yylval.intv = strtol(yytext, (char**)NULL, 0); lex_log_x(yylineno,"int_const",yytext); length += yyleng; return CONST_INT; }
<INITIAL>{realnum}	{ yylval.fltv = atof(yytext);lex_log_x(yylineno,"float_const",yytext); length += yyleng; return CONST_FLOAT;}
<INITIAL>{id}	{ length += yyleng;lex_log_x(yylineno,"id",yytext); yylval.strv = strdup(yytext); return ID; }
<INITIAL>\n	{ yycolumn = 1; length += yyleng; }
<INITIAL>[\r\t ]+	{ length += yyleng; }
    /* 单行注释 */
<INITIAL>\/\/.*\n	{ length += yyleng; }
    /* 多行注释 */
<INITIAL>"/*"	{length += yyleng; last_line = yylineno;BEGIN COMMENT;}
<COMMENT>"*/"	{length += yyleng;BEGIN INITIAL;}
<COMMENT>.|\n|\r	{ yymore(); }
<COMMENT><<EOF>>	{ eof_error = 1; lex_err( yylineno,"Illegal Character:\\0"); BEGIN INITIAL; return 0;}
<INITIAL>[0-9a-zA-Z_]+|[^\n\r\t ]	{ length += yyleng; lex_err_x(yylineno,yytext,"Invalid Token");}

%%


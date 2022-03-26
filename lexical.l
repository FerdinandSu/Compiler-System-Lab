
%option noyywrap yylineno

%{
#include <stdio.h>   
#include <stdlib.h>
#include <string.h>
#include "syntax.tab.h"
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
%}

%x COMMENT


binary 0[bB][01]+
octal (0[0-7]+)
decimal ([1-9][0-9]*|0)
hex 0[xX][0-9a-fA-F]+
realnum ([0-9]*\.[0-9]+|[0-9]+\.)([eE][\+\-]?[0-9]+)?
id [_a-zA-Z][_0-9a-zA-Z]{0,31}

%%

<INITIAL>"int"	{ lex_log(yylineno,"int"); return INT; }
<INITIAL>"float"	{ lex_log(yylineno,"float"); return FLOAT; }

<INITIAL>"while"	{ lex_log(yylineno,"while"); return WHILE; }
<INITIAL>"if"	{ lex_log(yylineno,"if"); return IF; }
<INITIAL>"else"	{ lex_log(yylineno,"else"); return ELSE; }
<INITIAL>"return"	{ lex_log(yylineno,"return"); return RETURN; }
<INITIAL>"struct"	{ lex_log(yylineno,"struct"); return STRUCT; }
<INITIAL>">="	{ lex_log(yylineno,">="); return GE; }
<INITIAL>"<="	{ lex_log(yylineno,"<="); return LE; }
<INITIAL>"=="	{ lex_log(yylineno,"=="); return EQ; }
<INITIAL>"!="	{ lex_log(yylineno,"!="); return NEQ; }
<INITIAL>"||"	{ lex_log(yylineno,"||"); return OR; }
<INITIAL>"&&"	{ lex_log(yylineno,"&&"); return AND; }
<INITIAL>"("	{ lex_log(yylineno,"("); return LP; }
<INITIAL>")"	{ lex_log(yylineno,")"); return RP; }
<INITIAL>"["	{ lex_log(yylineno,"[");return LB; }
<INITIAL>"]"	{ lex_log(yylineno,"]");return RB; }
<INITIAL>"."	{ lex_log(yylineno,"."); return DOT; }
<INITIAL>"*"	{ lex_log(yylineno,"*"); return MUL; }
<INITIAL>"+"	{ lex_log(yylineno,"+"); return ADD; }
<INITIAL>"-"	{ lex_log(yylineno,"-"); return SUB; }
<INITIAL>"!"	{ lex_log(yylineno,"!"); return NOT; }
<INITIAL>"/"	{ lex_log(yylineno,"/"); return DIV; }
<INITIAL>">"	{ lex_log(yylineno,">"); return GT; }
<INITIAL>"<"	{ lex_log(yylineno,"<"); return LT; }
<INITIAL>"="	{ lex_log(yylineno,"="); return ASSIGNOP; }
<INITIAL>","	{ lex_log(yylineno,","); return COMMA; }
<INITIAL>"{"	{ lex_log(yylineno,"{"); return LC; }
<INITIAL>"}"	{ lex_log(yylineno,"}"); return RC; }
<INITIAL>";"	{ lex_log(yylineno,";"); return SEM; }

<INITIAL>{binary}	{ yylval.intv = strtol(yytext + 2, (char**)NULL, 2);  lex_log_x(yylineno,"int_const",yytext); return CONST_INT; }
<INITIAL>{hex}|{octal}|{decimal}			{ yylval.intv = strtol(yytext, (char**)NULL, 0); lex_log_x(yylineno,"int_const",yytext); return CONST_INT; }
<INITIAL>{realnum}	{ yylval.fltv = atof(yytext);lex_log_x(yylineno,"float_const",yytext); return CONST_FLOAT;}
<INITIAL>{id}	{ lex_log_x(yylineno,"id",yytext); yylval.strv = strdup(yytext); return ID; }
<INITIAL>\n	{ yycolumn = 1; }
<INITIAL>[\r\t ]+	{ }
    /* 单行注释 */
<INITIAL>\/\/.*\n	{ }
    /* 多行注释 */
<INITIAL>"/*"	{BEGIN COMMENT;}
<COMMENT>"*/"	{BEGIN INITIAL;}
<COMMENT>.|\n|\r	{ yymore(); }
<COMMENT><<EOF>>	{ lex_err( yylineno,"Illegal Character:\\0"); BEGIN INITIAL; return 0;}
<INITIAL>[0-9a-zA-Z_]+|[^\n\r\t ]	{ lex_err_x(yylineno,yytext,"Invalid Token");}

%%


   /*FloatExpr: FloatExpr ATMOP FloatExpr
    | IntExpr ATMOP FloatExpr
    | FloatExpr ATMOP FloatExpr
    | LP FloatExpr RP
    | CONST_FLOAT
    ;

IntExpr: IntExpr ATMOP IntExpr
    | LP IntExpr RP
    | CONST_INT
    ;*/

/*
statement: sentence ';'         { printf("STMT\n"); }
    | statement sentence ';'
    ;

sentence: decl_stmt     { $$ = $1; }
    | assign_stmt       { $$ = $1; }
    | /* empty rule */  { $$ = 0; }
    ;
/*

decl_stmt: type_dec id_list { printf("stmt_decl\n"); }
    | type_dec assign_stmt  { printf("stmt_decl & assignment\n"); }
    ;

id_list: IDENTITY           { printf("id: %s\n", $1); $$ = $1; }
    | id_list ',' IDENTITY  { printf("id: %s\n", $3); $$ = $3; }
    ;

assign_stmt: IDENTITY ASSIGN expr    { printf("id: %s\nASSIGNMENT\n", $1); }
    ;

expr: factor    { $$ = $1; }
    | bin_expr  { $$ = $1; }
    ;

factor: INT_NUMBER      { printf("VALUE: %d\n", $1); $$ = $1; }
    | FLOAT_NUMBER      { printf("VALUE: %d\n", $1); $$ = $1; }
    | IDENTITY          { printf("VALUE: %s\n", $1); $$ = $1; }
    ;

bin_expr: expr '+' expr     { printf("PLUS.\n"); }
    | expr '-' expr         { printf("SUB.\n"); }
    | expr '*' expr         { printf("MUL.\n"); }
    | expr '/' expr         { printf("DIV.\n"); }
    ;

type_dec: INT       { printf("TYPE:INT\n"); }
    | FLOAT         { printf("TYPE:FLOAT\n"); }
    | SHORT         { printf("TYPE:SHORT\n"); }
    | LONG          { printf("TYPE:LONG\n"); }
    | UNSIGNED LONG { printf("TYPE:UNSIGNED\n"); }
    ;
*/


    /* 字符串常量 */
    /* <INITIAL>\"	{ length += yyleng; last_line = yylineno; BEGIN STRING0; }
<STR>[^\r\n\\\"]	{ yymore(); }
<STR>\\	{ yymore(); BEGIN STRESC; }
<STR>\"	{ yylval.strv = strdup(yytext); yylval.strv[strlen(yytext) - 1] = 0; length += yyleng; return CONST_STRING; BEGIN INITIAL; }
<STR>\n|\r	{ yyless(0); lex_err( yylineno,"Illegal Character:\\n|\\r"); BEGIN INITIAL; }
<STR><<EOF>>	{ eof_error = 1; lex_err( yylineno,"Illegal Character:\\0"); BEGIN INITIAL; return 0; }
<STRESC>\n|\r { yymore(); yyleng-=2;yytext[yyleng]=0;BEGIN STR; }*/
    /* 不真的转义 */
    /*<STRESC>.	{ yymore(); BEGIN STR; }
<STRESC><<EOF>>	{ eof_error = 1; lex_err( yylineno,"Illegal Character:\\0"); BEGIN INITIAL; return 0; }*/
    /* 关键字 界符和操作符 */

        /*<INITIAL>char        { length += yyleng; return CHAR; }
<INITIAL>void        { length += yyleng; return VOID; }*/

    /* ignore 转义char */
    /*<INITIAL>'.'	{ yylval.chrv = yytext[1]; length += yyleng; return CONST_CHAR; }*/

//    %x STR
//%x STRESC
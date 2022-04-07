%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "lex.yy.c"

extern int yylex();
extern int yyparse();
extern FILE* yyin, *yyout;

union NodeValue
{
    int intv;
    double fltv;
    char* strv; 
};

union NodeValue EmptyNodeValue;

struct Node
{
    char* type;
    union NodeValue value;
    int children_count;
    struct Node** children;
    int line_num;

};

struct Node empty_node_origin={
	type:"EMPTY",
	value:{intv:0},
	children_count:0,
	children:NULL,
	line_num:-1
};

struct Node* root=NULL;
struct Node* new_node(char* type,int ln, int children_count,struct Node* children[]);
struct Node* new_vnode(char* type, int ln,union NodeValue value);
struct Node* new_fnode(char* type, int ln);
struct Node* empty_node=&empty_node_origin;
struct Node* create_node(char* type,union NodeValue value, int children_count,struct Node* children[],int ln);
union NodeValue uv_int(int value);
union NodeValue uv_flt(float value);
union NodeValue uv_str(char* value);
int yylex();
int max_line_num = 0;
char* error_expression;
%}

%locations

%union {
    struct Node* node;
    long intv;
    double fltv;
    char* strv;
    /*char chrv;*/
}

    /* tokens */
%token INT
%token FLOAT
    /*%token CHAR
%token VOID*/
%token WHILE
%token IF
%token ELSE
%token RETURN
%token STRUCT

%token <strv> ID
%token <intv> CONST_INT
%token <fltv> CONST_FLOAT
    /*%token <chrv> CONST_CHAR
%token <strv> CONST_STRING*/

%token COMMA SEM
	/* 运算符优先级越向下越高 */
    /* = */
%right ASSIGNOP
    /* || */
%left OR
	/* && */
%left AND
    /* > >= < <= == != */
%left GT GE LT LE EQ NEQ
    /* + - */
%left ADD SUB
    /* * / */
%left MUL DIV
    /* !*/
%right NOT
    /* ( ) [ ] .*/
%left LP RP LB RB DOT
    /* { } */
%left LC RC











/*%type <intv> IntExpr
%type ElseExpr
%type <fltv> FloatExpr*/
%type<node> RELOP TYPE
%type<node> Program ExtDefList ExtDef ExtDecList Specifier VarDec FunDec CompSt
%type<node> StructSpecifier OptTag DefList Tag
%type<node> VarList ParamDec StmtList Stmt Exp
%type<node> Def Dec DecList Args
%start Program

%%

    /* Tokens */
RELOP: GT	{
		$$=new_fnode("GT",@$.first_line);
	}
    |LT	{
		$$=new_fnode("LT",@$.first_line);
	}
    |GE	{
		$$=new_fnode("GE",@$.first_line);
	}
    |LE	{
		$$=new_fnode("LE",@$.first_line);
	}
    |EQ	{
		$$=new_fnode("EQ",@$.first_line);
	}
    |NEQ	{
		$$=new_fnode("NEQ",@$.first_line);
	}
    ;
TYPE: INT	{
		$$=new_fnode("int",@$.first_line);
	}
    | FLOAT	{
		$$=new_fnode("float",@$.first_line);
	}
    ;
    /* High-Level Definitions */
Program: ExtDefList	{
		struct Node* nodes[]={
			$1
			};
		$$=new_node("Program",@$.first_line,1,nodes);
		root=$$;
		max_line_num=@$.first_line;
	}
    ;

ExtDefList: ExtDef ExtDefList	{
		struct Node* nodes[]={
			$1,
			$2
			};
		$$=new_node("ExtDefList",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    | /* empty string */	{
		$$ = empty_node;
	}
    ;

ExtDef: Specifier ExtDecList SEM	{
		struct Node* nodes[]={
			$1,
			$2,
			new_fnode("SEM",@3.first_line),
			};
		$$=new_node("ExtDef",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Specifier SEM	{
		struct Node* nodes[]={
			$1,
			new_fnode("SEM",@2.first_line)
			};
		$$=new_node("ExtDef",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    | Specifier FunDec CompSt	{
		struct Node* nodes[]={
			$1,
			$2,
			$3
			};
		$$=new_node("ExtDef",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    ;

ExtDecList: VarDec	{
		struct Node* nodes[]={$1};
		$$=new_node("ExtDecList",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | VarDec COMMA ExtDecList	{
		struct Node* nodes[]={$1,new_fnode("COMMA",@2.first_line),	$3};
		$$=new_node("ExtDecList",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    ;
    /* Specifiers */

Specifier: TYPE	{
			/* TYPE被传递 */
		union NodeValue nv;
		nv.strv=strdup($1->type);
		struct Node* nodes[]={
			new_vnode("TYPE",@1.first_line,nv)
			};
		$$=new_node("Specifier",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | StructSpecifier	{
		struct Node* nodes[]={
			$1
			};
		$$=new_node("Specifier",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    ;

StructSpecifier: STRUCT OptTag LC DefList RC	{
		struct Node* nodes[]={
				new_fnode("STRUCT",@1.first_line),
				$2,
				new_fnode("LC",@2.first_line),
				$4,	
				new_fnode("RC",@4.first_line)
			};
		$$=new_node("StructSpecifier",@$.first_line,5,nodes);
		max_line_num=@$.first_line;
	}
    | STRUCT Tag	{
		struct Node* nodes[]={
			new_fnode("STRUCT",@1.first_line),
			$2
		};
		$$=new_node("StructSpecifier",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    ;

OptTag: ID	{
		struct Node* nodes[]={
			new_vnode("ID",@1.first_line,uv_str($1))			
		};
		$$=new_node("OptTag",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | /* empty string */	{
		$$ = empty_node;
	}
    ;

Tag: ID	{
		struct Node* nodes[]={
			new_vnode("ID",@1.first_line,uv_str($1))
		};
		$$=new_node("Tag",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    ;

    /* Declarators */
VarDec: ID	{
		struct Node* nodes[]={
			new_vnode("ID",@1.first_line,uv_str($1))
		};
		$$=new_node("VarDec",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    |VarDec LB CONST_INT RB	{
		struct Node* nodes[]={
			$1,
			new_fnode("LB",@2.first_line),
			new_vnode("CONST_INT",@3.first_line,uv_int($3)),
			new_fnode("RB",@4.first_line)
			};
		$$=new_node("VarDec",@$.first_line,4,nodes);
		max_line_num=@$.first_line;
	}
    ;

FunDec: ID LP VarList RP	{
		struct Node* nodes[]={			
			new_vnode("ID",@1.first_line,uv_str($1)),			
			new_fnode("LP",@2.first_line),
			$3,
			new_fnode("RP",@4.first_line)
		};
		$$=new_node("FunDec",@$.first_line,4,nodes);
		max_line_num=@$.first_line;
	}
    | ID LP RP	{
		struct Node* nodes[]={	
			new_vnode("ID",@1.first_line,uv_str($1)),
			new_fnode("LP",@2.first_line),
			new_fnode("RP",@3.first_line)
		};
		$$=new_node("FunDec",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    ;

VarList: ParamDec COMMA VarList	{
		struct Node* nodes[]={
			$1,
			new_fnode("COMMA",@2.first_line),
			$3
			};
		$$=new_node("VarList",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | ParamDec	{
		struct Node* nodes[]={
			$1
			};
		$$=new_node("VarList",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    ;

ParamDec: Specifier VarDec	{
		struct Node* nodes[]={
			$1,
			$2
			};
		$$=new_node("ParamDec",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    ;
    /* Statements */

CompSt: LC DefList StmtList RC	{
		struct Node* nodes[]={
			new_fnode("LC",@1.first_line),
			$2,
			$3,
			new_fnode("RC",@4.first_line)
			};
		$$=new_node("CompSt",@$.first_line,4,nodes);
		max_line_num=@$.first_line;
	}
    |   LC error RC { $$ = NULL; }
    ;

StmtList: Stmt StmtList	{
		struct Node* nodes[]={
			$1,
			$2,
			};
		$$=new_node("StmtList",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    | /* empty string */	{
		$$ = empty_node;
	}
    ;
Stmt: Exp SEM	{
		struct Node* nodes[]={
			$1,
			new_fnode("SEM",@2.first_line)
			};
		$$=new_node("Stmt",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    | CompSt	{
		struct Node* nodes[]={
			$1,
			};
		$$=new_node("Stmt",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | RETURN Exp SEM	{
		struct Node* nodes[]={
			new_fnode("RETURN",@1.first_line),
			$2,
			new_fnode("SEM",@3.first_line)
			};
		$$=new_node("Stmt",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | IF LP Exp RP Stmt	{
		struct Node* nodes[]={
			new_fnode("IF",@1.first_line),
			new_fnode("LP",@2.first_line),
			$3,
			new_fnode("RP",@4.first_line),
			$5
			};
		$$=new_node("Stmt",@$.first_line,5,nodes);
		max_line_num=@$.first_line;
	}
    | IF LP Exp RP Stmt ELSE Stmt	{
		struct Node* nodes[]={
			new_fnode("IF",@1.first_line),
			new_fnode("LP",@2.first_line),
			$3,
			new_fnode("RP",@4.first_line),
			$5,
			new_fnode("ELSE",@4.first_line),
			$7
			};
		$$=new_node("Stmt",@$.first_line,7,nodes);
		max_line_num=@$.first_line;
	}
    | WHILE LP Exp RP Stmt	{
		struct Node* nodes[]={
			new_fnode("WHILE",@1.first_line),
			new_fnode("LP",@2.first_line),
			$3,
			new_fnode("RP",@4.first_line),
			$5
			};
		$$=new_node("Stmt",@$.first_line,5,nodes);
		max_line_num=@$.first_line;
	}
    | error SEM { $$ = NULL; }
    ;

    /* Local Definitions */
DefList: Def DefList	{
		struct Node* nodes[]={
			$1,
			$2,
			};
		$$=new_node("DefList",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    | /* empty string */	{
		$$ = empty_node;
	}
    ;

Def: Specifier DecList SEM	{
		struct Node* nodes[]={
			$1,
			$2,
			new_fnode("SEM",@3.first_line),
			};
		$$=new_node("Def",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    ;

DecList: Dec	{
		struct Node* nodes[]={
			$1
			};
		$$=new_node("DecList",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | Dec COMMA DecList	{
		struct Node* nodes[]={
			$1,
			new_fnode("COMMA",@2.first_line),
			$3
			};
		$$=new_node("DecList",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    ;

Dec: VarDec	{
		struct Node* nodes[]={
			$1,
			};
		$$=new_node("Dec",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | VarDec ASSIGNOP Exp	{
		struct Node* nodes[]={
			$1,
			new_fnode("ASSIGNOP",@2.first_line),
			$3
			};
		$$=new_node("Dec",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    ;


    /* Expressions */

Exp: Exp ASSIGNOP Exp	{
		struct Node* nodes[]={$1,	new_fnode("ASSIGNOP",@2.first_line),$3	};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp AND Exp	{
		struct Node* nodes[]={
			$1,
			new_fnode("AND",@2.first_line),
			$3
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp OR Exp	{
		struct Node* nodes[]={
			$1,
			new_fnode("OR",@2.first_line),
			$3
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp RELOP Exp	{
			/* RELOP被传递 */
		struct Node* nodes[]={
			$1,
			new_fnode($2->type,@2.first_line),
			$3
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp ADD Exp	{
		struct Node* nodes[]={
			$1,
			new_fnode("ADD",@2.first_line),
			$3
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp SUB Exp	{
		struct Node* nodes[]={
			$1,
			new_fnode("SUB",@2.first_line),
			$3
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp MUL Exp	{
		struct Node* nodes[]={
			$1,
			new_fnode("MUL",@2.first_line),
			$3
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp DIV Exp	{
		struct Node* nodes[]={
			$1,
			new_fnode("DIV",@2.first_line),
			$3
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | LP Exp RP	{
		struct Node* nodes[]={
			new_fnode("LP",@1.first_line),
			$2,
			new_fnode("RP",@3.first_line)
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | SUB Exp	{
		/* 这里是负号NEG */
		struct Node* nodes[]={
			new_fnode("NEG",@1.first_line),
			$2
			};
		$$=new_node("Exp",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    | NOT Exp	{
		struct Node* nodes[]={
			new_fnode("NOT",@1.first_line),
			$2
			};
		$$=new_node("Exp",@$.first_line,2,nodes);
		max_line_num=@$.first_line;
	}
    | ID LP Args RP	{
		struct Node* nodes[]={
			new_vnode("ID",@1.first_line,uv_str($1)),
			new_fnode("LP",@2.first_line),
			$3,
			new_fnode("RP",@4.first_line),
			};
		$$=new_node("Exp",@$.first_line,4,nodes);
		max_line_num=@$.first_line;
	}
    | ID LP RP	{
		struct Node* nodes[]={
			new_vnode("ID",@1.first_line,uv_str($1)),
			new_fnode("LP",@2.first_line),
			new_fnode("RP",@3.first_line),
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | Exp LB Exp RB	{
		struct Node* nodes[]={
			$1,
			new_fnode("LB",@2.first_line),
			$3,
			new_fnode("RB",@4.first_line),
			};
		$$=new_node("Exp",@$.first_line,4,nodes);
		max_line_num=@$.first_line;
	}
    | Exp DOT ID	{
		struct Node* nodes[]={
			$1,
			new_fnode("DOT",@2.first_line),
			new_vnode("ID",@3.first_line,uv_str($3))
			};
		$$=new_node("Exp",@$.first_line,3,nodes);
		max_line_num=@$.first_line;
	}
    | ID	{
		struct Node* nodes[]={
			new_vnode("ID",@1.first_line,uv_str($1))
			};
		$$=new_node("Exp",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | CONST_INT	{
		struct Node* nodes[]={
			new_vnode("CONST_INT",@1.first_line,uv_int($1))
			};
		$$=new_node("Exp",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    | CONST_FLOAT	{
		struct Node* nodes[]={
			new_vnode("CONST_FLOAT",@1.first_line,uv_flt($1))
			};
		$$=new_node("Exp",@$.first_line,1,nodes);
		max_line_num=@$.first_line;
	}
    ;

Args: Exp COMMA Args
    | Exp
    ;

 

%%

struct Node* create_node(char* type,union NodeValue value, int children_count,struct Node* children[],int ln){
    struct Node* r= (struct Node*)malloc(sizeof(struct Node));
    r->type=strdup(type);
    if(children_count>0){
        r->children=(struct Node**)malloc(children_count * sizeof(struct Node*));
		for(int i=0;i<children_count;i++){
			r->children[i]=children[i];		
		}
    }
	else
	{
        r->children=NULL;
    }
    r->value=value;
    r->children_count=children_count;
    r->line_num=ln;
	if (YYDEBUG){
		printf("NodeCreated[%x]: {Type=%s,children_count=%d,ln=%d}\n",r,r->type,r->children_count,r->line_num);
	}


    return r;
}

union NodeValue uv_int(int value){
	union NodeValue uv;
	uv.intv=value;
	return uv;
}
union NodeValue uv_flt(float value){
	union NodeValue uv;
	uv.fltv=value;
	return uv;
}
union NodeValue uv_str(char* value){
	union NodeValue uv;
	uv.strv=value;
	return uv;
}

struct Node* new_node(char* type,int ln, int children_count,struct Node* children[]){
	return create_node(type,EmptyNodeValue,children_count,children,ln);
}
struct Node* new_vnode(char* type, int ln,union NodeValue value){
	return create_node(type,value,0,NULL,ln);
}
struct Node* new_fnode(char* type, int ln){
		return create_node(type,EmptyNodeValue,0,NULL,ln);
}

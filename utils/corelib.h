#pragma once
#include<memory.h>
#include "../syntax.tab.h"
#include "../lib/hashtable/hashtable.h"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define CLEAN "\033[0m"

#define new(T) calloc(sizeof(T),1);
#define cast(T,origin) (T)(origin)
#define is ==
#define IS_TRUE(f) (f)!=0
#define IS_FALSE(f) (f)==0
#define null NULL
#define ASSERT_NOT_NULL(o) if (o is null) return null
#define TODO throw("Not Implemented","TODO");
#define NotSupportedException "Such operation is not supported in current Status"
typedef struct control_flags_t
{
	int implemented : 1;
	int readonly : 1;
	/**
	 * @brief 是否为内建符号(read=1和write=2函数)
	 *
	 */
	int builtin : 2;
	/**
	 * @brief 中间代码生成中，表达式是否可以被内联
	 * 0: 不可内联
	 * 1：可内联
	 * (ref指向的操作数为常量，或上一操作是赋值，因此可并入赋值表达式)
	 *
	 */
	int ir_inline : 1;
	int padding : 26;
} control_flags;

// typedef enum ir_inline_type_t{
// 	NOT_INLINE,
// 	INLINE_ASSIGN,
// 	INLINE_ELSE
// } ir_inline_type;
typedef void* object;
typedef hash_table* symbol_table;

typedef struct node_t
{
	char* type;
	union
	{
		int intv;
		double fltv;
		char* strv;
	} value;
	int children_count;
	struct node_t** children;
	int line_num;
} synnode;
typedef synnode* nodeptr;

int streql(char* s1, char* s2);
extern void trans_err(int type, int ln);
extern void trans_err_x(int type, int ln, char* expr);

//typedef list symbol_table;

typedef char* string;

typedef enum basic_type_t
{
	TYPE_INT,
	TYPE_FLOAT
} basic_type;


typedef enum class_schema_t
{
	CLASS_STANDARD,
	CLASS_FUNCTION,
	CLASS_ARRAY,
	CLASS_STRUCT
} class_schema;
typedef struct array_def_t
{
	struct class_t* type;
	size_t size;
} array_def;
//extern struct list_t;
typedef struct function_def_t
{
	struct class_t* type;
	/**
	 * @brief list<symbol>
	 *
	 */
	struct list_t* params;
} function_def;
typedef union class_def_t
{
	basic_type type;
	array_def arr_def;
	function_def func_def;
	symbol_table stu_def;
} class_def;
/**
 * @brief 符号的类型定义
 *
 */
typedef struct class_t
{
	class_schema schema;
	class_def def;
} *class;
class new_class(
	class_schema schema, class_def type_def);

class class_int();

class clscpy(class origin);
int clseql(class t1, class t2);

class class_float();
control_flags readonly_flag();
int size_of_class(class c);
void throw(string desc, string location);


#include "../syntax.tab.c"
#include "../lib/hashtable/hashtable.h"
#include "list.h"
typedef string string;

typedef enum basic_type_t
{
	TYPE_INT,
	TYPE_FLOAT
} basic_type;

typedef hash_table *symbol_table;
typedef enum symbol_schema_t
{
	SYMBOL_SCHEMA_STANDARD,
	SYMBOL_SCHEMA_FUNCTION,
	SYMBOL_SCHEMA_ARRAY,
	SYMBOL_SCHEMA_STRUCT
} symbol_schema;
typedef struct array_def_t
{
	struct symbol_type_t *type;
	size_t size;
} array_def;
typedef struct function_def_t
{
	struct symbol_type_t *type;
	symbol_table args
} function_def;
typedef union symbol_type_def_t
{
	basic_type type;
	array_def arr_def;
	function_def func_def;
	symbol_table stu_def;
} symbol_type_def;
/**
 * @brief 符号的类型定义
 *
 */
typedef struct symbol_type_t
{
	symbol_schema schema;
	symbol_type_def def;
} * symbol_type;
symbol_type new_symbol_type();

symbol_type type_int();

symbol_type typecpy(symbol_type origin);
int typeeql(symbol_type t1, symbol_type t2);

symbol_type type_float();

typedef struct symbol_flags_t
{
	int implemented : 1;
	int readonly : 1;
	int padding : 30;
} symbol_flags;
typedef struct symbol_t
{
	symbol_type type;
	string name;
	symbol_flags flags;
} * symbol;

symbol new_symbol(symbol_type type, string name, int readonly, int implemented);
symbol_type new_symbol_type(
	symbol_schema schema, symbol_type_def type_def);
symbol_table new_symbol_table();
void destroy_symbol_table(symbol_table table);

#define __WITH_MURMUR

typedef struct symbol_table_enumerator_t
{
	void **keys;
	symbol_table table;
	size_t entry_count;
	size_t current_count;
} * symbol_table_enumerator;

symbol_table_enumerator create_symbol_table_enumerator(symbol_table t);
void destroy_symbol_table_enumerator(symbol_table_enumerator e);
int has_next_symbol_table_enumerator(symbol_table_enumerator e);
symbol get_current_symbol_table_enumerator(symbol_table_enumerator e);
void move_next_symbol_table_enumerator(symbol_table_enumerator e);
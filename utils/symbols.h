#pragma once

#define __WITH_MURMUR
#include "../lib/hashtable/hashtable.h"
#include "list.h"
#include "corelib.h"
#include "statements.h"
#include "expressions.h"

typedef union symbol_ref_t
{
	statement_comp func;
	expression var;
	nodeptr origin;
} symbol_ref;
typedef struct symbol_t
{
	class type;
	string name;
	control_flags flags;
	symbol_ref reference;
} * symbol;
symbol new_symbol(class type, string name, int readonly, int implemented);
class new_class(
	class_schema schema, class_def type_def);
symbol_table new_symbol_table();
symbol_table new_symbol_table_from_list(list l);
void destroy_symbol_table(symbol_table table);
symbol get_symbol(symbol_table slist, string name);
int has_symbol(symbol_table slist, string name);
void add_symbol(symbol_table slist, symbol sym);

typedef struct symbol_table_enumerator_t
{
	void **keys;
	symbol_table table;
	unsigned int entry_count;
	size_t current_count;
} * symbol_table_enumerator;
symbol_table_enumerator create_symbol_table_enumerator(symbol_table t);
void destroy_symbol_table_enumerator(symbol_table_enumerator e);
int has_next_symbol_table_enumerator(symbol_table_enumerator e);
symbol get_current_symbol_table_enumerator(symbol_table_enumerator e);
void move_next_symbol_table_enumerator(symbol_table_enumerator e);
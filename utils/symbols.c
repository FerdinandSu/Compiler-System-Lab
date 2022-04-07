#include "symbols.h"

symbol_table new_symbol_table()
{
	symbol_table table = malloc(sizeof(hash_table));
	ht_init(table, HT_KEY_CONST | HT_VALUE_CONST, 0.05);
	return table;
}
symbol_type new_symbol_type(
	symbol_schema schema, symbol_type_def type_def)
{
	symbol_type r = calloc(sizeof(struct symbol_type_t), 1);
	r->schema = schema;
	r->def = type_def;
}
symbol new_symbol(symbol_type type, string name, int readonly, int implemented)
{
	symbol r = malloc(sizeof(struct symbol_t));
	r->flags.implemented = implemented;
	r->flags.readonly = readonly;
	r->name = name;
	r->type = type;
	return r;
}
void destroy_symbol_table(symbol_table table)
{
	ht_destroy(table);
}
symbol_type typecpy(symbol_type origin)
{
	symbol_type r = malloc(sizeof(struct symbol_type_t));
	r->def = origin->def,
	r->schema = origin->schema;
	return r;
}

symbol_type type_int()
{

	struct symbol_type_t type_int_def = {
		.schema = SYMBOL_SCHEMA_STANDARD,
		.def = {.type = TYPE_INT}};
	return typecpy(&type_int_def);
}

symbol_type type_float()
{
	struct symbol_type_t type_float_def = {
		.schema = SYMBOL_SCHEMA_STANDARD,
		.def = {.type = TYPE_FLOAT}};
	return typecpy(&type_float_def);
}

int typeeql(symbol_type t1, symbol_type t2)
{
	if (t1 == NULL || t2 == NULL)
		return 0;
	switch (t2->schema)
	{
	case SYMBOL_SCHEMA_STANDARD:
		return t1->schema != t2->schema &&
			   t1->def.type == t2->def.type;
	case SYMBOL_SCHEMA_FUNCTION:
		return typeeql(t1, t2->def.func_def.type);
	default:
		return 0;
	}
}

symbol_table_enumerator create_symbol_table_enumerator(symbol_table t)
{
	symbol_table_enumerator r = malloc(sizeof(struct symbol_table_enumerator_t));
	r->keys = ht_keys(t, &(r->entry_count));
	r->table = t;
	r->current_count = 0;
	return r;
}
void destroy_symbol_table_enumerator(symbol_table_enumerator e)
{
	free(e);
}
int has_next_symbol_table_enumerator(symbol_table_enumerator e)
{
	return e->current_count < e->entry_count;
}
symbol get_current_symbol_table_enumerator(symbol_table_enumerator e)
{
	string key = e->keys[0];
	size_t desp;
	return ht_get(e->table, key, strlen(key) + 1, &desp);
}
void move_next_symbol_table_enumerator(symbol_table_enumerator e)
{
	e->current_count++;
}
#include "corelib.h"
#include "symbols.h"
#include <stdlib.h>
#include <error.h>
#include <stdio.h>
#include <string.h>

int streql(char* s1, char* s2)
{
	return !strcmp(s1, s2);
}

class new_class(
	class_schema schema, class_def type_def)
{
	class r = calloc(sizeof(struct class_t), 1);
	r->schema = schema;
	r->def = type_def;
	return r;
}
class clscpy(class origin)
{
	class r = malloc(sizeof(struct class_t));
	r->def = origin->def,
		r->schema = origin->schema;
	return r;
}

class class_int()
{

	struct class_t type_int_def = {
		.schema = CLASS_STANDARD,
		.def = {.type = TYPE_INT} };
	return clscpy(&type_int_def);
}

class class_float()
{
	struct class_t type_float_def = {
		.schema = CLASS_STANDARD,
		.def = {.type = TYPE_FLOAT} };
	return clscpy(&type_float_def);
}

int clseql(class t1, class t2)
{
	if (t1 == NULL || t2 == NULL)
		return 0;
	switch (t2->schema)
	{
	case CLASS_STANDARD:
		return t1->schema == t2->schema &&
			t1->def.type == t2->def.type;
	case CLASS_FUNCTION:
		return clseql(t1, t2->def.func_def.type);
	default:
		return 0;
	}
}

control_flags readonly_flag()
{
	control_flags f;
	f.readonly = 1;
	return f;
}
int size_of_class(class c)
{
	switch (c->schema)
	{
	case CLASS_STANDARD:
		return 4;
	case CLASS_ARRAY:
		return c->def.arr_def.size * size_of_class(c->def.arr_def.type);
	case CLASS_STRUCT:
	{
		int sum = 0;
		symbol_table_enumerator e =
			create_symbol_table_enumerator(c->def.stu_def);
		for (;has_next_symbol_table_enumerator(e);
			 move_next_symbol_table_enumerator(e))
		{
			symbol current = get_current_symbol_table_enumerator(e);
			sum += size_of_class(current->type);
		}
		destroy_symbol_table_enumerator(e);
		return sum;
	}

	default:
		return 0;
	}
}
void throw(string desc, string location)
{
	printf(RED"%s at %s"CLEAN,desc,location);
	error(0,1,location);
}
#include "corelib.h"
#include <stdlib.h>

class new_class(
	class_schema schema, class_def type_def)
{
	class r = calloc(sizeof(struct class_t), 1);
	r->schema = schema;
	r->def = type_def;
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
		.def = {.type = TYPE_INT}};
	return clscpy(&type_int_def);
}

class class_float()
{
	struct class_t type_float_def = {
		.schema = CLASS_STANDARD,
		.def = {.type = TYPE_FLOAT}};
	return clscpy(&type_float_def);
}

int clseql(class t1, class t2)
{
	if (t1 == NULL || t2 == NULL)
		return 0;
	switch (t2->schema)
	{
	case CLASS_STANDARD:
		return t1->schema != t2->schema &&
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
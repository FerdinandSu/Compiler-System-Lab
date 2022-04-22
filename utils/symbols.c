#include "symbols.h"
#include "stdlib.h"
#include "../lib/hashtable/hashtable.h"
#include "../lib/hashtable/murmur.h"

symbol_table new_symbol_table()
{
	symbol_table table = malloc(sizeof(hash_table));
	ht_init(table, HT_KEY_CONST | HT_VALUE_CONST, 0.05, MurmurHash3_x86_32, MurmurHash3_x86_128, MurmurHash3_x64_128);
	return table;
}

symbol new_symbol(class type, string name, int readonly, int implemented, int index)
{
	symbol r = malloc(sizeof(struct symbol_t));
	r->flags.implemented = implemented;
	r->flags.readonly = readonly;
	r->index = index;
	r->name = name;
	r->type = type;
	return r;
}
symbol get_symbol(symbol_table slist, string name)
{
	size_t v_size;
	return ht_get(slist, name, strlen(name) + 1, &v_size);
}
int has_symbol(symbol_table slist, string name)
{
	return ht_contains(slist, name, strlen(name) + 1);
}
void add_symbol(symbol_table slist, symbol sym)
{
	return ht_insert(slist, sym->name, strlen(sym->name) + 1,
					 sym, sizeof(struct symbol_t));
}
void destroy_symbol_table(symbol_table table)
{
	ht_destroy(table);
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
	string key = e->keys[e->current_count];
	size_t desp;
	return ht_get(e->table, key, strlen(key) + 1, &desp);
}
void move_next_symbol_table_enumerator(symbol_table_enumerator e)
{
	e->current_count++;
}

symbol_table new_symbol_table_from_list(list l)
{
	list_enumerator e = create_enumerator(l);
	symbol_table st = new_symbol_table();
	for (; has_next_enumerator(e); move_next_enumerator(e))
	{
		symbol sym = get_current_enumerator(e);
		add_symbol(st, sym);
	}
	destroy_enumerator(e);
	return st;
}

list symbol_table_to_list(symbol_table t)
{
	int buf_siz = t->key_count;
	list l = new_list();
	if (buf_siz == 0)return l;
	symbol* const buf = calloc(sizeof(symbol), buf_siz);
	symbol* buf_ptr = buf;
	symbol_table_enumerator e;
	for (e = create_symbol_table_enumerator(t);
		 has_next_symbol_table_enumerator(e);
		 move_next_symbol_table_enumerator(e))
	{
		symbol current = get_current_symbol_table_enumerator(e);
		*buf_ptr=current;
		buf_ptr++;
	}
	destroy_symbol_table_enumerator(e);
	if (buf_siz > 1)
	{
		symbol* hi = buf_ptr - 1;
		symbol* lo = buf;
		symbol* p;
		symbol* max;
		while (hi > lo)
		{
			max = lo;

			/*下面这个for循环作用是从lo到hi的元素中，选出最大的一个，max指针指向这个最大项*/

			for (p = lo + 1; p <= hi; p++)
			{
				if ((**p).index > (**max).index)
				{
					max = p;

				}

			}
			symbol swp = *max;
			*max = *hi;
			*hi = swp;
			hi--;
		}

	}
	for (int i = 1;i < buf_siz;i++)
		add_list(l, buf[i]);
	free(buf);
	return l;
}
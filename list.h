#pragma once
#include <stdlib.h>
#include "corelib.h"

typedef struct list_node_t
{
    object element;
    struct list_node_t *next;
    struct list_node_t *last;
} * list_node;

typedef struct list_t
{
    list_node first;
    list_node last;
    size_t count;
} * list;

typedef struct list_enumerator_t
{
    list_node current;
} * list_enumerator;

list new_list();
list new_list_singleton(object e);
void add_list(list l, object e);
void destroy_list(list l);
list_enumerator create_enumerator(list l);
void destroy_enumerator(list_enumerator e);
int has_next_enumerator(list_enumerator e);
object get_current_enumerator(list_enumerator e);
void move_next_enumerator(list_enumerator e);
list new_list_of(int count, ...);

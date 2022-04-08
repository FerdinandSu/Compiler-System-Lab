
#include <stdlib.h>
#include <stdarg.h>
#include "list.h"

list new_list()
{
    return calloc(sizeof(struct list_t), 1);
}
list_node new_list_node(object element)
{
    list_node node = calloc(sizeof(struct list_node_t), 1);
    node->element = element;
    return node;
}
void add_list(list l, object e)
{
    list_node new = new_list_node(e);
    if (l->count == 0)
    {
        l->first = new;
    }
    else
    {
        new->last = l->last;
        l->last->next = new;
    }
    l->last = new;
    l->count++;
}
list new_list_singleton(object e)
{
    list l = new_list();
    add_list(l, e);
    return l;
}
void destroy_list(list l)
{
    for (list_node n = l->first; n != NULL;)
    {
        list_node nn = n->next;
        free(n);
        n = nn;
    }
    free(l);
}
list_enumerator create_enumerator(list l)
{
    list_enumerator e = calloc(sizeof(struct list_enumerator_t), 1);
    e->current = l->first;
    return e;
}
void destroy_enumerator(list_enumerator e)
{
    free(e);
}
int has_next_enumerator(list_enumerator e)
{
    return e->current != NULL;
}
object get_current_enumerator(list_enumerator e)
{
    return e->current->element;
}
void move_next_enumerator(list_enumerator e)
{
    e->current = e->current->next;
}
list new_list_of(int count, ...)
{
    list l = new_list();
    va_list vl;
    va_start(vl, count);
    for (int i = 0; i < count; i++)
    {
        add_list(l,va_arg(vl, object));
    }
    va_end(vl);

    return l;

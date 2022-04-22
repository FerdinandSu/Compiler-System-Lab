
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

list prepend_list(object e, list l)
{
    list_node new = new_list_node(e);
    if (l->count == 0)
    {
        l->last = new;
    }
    else
    {
        new->next = l->first;
        l->first->last = new;
    }
    l->first = new;
    l->count++;
    return l;
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

list_enumerator create_reverse_enumerator(list l)
{
    list_enumerator e = new(struct list_enumerator_t);
    e->current = l->last;
    return e;
}

void move_next_reverse_enumerator(list_enumerator e)
{
    e->current = e->current->last;
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
        add_list(l, va_arg(vl, object));
    }
    va_end(vl);

    return l;
}
list concat_list(list l1, list l2)
{
    if (l1->count == 0)
    {
        destroy_list(l1);
        return l2;
    }
    if (l2->count == 0)
    {
        destroy_list(l2);
        return l1;
    }
    l1->last->next = l2->first;
    l2->first->last = l1->last;
    l1->last = l2->last;
    l1->count += l2->count;
    free(l2);
    return l1;
}

list append_list(list l, object e)
{
    add_list(l, e);
    return l;
}
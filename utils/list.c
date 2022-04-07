
#include <stdlib.h>
#include "list.h"

list *new_list()
{
    return calloc(sizeof(list), 1);
}
list_node *new_list_node(void *element)
{
    list_node *node = calloc(sizeof(list_node), 1);
    node->element = element;
    return node;
}
void add_list(list *l, void *e)
{
    list_node *new = new_list_node(e);
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
void destroy_list(list *l)
{
    for (list_node *n = l->first; n != NULL;)
    {
        list_node *nn = n->next;
        free(n);
        n = nn;
    }
    free(l);
}
list_enumerator *create_enumerator(list *l)
{
    list_enumerator *e = calloc(sizeof(list_enumerator), 1);
    e->current = l->first;
    return e;
}
void destroy_enumerator(list_enumerator *e)
{
    free(e);
}
int has_next_enumerator(list_enumerator *e)
{
    return e->current != NULL;
}
void *get_current_enumerator(list_enumerator *e)
{
    return e->current->element;
}
void move_next_enumerator(list_enumerator *e)
{
    e->current = e->current->next;
}

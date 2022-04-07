#include "stack.h"
#include <memory.h>

stack *new_stack()
{
    return calloc(sizeof(stack), 1);
}
stack_node *new_stack_node(void *element)
{
    stack_node *node = calloc(sizeof(stack_node), 1);
    node->element = element;
    return node;
}
void push_stack(stack *stk, void *element)
{
    stack_node *new_node = new_stack_node(element);
    new_node->next = stk->top;
    stk->top = new_node;
}
int is_empty_stack(stack *stk)
{
    return stk->top == NULL;
}
void *peek_stack(stack *stk)
{
    return is_empty_stack(stk) ? NULL : stk->top->element;
}
void *pop_stack(stack *stk)
{
    void *top = peek_stack(stk);
    if (!is_empty_stack(stk))
    {
        stk->top = stk->top->element;
    }
    return top;
}
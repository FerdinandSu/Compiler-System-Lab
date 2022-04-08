#include "stack.h"
#include <memory.h>

stack new_stack()
{
    return calloc(sizeof(struct stack_t), 1);
}
stack_node new_stack_node(object e)
{
    stack_node node = calloc(sizeof(struct stack_node_t), 1);
    node->element = e;
    return node;
}
void push_stack(stack stk, object e)
{
    stack_node new_node = new_stack_node(e);
    new_node->next = stk->top;
    stk->top = new_node;
}
int is_empty_stack(stack stk)
{
    return stk->top == NULL;
}
object peek_stack(stack stk)
{
    return is_empty_stack(stk) ? NULL : stk->top->element;
}
object pop_stack(stack stk)
{
    object top = peek_stack(stk);
    if (!is_empty_stack(stk))
    {
        stk->top = stk->top->element;
    }
    return top;
}
void destroy_stack(stack stk)
{
    // 程序结束了，内存就释放了，所以不会内存泄漏啊（确信）
}

stack_enumerator create_stack_enumerator(stack s){
    stack_enumerator e=new(struct stack_enumerator_t);
    e->current=s->top;
    return e;
}
void destroy_stack_enumerator(stack_enumerator e){
    free(e);
}
int has_next_stack_enumerator(stack_enumerator e){
    return e->current !=NULL;
}
object get_current_stack_enumerator(stack_enumerator e){
    return e->current->element;
}
void move_next_stack_enumerator(stack_enumerator e){
    e->current=e->current->next;
}
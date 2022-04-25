#pragma once
#include<stdlib.h>
#include "corelib.h"

typedef struct stack_node_t
{
    object element;
    struct stack_node_t* next;
} *stack_node;

typedef struct stack_t
{
    stack_node top;
} *stack;

stack new_stack();
void destroy_stack(stack stk);
void push_stack(stack stk,object element);
int is_empty_stack(stack stk);
object peek_stack(stack stk);
object pop_stack(stack stk);

typedef struct stack_enumerator_t
{
    stack_node current;
} * stack_enumerator;

stack_enumerator create_stack_enumerator(stack s);
void destroy_stack_enumerator(stack_enumerator e);
int has_next_stack_enumerator(stack_enumerator e);
object get_current_stack_enumerator(stack_enumerator e);
void move_next_stack_enumerator(stack_enumerator e);

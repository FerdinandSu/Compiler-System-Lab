#pragma once
#include<stdlib.h>

typedef struct stack_node_t
{
    void* element;
    struct stack_node_t* next;
} stack_node;

typedef struct stack_t
{
    stack_node* top;
} stack;

stack* new_stack();
void push_stack(stack* stk,void* element);
int is_empty_stack(stack* stk);
void* peek_stack(stack* stk);
void* pop_stack(stack* stk);
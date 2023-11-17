#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../include/stack.h"

Stack StackCtor(const size_t capacity)
{
    ASSERT(capacity, return {});

    Stack stack = {};

    stack.capacity = capacity;

    stack.data = (data_t *)calloc(capacity * sizeof(data_t), sizeof(char));

    ASSERT(stack.data, return {});

    return stack;
}

int StackDtor(Stack *stack)
{
    STACK_VERIFICATOR(stack, EXIT_FAILURE);

    stack->size     = ULLONG_MAX;
    stack->capacity = 0;

    free(stack->data);
    stack->data = NULL;

    return EXIT_SUCCESS;
}


static int StackExpansion(Stack *stack)
{
    if(stack->size == stack->capacity)
    {
        data_t *data_r = (data_t *)realloc(stack->data, sizeof(data_t) * stack->capacity * 2);
        ASSERT(data_r, return EXIT_FAILURE);

        stack->capacity *= 2;

        stack->data = data_r;

        EXEC_ASSERT(memset(stack->data + stack->size, 0, sizeof(data_t) * stack->size) == EXIT_SUCCESS, return EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

int PushStack(Stack *stack, const data_t val)
{
    STACK_VERIFICATOR(stack, EXIT_FAILURE);

    EXEC_ASSERT(StackExpansion(stack) == EXIT_SUCCESS, return EXIT_FAILURE);

    stack->data[stack->size++] = val;

    return EXIT_SUCCESS;
}


static int StackShrink(Stack *stack)
{
    if(stack->size * 4 == stack->capacity)
    {
        data_t *data_r = (data_t *)realloc(stack->data, sizeof(data_t) * stack->capacity / 2);
        ASSERT(data_r, return EXIT_FAILURE);

        stack->capacity /= 2;

        stack->data = data_r;
    }

    return EXIT_SUCCESS;
}

int PopStack(Stack *stack, data_t *ret_val)
{
    STACK_VERIFICATOR(stack, EXIT_FAILURE);

    ASSERT(stack->size != 0, return EXIT_FAILURE);

    stack->size--;

    if(ret_val) *ret_val = stack->data[stack->size];
    stack->data[stack->size] = 0;

    EXEC_ASSERT(StackShrink(stack) == EXIT_SUCCESS, return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

int ClearStack(Stack *stack)
{
    STACK_VERIFICATOR(stack, EXIT_FAILURE);

    EXEC_ASSERT(memset(stack->data, 0, sizeof(data_t) * stack->size) == EXIT_SUCCESS, return EXIT_FAILURE);

    stack->size = 0;

    EXEC_ASSERT(StackShrink(stack) == EXIT_SUCCESS, return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

void StackDump(Stack *stack)
{
    ASSERT(stack, return);

    LOG("Stack[%p]:       \n"
        "\tsize     = %zu;\n"
        "\tcapacity = %zu;\n"
        "\tdata[%p]:      \n", stack, stack->size, stack->capacity, stack->data);

    ASSERT(stack->data, return);

    for(size_t i = 0; i < stack->capacity; i++) {
        LOG("\t\t");
        LOG((i < stack->size) ? "*" : " ");
        LOG("[%3zu] = " DATA_FORMAT "\n", i, stack->data[i]);
    }
}

#ifdef PROTECT
bool IsStackValid(Stack *stack)
{
    ASSERT(stack      , return false);
    ASSERT(stack->data, return false);

    ASSERT(stack->capacity <= UINT_MAX   , return false);
    ASSERT(stack->capacity != 0          , return false);
    ASSERT(stack->size <= stack->capacity, return false);

    return true;
}
#endif
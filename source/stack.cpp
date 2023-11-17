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
    STACK_VER(stack, EXIT_FAILURE);

    stack->size     = ULLONG_MAX;
    stack->capacity = 0;

    free(stack->data);
    stack->data = NULL;

    return EXIT_SUCCESS;
}


static int OptimalExpansion(Stack *stack)
{
    if(stack->size == stack->capacity)
    {
        data_t *temp_ptr = (data_t *)realloc(stack->data, sizeof(data_t) * stack->capacity * 2);
        ASSERT(temp_ptr, return EXIT_FAILURE);

        stack->capacity *= 2;

        stack->data = temp_ptr;

        EXEC_ASSERT(memset(stack->data + stack->size, 0, sizeof(data_t) * stack->size), return EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

int PushStack(Stack *stack, const data_t val)
{
    STACK_VER(stack, EXIT_FAILURE);

    EXEC_ASSERT(!OptimalExpansion(stack), return EXIT_FAILURE);

    stack->data[stack->size++] = val;

    return EXIT_SUCCESS;
}


static int OptimalShrink(Stack *stack)
{
    if(stack->size * 4 == stack->capacity)
    {
        data_t *temp_ptr = (data_t *)realloc(stack->data, sizeof(data_t) * stack->capacity / 2);
        ASSERT(temp_ptr, return EXIT_FAILURE);

        stack->capacity /= 2;

        stack->data = temp_ptr;
    }

    return EXIT_SUCCESS;
}

int PopStack(Stack *stack, data_t *ret_val)
{
    STACK_VER(stack, EXIT_FAILURE);

    ASSERT(stack->size != 0, return EXIT_FAILURE);

    stack->size--;

    if(ret_val) *ret_val = stack->data[stack->size];
    stack->data[stack->size] = 0;

    EXEC_ASSERT(!OptimalShrink(stack), return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

int ClearStack(Stack *stack)
{
    STACK_VER(stack, EXIT_FAILURE);

    EXEC_ASSERT(memset(stack->data, 0, sizeof(data_t) * stack->size), return EXIT_FAILURE);
    stack->size = 0;

    EXEC_ASSERT(!OptimalShrink(stack), return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

void StackDump(Stack *stack)
{
    ASSERT(stack, return);

    LOG("Stack[%p]:         \n"
        "\tsize     = %zu;  \n"
        "\tcapacity = %zu;  \n"
        "\tdata[%p]:        \n", stack, stack->size, stack->capacity, stack->data);

    ASSERT(stack->data, return);

    for(size_t i = 0; i < stack->capacity; i++)
    {
        LOG("\t\t");
        LOG((i < stack->size) ? "*" : " ");
        LOG("[%3zu] = " DTS "\n", i, stack->data[i]);
    }
}

#ifdef PROTECT
int StackVer(Stack *stack)
{
    ASSERT(stack      , return EXIT_FAILURE);
    ASSERT(stack->data, return EXIT_FAILURE);

    ASSERT(0 != stack->capacity && stack->capacity <= UINT_MAX, return EXIT_FAILURE);
    ASSERT(stack->size <= stack->capacity                     , return EXIT_FAILURE);

    return EXIT_SUCCESS;
}
#endif
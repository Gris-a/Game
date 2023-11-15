#ifndef STACK_H
#define STACK_H

#include "log.h"

typedef long long data_t;
#define DTS "%lld"

struct Stack
{
    size_t size;
    size_t capacity;

    data_t *data;
};

#define STACK_DUMP(stack_ptr)   LOG("%s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                StackDump(stack_ptr)

#ifdef PROTECT
#define STACK_VER(stack_ptr, ret_val_on_fail)   if(StackVer(stack_ptr))\
                                                {\
                                                    LOG("%s:%s: Error: invalid stack.\n", __FILE__, __PRETTY_FUNCTION__);\
                                                    STACK_DUMP(stack_ptr);\
                                                    return ret_val_on_fail;\
                                                }
#else
#define STACK_VER(...)
#endif

Stack StackCtor(const size_t capacity = 2);

int StackDtor(Stack *stack);

int PushStack(Stack *stack, const data_t val);

int PopStack(Stack *stack, data_t *ret_val = NULL);

int ClearStack(Stack *stack);

void StackDump(Stack *stack);

#ifdef PROTECT
int StackVer(Stack *stack);
#endif

#endif //STACK_H

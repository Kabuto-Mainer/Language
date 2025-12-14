#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Stack.h"
#include "SupportFunc.h"


Stack StackCtr (int size)
{
    if (size < 0)
        EXIT_FUNC ("Bad size", NULL);

    StackNumber_t* stack = (StackNumber_t*) calloc (1, sizeof (StackNumber_t));
    if (stack == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    stack->data = (int*) calloc ((size_t) size, sizeof (int));
    if (stack->data == NULL)
    {
        free (stack);
        EXIT_FUNC ("NULL calloc", NULL);
    }

    stack->size = 0;
    stack->capacity = size;

    return stack;
}

void StackPush (Stack stack, int value)
{
    assert (stack);

    if (stack->size >= stack->capacity)
    {
        int* buffer = (int*) realloc (stack->data, size_t (stack->size) * 2 * sizeof (int));
        if (!buffer)
            EXIT_FUNC ("NULL realloc",  );
        stack->data = buffer;
        stack->capacity *= 2;
    }

    stack->data[stack->size++] = value;
    return;
}

int StackPop (Stack stack)
{
    assert (stack);

    if (stack->size == 0)
        EXIT_FUNC ("Bad pop", 0);

    return stack->data[--stack->size];
}

void StackDtr (Stack stack)
{
    assert (stack);

    if (stack->data != NULL)
        free (stack->data);

    free (stack);
    return;
}





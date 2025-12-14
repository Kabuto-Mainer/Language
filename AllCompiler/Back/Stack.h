#ifndef STACK_H
#define STACK_H

struct StackNumber_t
{
    int size;
    int capacity;
    int* data;
};

typedef struct StackNumber_t* Stack;

Stack StackCtr (int size);

void StackPush (Stack stack, int value);

int StackPop (Stack stack);

void StackDtr (Stack stack);


#endif /* STACK_H */

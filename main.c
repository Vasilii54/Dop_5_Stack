#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef double MY_TYPE;

#define POISON (MY_TYPE)3802

struct stack_t {
    // (MY_TYPE)* data;
    double* data;
    int size;
    int capacity;
};

void init_stk(struct stack_t* stk, size_t len, int* err);

void Dump(struct stack_t* stk, int err, int* num, const char** errors);

void Push(struct stack_t* stk, MY_TYPE elem, int* err);

MY_TYPE Pop(struct stack_t* stk, int* err);

void stk_destroy(struct stack_t* stk);

int num = 1;
const char* errors[] {
    "Everything is OK",
    "Not defined right now",
    "You are deleting from empty stack"
};

int main() {

    stack_t stk1 = {};
    int err = 0;
    size_t len = 5;

    init_stk(&stk1, len, &err);
    if(err != 0) Dump(&stk1, err, &num, errors);

    // Dump(&stk1, err, &num, errors);

    Push(&stk1, 11.1, &err);
    if(err != 0) Dump(&stk1, err, &num, errors);

    // Dump(&stk1, err, &num, errors);

    Push(&stk1, 22.2, &err);
    if(err != 0) Dump(&stk1, err, &num, errors);

    // Dump(&stk1, err, &num, errors);

    MY_TYPE last_el = Pop(&stk1, &err);

    // Dump(&stk1, err, &num, errors);

    printf("%f\n", last_el);

    for(int i = 0; i < 12; ++i) {
        Push(&stk1, (MY_TYPE)i, &err);
        if(err != 0) Dump(&stk1, err, &num, errors);
        Dump(&stk1, err, &num, errors);
    }

    MY_TYPE sec_last = Pop(&stk1, &err);

    printf("%f\n", sec_last);

    stk_destroy(&stk1);

    // Dump(&stk1, err, &num, errors);
    return 0;
}

void init_stk(struct stack_t* stk, size_t len, int* err) {
    assert(stk);

    if(stk -> size != 0 || stk -> capacity != 0) *err = 1;

    stk -> data = (MY_TYPE*)calloc(len, sizeof(MY_TYPE));
    stk -> capacity = len;
    stk -> size = 0;

    assert(stk -> data != NULL);
}

void Dump(struct stack_t* stk, int err, int* num, const char** errors) { // чем красивее тем лучше

    printf("\n=== It is Dump number %d. ===\n\nCurrent error number is %d\n%s\n"
    "Capacity: %d\nSize: %d\nCurrent stack:\n\n",
    (*num)++, err, errors[err], stk -> capacity, stk -> size);

    for(int i = 0; i < stk -> capacity; ++i) {
        if(i < stk -> size) {
            printf("* [%d] %f\n", i, stk -> data[i]);
        }
        else {
            printf("[%d] %f\n", i, stk -> data[i]);
        }
    }
    printf("\n=== End of Dump function. ===\n\n");
}

void Push(struct stack_t* stk, MY_TYPE elem, int* err) {

    stk -> data[stk -> size++] = elem;

    if(stk -> size > stk -> capacity - 1) {
        int old = stk -> capacity;
        // stk -> capacity *= 1.5;
        stk -> data = (MY_TYPE*)realloc(stk -> data, (stk -> capacity *= 1.5) * sizeof(MY_TYPE));

        assert(stk -> data != NULL);

        memset(stk -> data + old, 0, (stk -> capacity - old) * sizeof(MY_TYPE));
    }
}

MY_TYPE Pop(struct stack_t* stk, int* err) {
    if(stk -> size == 0) {
        *err = 2;
        Dump(stk, *err, &num, errors);
    }

    stk -> size--;

    MY_TYPE elem = stk -> data[stk -> size];

    stk -> data[stk -> size] = POISON;

    return elem;
}

void stk_destroy(struct stack_t* stk) {
    free(stk -> data);
    stk -> size = 0;
    stk -> capacity = 0;
}

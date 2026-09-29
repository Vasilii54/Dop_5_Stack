#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef double MY_TYPE;

#define POISON (MY_TYPE)3802

struct stack_t {
    MY_TYPE* data;
    int size;
    int capacity;
};

void init_stk(struct stack_t* stk, size_t len, int* err);

void Dump(struct stack_t* stk, int err);

void stk_verify(struct stack_t* stk, int err);

void Push(struct stack_t* stk, MY_TYPE elem, int* err);

MY_TYPE Pop(struct stack_t* stk, int* err);

MY_TYPE Top(struct stack_t* stk, int* err);

void stk_destroy(struct stack_t* stk);

int num = 1;
const char* errors[] { // написать ошибки по уровням: info, debuf warning, error, fatal
                       // только fatal должен вызывать abort()
    "Everything is OK",
    "Not defined right now",
    "You are deleting from empty stack"
};

// TODO: цветной вывод в Dump, VA ARGS в Dump, гистерезис, static переменные - сделать чтобы в дамп передавало причину дампа необязательным параметром

// NDEBUG отменяет все assert
// define STACK_DEBUG
// #ifdef STACK_DEBUG
// ... - ниже действия и объявления в режиме дебага, например, запись в структуру информации о ней (где, как, кем создана)
// #endif вроде надо написать

// #define STACK_DEBUG // РАССКОМЕНТИТЬ ДЛЯ РЕЖИМА ДЕБАГА ИЛИ ИСПОЛЬЗОВАТЬ ФЛАГ
// -D STACK_DEBUG в компиляторе флаг чтообы задефайнить

#ifdef STACK_DEBUG
#define ON_DBG(x) x

#else
#define ON_DBG(x)
#endif

// тогда в режиме дебага заменится на дебажную версию, иначе просто останется пустота


#include "log.c"

int main() {
    log_start();

    stack_t stk1 = {};
    int err = 0;
    size_t len = 5;

    init_stk(&stk1, len, &err);
    stk_verify(&stk1, err);

    // Dump(&stk1, err, &num, errors);

    Push(&stk1, 11.1, &err);
    stk_verify(&stk1, err);

    // Dump(&stk1, err, &num, errors);

    Push(&stk1, 22.2, &err);
    stk_verify(&stk1, err);

    // Dump(&stk1, err, &num, errors);

    MY_TYPE last_el = Pop(&stk1, &err);

    // Dump(&stk1, err, &num, errors);

    printf("%f\n", last_el);

    for(int i = 1; i < 12; ++i) {
        Push(&stk1, (MY_TYPE)i, &err);
        stk_verify(&stk1, err);
        ON_DBG(Dump(&stk1, err);)
    }
    for(int i = 0; i < 10; ++i) {
        MY_TYPE sec_last = Pop(&stk1, &err);

        printf("%f\n", sec_last);
        ON_DBG(Dump(&stk1, err);)
    }

    MY_TYPE almost_last = Top(&stk1, &err);
    printf("%f\n", almost_last);

    stk_destroy(&stk1);

    ON_DBG(Dump(&stk1, err);)

    log_end();
    return 0;
}

void stk_verify(struct stack_t* stk, int err) {
    if(err != 0) Dump(stk, err);
}

void init_stk(struct stack_t* stk, size_t len, int* err) {
    assert(stk);
    assert(err);

    if(stk -> size != 0 || stk -> capacity != 0) *err = 1;

    stk -> data = (MY_TYPE*)calloc(len, sizeof(MY_TYPE));
    stk -> capacity = len;
    stk -> size = 0;

    assert(stk -> data != NULL);
}

void Dump(struct stack_t* stk, int err) { // чем красивее тем лучше

    /**/
    printf("\n\e[0;36m    === It is Dump number %d. ===\n\n\e[0m"
    "\e[0;33m\tError number is %2d\n\n\t%s\n\n"
    "\tCapacity: %8d\n\n"
    "\tSize: %12d\n\n"
    "\tCurrent stack:\n\n\e[0m",
    num++, err, errors[err], stk -> capacity, stk -> size);


    // log_write_print("\n    === It is Dump number %d. ===\n\n")

    if(stk -> data == NULL && stk -> size == 0 && stk -> capacity == 0) {
        printf("\e[0;35m\tIt is an empty stack, after initialising or destroying.\n");
        // log_write_print("\tIt is an empty stack, after initialising or destroying.\n", CIAN_TEXT);
    } else {
        // PRINTING ELEMENTS
        for(int i = 0; i < stk -> capacity; ++i) {
            if(i < stk -> size) {
                printf("\e[0;32m\t* [%d] %f\n\e[0m", i, stk -> data[i]);
            }
            else {
                printf("\e[0;32m\t  [%d] %f\n\e[0m", i, stk -> data[i]);
            }
        }
    }
    printf("\n\e[0;36m    === End of Dump function. ===\n\n\e[0m");
}

void Push(struct stack_t* stk, MY_TYPE elem, int* err) {

    if(stk -> size >= stk -> capacity) {
        int old = stk -> capacity;
        // stk -> capacity *= 1.5;
        stk -> data = (MY_TYPE*)realloc(stk -> data, (stk -> capacity *= 1.5) * sizeof(MY_TYPE));

        assert(stk -> data != NULL);

        memset(stk -> data + old, 0, (stk -> capacity - old) * sizeof(MY_TYPE));
    }

    stk -> data[stk -> size++] = elem;

}

MY_TYPE Pop(struct stack_t* stk, int* err) {
    if(stk -> size == 0) {
        *err = 2;
        Dump(stk, *err);
    }

    stk -> size--;

    MY_TYPE elem = stk -> data[stk -> size];

    stk -> data[stk -> size] = POISON;

    if(stk -> size * 2 < stk -> capacity) {
        stk -> data = (MY_TYPE*)realloc(stk -> data, (stk -> capacity *= 0.5) * sizeof(MY_TYPE));

        assert(stk -> data != NULL);
    }

    return elem;
}

MY_TYPE Top(struct stack_t* stk, int* err) {
    if(stk -> size == 0) {
        *err = 2;
        Dump(stk, *err);
    }

    MY_TYPE elem = stk -> data[stk -> size - 1];

    return elem;
}

void stk_destroy(struct stack_t* stk) {
    free(stk -> data);
    stk -> data = NULL;
    stk -> size = 0;
    stk -> capacity = 0;
}

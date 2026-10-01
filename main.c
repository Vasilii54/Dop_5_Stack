#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

typedef double MY_TYPE;

#define POISON (MY_TYPE)3802
#define MXLN 300

// #define STACK_DEBUG // РАССКОМЕНТИТЬ ДЛЯ РЕЖИМА ДЕБАГА
// ИЛИ ИСПОЛЬЗОВАТЬ ФЛАГ -D STACK_DEBUG

#ifdef STACK_DEBUG
#define ON_DBG(...) __VA_ARGS__
#else
#define ON_DBG(...)
#endif

#define GET_VAR_NAME(var) #var

void init_stk(struct stack_t* stk, size_t len, int* err
              ON_DBG(, const char* name, const char* file, int line));

void Dump(struct stack_t* stk, int err, ...);

void stk_verify(struct stack_t* stk, int err);

void Push(struct stack_t* stk, MY_TYPE elem, int* err);

MY_TYPE Pop(struct stack_t* stk, int* err);

MY_TYPE Top(struct stack_t* stk, int* err);

void stk_destroy(struct stack_t* stk);

const char* errors[] { // написать ошибки по уровням: info, debuf warning, error, fatal
                       // только fatal должен вызывать abort()
    "Everything is OK",
    "Not defined right now",
    "You are deleting from empty stack"
};

// TODO: цветной вывод в Dump, VA ARGS в Dump, гистерезис, static переменные -
// TODO: сделать чтобы в дамп передавало причину дампа необязательным параметром
// TODO: stack verify - разбить на несколько условий и вызывать дампы по разным причинам, писать в дамп причину вызова
// TODO: канареечная защита самой структуры, класть с начала и с конца структуры
// TODO: добавить в дамп информацию о структуре, которая собирается в режиме ON_DBG
// TODO: в функции инициализации проинициализировать значения
// АССЕРТ ЛЕТИТ НА ФУНКЦИИ ПОП

// UNIT TEST-ы ДЛЯ СТЕКА

struct stack_t {
    MY_TYPE* data;
    int size;
    int capacity;
    ON_DBG(const char* name;
    const char* file;
    int line;)
};


#include "log.c"


int main() {
    log_start();

    stack_t stk1 = {};
    int err = 0;
    size_t len = 5;

    init_stk(&stk1, len, &err ON_DBG(, GET_VAR_NAME(stk1), __FILE__, __LINE__));
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
        ON_DBG(Dump(&stk1, err, "Debugging in pushing");)

    }
    for(int i = 0; i < 10; ++i) {
        MY_TYPE sec_last = Pop(&stk1, &err);

        printf("%f\n", sec_last);
        ON_DBG(Dump(&stk1, err, "Debugging in popping");)
    }

    MY_TYPE almost_last = Top(&stk1, &err);
    printf("%f\n", almost_last);

    stk_destroy(&stk1);

    ON_DBG(Dump(&stk1, err, "Debugging after destroying");)

    log_end();
    return 0;
}
// разбить все эти причины по одиночке и менять ошибки, вызывать дамп по разным причинам
void stk_verify(struct stack_t* stk, int err) {
    if(err != 0)
        Dump(stk, err, "called because error\n");

    if(stk -> size < 0)
        Dump(stk, err, "negative size of stack\n");

    if(stk -> capacity < 0)
        Dump(stk, err, "negative capacity of stack\n");

    if(stk -> size > stk -> capacity)
        Dump(stk, err, "stack size is more than capacity\n");
    /*
    if(stk -> data[-1] != NAN)
        Dump(stk, err, "Something wrong with LEFT canary\n");

    if(stk -> data[stk -> capacity - 1] != NAN)
        Dump(stk, err, "Something wrong with RIGHT canary\n");
    */
}

void init_stk(struct stack_t* stk, size_t len, int* err
              ON_DBG(, const char* name, const char* file, int line)) {

    ON_DBG(
    stk -> name = name;
    stk -> file = file;
    stk -> line = line;
    )

    assert(stk);
    assert(err);

    if(stk -> size != 0 || stk -> capacity != 0) *err = 1;

    // stk -> data = (MY_TYPE*)calloc(len + 2, sizeof(MY_TYPE)); // + 2 для канареек
    stk -> data = (MY_TYPE*)calloc(len, sizeof(MY_TYPE));
    /*
    stk -> data[0] = NAN;
    stk -> data[len + 1] = NAN;
    stk -> data += 1;
    */
    stk -> capacity = len;
    stk -> size = 0;

    assert(stk -> data != NULL);
}

void Dump(struct stack_t* stk, int err, ...) { // чем красивее тем лучше

    static int num = 1;

    // static char* buf = (char*)calloc(MXLN, sizeof(char));
    char buf[MXLN] = {};

    sprintf(buf,"\n    === It is Dump number %d. ===\n\n", num++);
    log_write_print(buf, YELLOW_TEXT);
    // free(buf);

    va_list reason;
    va_start(reason, err);

    const char* text = va_arg(reason, const char*);
    sprintf(buf, "    %s\n", text);
    log_write_print(buf, GREEN_TEXT);

    va_end(reason);

    /*
    va_list reason; // печать необязательной строки, идёт не так, как задумано
    // char* txt = NULL;
    va_start(reason, err);
    sprintf(buf, "%s\n", reason);
    log_write_print(buf, RED_TEXT);
    /*
    for(char* c = reason; *c; ++c) {
        printf("%c", c);
    }
    putchar('\n'); */

    sprintf(buf, "\tError number is %2d\n\n\t%s\n\n"
    "\tCapacity: %8d\n\n"
    "\tSize: %12d\n\n",
    err, errors[err], stk -> capacity, stk -> size);
    log_write_print(buf, YELLOW_TEXT);

    ON_DBG(

    sprintf(buf, "    DBG MODE:\n\n    Name of stack is %s\n\n"
    "    It was created in file %s\n\n"
    "    In line number: %d\n\n"
    , stk -> name, stk -> file, stk -> line);
    log_write_print(buf, RED_TEXT);

    )

    sprintf(buf, "\tCurrent stack:\n\n");
    log_write_print(buf, YELLOW_TEXT);

    // log_write_print("\n    === It is Dump number %d. ===\n\n")

    // PRINTING ELEMENTS
    if(stk -> data == NULL && stk -> size == 0 && stk -> capacity == 0) {

        sprintf(buf, "\tIt is an empty stack, after initialising or destroying.\n");
        log_write_print(buf, PURPLE_TEXT);

    } else {
        for(int i = 0; i < stk -> capacity; ++i) {
            if(i < stk -> size) {

                if(stk -> data[i] == POISON) {

                    sprintf(buf, "\t* [%d] %f\n", i, stk -> data[i]);
                    log_write_print(buf, RED_TEXT);

                } else {

                    sprintf(buf, "\t* [%d] %f\n", i, stk -> data[i]);
                    log_write_print(buf, GREEN_TEXT);

                }
            }
            else {

                if(stk -> data[i] == POISON) {

                    sprintf(buf, "\t[%d] %f\n", i, stk -> data[i]);
                    log_write_print(buf, BLUE_TEXT);

                } else {

                    sprintf(buf, "\t[%d] %f\n", i, stk -> data[i]);
                    log_write_print(buf, RED_TEXT);

                }
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
        assert(stk -> data != NULL);
        // assert(stk -> capacity > 0);

        stk -> data = (MY_TYPE*)realloc(stk -> data, (stk -> capacity /= 2) * sizeof(MY_TYPE));

        MY_TYPE* wtf = stk -> data;
        // printf("%p\n", wtf);

        // assert((stk -> capacity *= 0.5) != 0);

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
    free(stk -> data - 1);
    stk -> data = NULL;
    stk -> size = 0;
    stk -> capacity = 0;
}

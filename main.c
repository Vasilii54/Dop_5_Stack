#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

// #define NDEBUG // ИЛИ ИСПОЛЬЗОВАТЬ ФЛАГ -D NDEBUG чтобы отключить asserts
#include <assert.h>

typedef double MY_TYPE;

#define POISON (MY_TYPE)3802
#define MXLN 300
#define LEFT_PIGEON (MY_TYPE)0xEBADEDA
#define RIGHT_PIGEON (MY_TYPE)0xCAFEBABE

// #define STACK_DEBUG // РАССКОММЕНТИТЬ ДЛЯ РЕЖИМА ДЕБАГА
// ИЛИ ИСПОЛЬЗОВАТЬ ФЛАГ -D STACK_DEBUG

#ifdef STACK_DEBUG
#define ON_DBG(...) __VA_ARGS__
#else
#define ON_DBG(...)
#endif

#define GET_VAR_NAME(var) #var

void init_stk(struct stack_t* stk, size_t len, int* err
              ON_DBG(, const char* name, const char* file, int line, const char* func));

void Dump(struct stack_t* stk, int* err, ...);

void stk_verify(struct stack_t* stk, int* err);

void Push(struct stack_t* stk, MY_TYPE elem, int* err);

MY_TYPE Pop(struct stack_t* stk, int* err);

MY_TYPE Top(struct stack_t* stk, int* err);

void stk_destroy(struct stack_t* stk);

long long hash(MY_TYPE* data, size_t len);

const char* errors[] { // написать ошибки по уровням: info, debuf warning, error, fatal
                    // только fatal должен вызывать abort()
    "info: Everything is OK",
    "warning: Not defined right now",
    "error: You are deleting from empty stack",
    "error: negative len of stack in initialising",
    "warning: your stack size is zero. Is it normal?",
    "fatal: "
};
/* Already done:
    TODO: цветной вывод в Dump, VA ARGS в Dump, гистерезис, static переменные -
    TODO: сделать чтобы в дамп передавало причину дампа необязательным параметром
    TODO: stack verify - разбить на несколько условий и вызывать дампы по разным причинам, писать в дамп причину вызова
    TODO: канареечная защита самой структуры, класть с начала и с конца структуры
    TODO: добавить в дамп информацию о структуре, которая собирается в режиме ON_DBG
    TODO: в функции инициализации проинициализировать значения
    TODO: UNIT TEST-ы ДЛЯ СТЕКА
    TODO: хэшированную защиту, хеш для данных и хеш для стека.
    TODO: нарочно сломать стек и посмотреть, будет ли выявлена ошибка.
    TODO: починить __FUNCTION__, адресы в дампе
*/
#include "./log/log.c"

struct stack_t {
    MY_TYPE* data;
    MY_TYPE* array;
    size_t size;
    size_t capacity;
    long long hash_res;
    ON_DBG(const char* name;
    const char* file;
    unsigned long long int line;
    const char* func;)
};

int main() {
    log_start();
    int err = 0;

    stack_t stk1 = {};
    size_t len = 5;

    init_stk(&stk1, len, &err ON_DBG(, GET_VAR_NAME(stk1), __FILE__, __LINE__, __FUNCTION__));
    stk_verify(&stk1, &err);

    // Dump(&stk1, err, "");

    Push(&stk1, 11.1, &err);
    stk_verify(&stk1, &err);

    Push(&stk1, 22.2, &err);
    stk_verify(&stk1, &err);

    MY_TYPE last_el = Pop(&stk1, &err);

    printf("%f\n", last_el);

    for(int i = 1; i < 3; ++i) {

        Push(&stk1, (MY_TYPE)i, &err);
        stk_verify(&stk1, &err);
        ON_DBG(Dump(&stk1, &err, "Debugging in pushing");)

    }
    for(int i = 0; i < 2; ++i) {
        MY_TYPE sec_last = Pop(&stk1, &err);

        printf("%f\n", sec_last);
        ON_DBG(Dump(&stk1, &err, "Debugging in popping");)
    }

    MY_TYPE almost_last = Top(&stk1, &err);
    printf("%f\n", almost_last);

    // Dump(&stk1, &err, "");

    ON_DBG(Dump(&stk1, &err, "Debugging before destroying");)

    stk_destroy(&stk1);

    ON_DBG(Dump(&stk1, &err, "Debugging after destroying");)


    printf("\n   !!!   NORMAL WORK IS OWER. NOW LET`S BREAK STACK   !!!   \n\n");


    stack_t stk2 = {};
    size_t len2 = 0;
    // printf("%zd %zu\n", len2, len2);
    init_stk(&stk2, len2, &err ON_DBG(, GET_VAR_NAME(stk2), __FILE__, __LINE__, __FUNCTION__));
    stk_verify(&stk2, &err);

    // stk2.size = -1;
    stk_verify(&stk2, &err);

    stk2.size = 0;

    Push(&stk2, 333.33, &err);
    Pop(&stk2, &err);
    Pop(&stk2, &err);

    stk2.data[-1] = 0xA4A4A;
    stk2.data[stk2.capacity] = 67;
    stk_verify(&stk2, &err);

    printf("NOW STK NUMBER THREE\n");

    stack_t stk3 = {};
    size_t len3 = 4;
    init_stk(&stk3, len3, &err ON_DBG(, GET_VAR_NAME(stk3), __FILE__, __LINE__, __FUNCTION__));
    stk_verify(&stk3, &err);

    Push(&stk3, 55.33, &err);
    Push(&stk3, 378.33, &err);
    stk_verify(&stk3, &err);

    stk3.data[0] = 67;
    stk_verify(&stk3, &err);

    log_end();
    return 0;
}

// разбить все эти причины по одиночке и менять ошибки, вызывать дамп по разным причинам
void stk_verify(struct stack_t* stk, int* err) {
    if(stk -> hash_res == LEFT_PIGEON && stk -> size > 0)
        stk -> hash_res = hash(stk -> data, stk -> size);

    if(stk -> hash_res != hash(stk -> data, stk -> size))
        Dump(stk, err, "HASH IS NOT THE SAME\n");

    if(*err != 0)
        Dump(stk, err, "called because error\n");

    if(stk -> data == NULL)
        Dump(stk, err, "data pointer is NULL\n");

    if(stk -> array == NULL)
        Dump(stk, err, "array pointer is NULL\n");

    if(stk -> size < 0)
        Dump(stk, err, "negative size of stack\n");

    if(stk -> capacity < 0)
        Dump(stk, err, "negative capacity of stack\n");

    if(stk -> size > stk -> capacity)
        Dump(stk, err, "stack size is more than capacity\n");

    if(*(stk -> array) != LEFT_PIGEON)
        Dump(stk, err, "Something wrong with LEFT canary\n");

    if(stk -> data[stk -> capacity] != RIGHT_PIGEON)
        Dump(stk, err, "Something wrong with RIGHT canary\n");

}

void init_stk(struct stack_t* stk, size_t len, int* err
              ON_DBG(, const char* name, const char* file, int line, const char* func)) {

    ON_DBG(
        stk -> name = name;
        stk -> file = file;
        stk -> line = line;
        stk -> func = func;
    )

    if(len < 0) {
        // *err = 3;
        Dump(stk, err, "Negative stack length in initialising\n");
        return;
    }
    else if(len == 0) {
        // *err = 4;
        ON_DBG(Dump(stk, err, "In initialisation length of stack data is zero\n"));
    }

    assert(stk);
    assert(err);

    if(stk -> size != 0 || stk -> capacity != 0) *err = 1;

    stk -> array = (MY_TYPE*)calloc(len + 2, sizeof(MY_TYPE)); // + 2 для канареек
    // stk -> data = (MY_TYPE*)calloc(len, sizeof(MY_TYPE));
    assert(stk -> array != NULL);
    ///*
    stk -> array[0] = LEFT_PIGEON;
    stk -> array[len + 1] = RIGHT_PIGEON;
    stk -> data = stk -> array + 1;
    //*/
    stk -> capacity = len;
    stk -> size = 0;
    stk -> hash_res = LEFT_PIGEON;

    assert(stk -> array != NULL);
    assert(stk -> data != NULL);
}

void Dump(struct stack_t* stk, int* err, ...) { // чем красивее тем лучше

    static int num = 1;

    // static char* buf = (char*)calloc(MXLN, sizeof(char));
    char buf[MXLN] = {};

    sprintf(buf,"\n    === It is Dump number %d. ===\n", num++);
    log_write_print(buf, YELLOW_TEXT);
    // free(buf);

    va_list reason;
    va_start(reason, err);

    const char* text = va_arg(reason, const char*);
    sprintf(buf, "    %s\n", text);
    log_write_print(buf, GREEN_TEXT);

    va_end(reason);

    sprintf(buf, "\tError number is %2d\n\n\t%s\n\n"
    "\tCapacity: %8zu\n\n"
    "\tSize: %12zu\n\n",
    *err, errors[*err], stk -> capacity, stk -> size);
    log_write_print(buf, YELLOW_TEXT);

    ON_DBG(

    sprintf(buf, "    DBG MODE:\n\n    Name of stack is %s\n\n"
    "    It was created in file %s\n\n"
    "    In line number: %d\n\n"
    "    In function: %s\n\n"
    , stk -> name, stk -> file, stk -> line, stk -> func);
    log_write_print(buf, RED_TEXT);

    )

    sprintf(buf, "\tCurrent stack:\n\n");
    log_write_print(buf, YELLOW_TEXT);

    // PRINTING ELEMENTS
    if(stk -> data == NULL && stk -> array == NULL && stk -> size == 0 && stk -> capacity == 0) {

        sprintf(buf, "\tIt is an empty stack, after initialising or destroying.\n");
        log_write_print(buf, PURPLE_TEXT);

    } else {

        sprintf(buf, "\tLeft pigeon: %X\n", *(stk -> array));
        log_write_print(buf, BLUE_TEXT);

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

        sprintf(buf, "\tRight pigeon: %X\n", stk -> data[stk -> capacity]);
        log_write_print(buf, BLUE_TEXT);

    }
    printf("\n\e[0;36m    === End of Dump function. ===\n\n\e[0m");
}

void Push(struct stack_t* stk, MY_TYPE elem, int* err) {

    if(stk -> capacity == 0) {
        stk -> capacity = 4;
        stk -> array = (MY_TYPE*)realloc(stk -> array, stk -> capacity * sizeof(MY_TYPE));
        stk -> data = stk -> array + 1;
    }

    if(stk -> size >= stk -> capacity) {
        size_t new_cap = (stk -> capacity) + (stk -> capacity) / 2;
        size_t old = stk -> capacity;

        stk -> array = (MY_TYPE*)realloc(stk -> array, (new_cap + 2) * sizeof(MY_TYPE));

        assert(stk -> array != NULL);
        assert(stk -> data != NULL);

        stk -> capacity = new_cap;

        stk -> data = stk -> array + 1;

        memset(stk -> data + old, 0, (stk -> capacity - old) * sizeof(MY_TYPE));
        stk->array[stk -> capacity + 1] = RIGHT_PIGEON;
    }

    stk -> data[stk -> size++] = elem;

    stk -> hash_res = hash(stk -> data, stk -> size);
}

MY_TYPE Pop(struct stack_t* stk, int* err) {
    if(stk -> size == 0) {
        *err = 2;
        Dump(stk, err, "Popping from empty stack");
        return NAN;
    }

    stk -> size--;

    MY_TYPE elem = stk -> data[stk -> size];

    stk -> data[stk -> size] = POISON;

    if(stk -> size * 2 < stk -> capacity) {
        assert(stk -> data != NULL);
        // assert(stk -> capacity > 0);

        size_t new_cap = (stk -> capacity) / 2 + 2;

        stk -> array = (MY_TYPE*)realloc(stk -> array, (new_cap + 2) * sizeof(MY_TYPE));

        stk -> capacity = new_cap;

        stk -> data = stk -> array + 1;

        // assert((stk -> capacity *= 0.5) != 0);
        assert(stk -> array != NULL);
        assert(stk -> data != NULL);

        stk -> array[0] = LEFT_PIGEON;
        stk -> array[stk->capacity + 1] = RIGHT_PIGEON;
    }

    stk -> hash_res = hash(stk -> data, stk -> size);

    return elem;
}

MY_TYPE Top(struct stack_t* stk, int* err) {
    if(stk -> size == 0) {
        *err = 2;
        Dump(stk, err, "Stack is empty but you want to get top element");
        return NAN;
    }

    MY_TYPE elem = stk -> data[stk -> size - 1];

    return elem;
}

// https://github.com/paulxcalder/hash_table
long long hash(MY_TYPE* data, size_t len) {
    if(len == 0)
        return LEFT_PIGEON;
    long long res = 0;
    for(size_t i = 0; i < len; ++i) {
        res = ((res << 5) + res) + (long long)data[i];
    }
    return res;
}

void stk_destroy(struct stack_t* stk) {
    free(stk -> array);
    stk -> data = NULL;
    stk -> array = NULL;
    stk -> size = 0;
    stk -> capacity = 0;
}

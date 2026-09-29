#include <stdio.h>
#include <stdlib.h>

#include "log.c"

int main() {
    log_start();
    char str1[] = "Hello it is Misha code";
    char str2[] = "second string";
    // log_write(str1);
    log_write_print(str1, RED_TEXT);
    log_write_print(str1, INFO);
    log_write_print(str1, ERROR);
    log_write_print(str1, CIAN_TEXT);
    log_write_print(str1, PURPLE_TEXT);
    // log_write(WHITE_TEXT str2);
    log_end();
    return 0;
}

/*
    if (fp == NULL) {
        printf("%sFile data reading error: file doesn't exist%s\n", ERROR, DEFAULT);
        log_write_and_printf("File data reading error: file doesn't exist\n");

        return 0;
    }
*/

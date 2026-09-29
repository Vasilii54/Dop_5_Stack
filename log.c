/*! @file               log.c
 *  @brief              Функции, связанные с записью логов в лог-файл
 */

/* В данном файле назначается стандартное имя лог-файла: test.log.
 *
 * Для хранения данного имени используется глобальная переменная, поскольку имя лог-файла используется во многих местах программы и по её логике не переназначается,
 * чтобы не передавать его каждый раз из main() во все вложенные функции с логированием. Поскольку в будущем планируется реализовать возможность ввода пользовательского
 * имени лог-файла в аргументах терминала при запуске программы, данная глобальная переменная не является константной.
 *
 * В теории, можно сделать проверку через assert() при открытии файла, но, поскольку для случая ошибки открытия программы задумано поведение без какого-либо вывода
 * логов в файл с предупреждением об этом пользователя, этого не было сделано.
 */


#ifndef __LOG_C__
#define __LOG_C__


#include <assert.h>

#include "colors.c"


#define MAX_LOGFILE_NAME_LEN 255 // Максимальная длина имени лог-файла
#define MAX_LOG_STRING_SIZE 1024 // Максимальный размер для строки, направляемой в лог


// Глобальные переменные
char logfile[MAX_LOGFILE_NAME_LEN] = "test.log"; // Имя лог-файла
FILE *log_fp = nullptr; // Инициализация потока лог-файла



// Прототип функции для очистки буфера вывода
void cleanLogBuffer(char logBuffer[], int bufferLength);

// Прототип функции для начала записи логов
int log_start();

// Прототип функции для записи логов
int log_write(char line[]);

// Прототип функции для записи логов в файл и вывода той же строки в консоль
int log_write_print(char line[], const char color[]);

// Прототип функции для конца записи логов
void log_end(void);


/*!                Функция для очистки буфера вывода
 *  @param [in]    logBuffer        буфер вывода в лог
 *  @param [in]    bufferLength     размер буфера вывода
 */
void cleanLogBuffer (char logBuffer[], int bufferLength) {
    for (int i = 0; i < bufferLength; i++) {
        logBuffer[i] = '\0';
    }
}


/*!                Функция для начала записи логов в файл logfile в каталоге программы
 *   @n            В случае, если файл отсутствует, создаёт его, иначе открывает и очищает содержимое
 *  @return        1 в случае ошибки открытия или создания файла,
 *   @n            0 в случае успешного открытия файла и записи метки начала лога
 */
int log_start(void) {
    log_fp = fopen(logfile, "w"); // Открытие файла

    if (log_fp == NULL) {
        printf("%sError of opening or creating the log file. Logs will not be written%s\n", ERROR, DEFAULT);
        return 1;
    }
    else {
        assert(log_fp != NULL);
        fprintf(log_fp, "--- Beginning of log file ---\nTests started.\n");
    }

    return 0;
}


/*!                Функция для записи логов в файл logfile в каталоге программы
 *  @param [in]    line             строка для записи в лог
 */
int log_write(char line[]) {
    if (log_fp == NULL) {
        return 1;
    }
    else {
        assert(log_fp != NULL);
        fprintf(log_fp, "%s\n", line);
    }

    return 0;
}


/*!                Функция для записи логов в файл logfile в каталоге программы и вывода той же строки в консоль
 *  @param [in]    line             строка для записи в лог и вывода в консоль
 */
int log_write_print(char line[], const char color[]) {
    if (log_fp == NULL) {
        return 1;
    }
    else {
        assert(log_fp != NULL);
        fprintf(log_fp, "%s\n", line);
    }

    printf("%s%s%s\n", color, line, RESET);

    return 0;
}


/*!                Функция для конца записи логов в файл logfile в каталоге программы
 *   @n            Запускает функцию log_write для записи строки с меткой конца лог-файла
 */
void log_end(void) {
    char output[] = "-------------------------------\n--- End of log file ---";
    int status = log_write(output);
    if (status == 0) {
        assert (log_fp != NULL);
        fclose(log_fp);
    }
}


#endif

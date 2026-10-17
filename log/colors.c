/*! @file               colors.c
 *  @brief              Файл с некоторыми кодами цветов в терминале
 */


#ifndef __COLORS_C__
#define __COLORS_C__

// Цвета текста
#define BLACK_TEXT  "\e[0;30m"
#define RED_TEXT    "\e[0;31m"
#define GREEN_TEXT  "\e[0;32m"
#define YELLOW_TEXT "\e[0;33m"
#define BLUE_TEXT   "\e[0;34m"
#define PURPLE_TEXT "\e[0;35m"
#define CIAN_TEXT   "\e[0;36m"
#define WHITE_TEXT  "\e[0;37m"
/*
#define BLACK_BG    "\e[40m"
#define RED_BG      "\e[41m"
#define YELLOW_BG   "\e[43m"
#define BLUE_BG     "\e[44m"
*/
// Цвета фона
#define BLACK_BG  "\e[0;40m"
#define RED_BG    "\e[0;41m"
#define GREEN_BG  "\e[0;42m"
#define YELLOW_BG "\e[0;43m"
#define BLUE_BG   "\e[0;44m"
#define PURPLE_BG "\e[0;45m"
#define CIAN_BG   "\e[0;46m"
#define WHITE_BG  "\e[0;47m"

// Стили текста
#define BOLD 1
#define PALE 2
#define CURLY 3
#define UNDERLINED 4
#define FLASHING 5
#define DRAWN 9

#define RED_BG_HI "\e[1;91m"

#define RESET "\e[0m"

#define DEFAULT BLACK_TEXT BLUE_BG // Делаем стандартным синий цвет фона и чёрный цвет текста
#define INFO BLACK_TEXT YELLOW_BG // Цветовая схема информативного текста
#define ERROR WHITE_TEXT RED_BG // Цветовая схема текста ошибок
#define MAIN BOLD CIAN_TEXT WHITE_BG

#endif

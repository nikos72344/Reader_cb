#ifndef __SCROLLS_H
#define __SCROLLS_H

#include <Windows.h>

#define ScrollSize 65536 // - макрос, используемый для решения проблемы ограниченных значений диапазона полосы прокрутки

//Функция обновления данных вывода текста

void PrintMetrixUpd(SystemState* SState);

//Функции обновления данных структуры скроллов

void vScrollUpd(SystemState* SState, HWND hwnd);

void hScrollUpd(SystemState* SState, HWND hwnd);

void ScrollsReset(SystemState* SState, HWND hwnd);

//Функции синхронизации выводимых строк

void ToLayout(SystemState* SState, HWND hwnd);

void ToDefault(SystemState* SState, HWND hwnd);

#endif
#ifndef __SCROLLS_H
#define __SCROLLS_H

#include <Windows.h>

#define ScrollSize 65536 // - макрос, используемый для решения пролемы ограниченных значений диапазона полосы прокрутки

//Функция обновления данных вывода текста

void PrintMetrixUpd(SysState* SState, RECT* RCT);

//Функции обновления данных структуры скроллов

void vScrollUpd(SysState* SState, HWND hwnd);

void hScrollUpd(SysState* SState, HWND hwnd);

void ScrollsReset(SysState* SState, HWND hwnd);

//Функция изменения заголовка окна в зависимости от открытого файла

void ChangeWndTitle(SysState* SState, HWND hwnd);

//Функция обработка агрументов командной строки

void Args(SysState* SState, HWND hwnd, LPARAM lParam);

#endif
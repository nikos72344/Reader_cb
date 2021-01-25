#ifndef __FILE_H
#define __FILE_H

#include"scrolls.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#define Buffer 10  // - размер буфера выделения памяти

//Функция инициализации структуры OPENFILENAME

void OFNInitialize(SystemState* SState, HWND hwnd);

//Функция обработка агрументов командной строки

void ArgumentsProcessing(SystemState* SState, HWND hwnd, LPARAM lParam);

//Функция работы с входным файлом

void OpenFileProcessing(SystemState* SState, HWND hwnd);

//Функция перерасчета начала строк

void TextResizer(SystemState* SState, HWND hwnd);

#endif
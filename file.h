#ifndef __FILE_H
#define __FILE_H

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

#define szBuffer 10  // - размер буфера для считывания

//Функция освобождения памяти

void ReleaseMemory(String* Strings, unsigned int StrNum);

//Функция считывания текста из файла

void ReadText(SysState* SState);

//Функция инициализации структуры OPENFILENAME

void FileInitialize(SysState* SState, HWND hwnd);

#endif

#ifndef __FUNCS_H
#define __FUNCS_H

#include <Windows.h>

#define autoScroll 10 // - макрос, значящий часть окна при прокрутке колеском мыши или клавиатурой

//Функции обработки сообщений Windows

void CREATE_FUNC(SysState* SState, HWND hwnd, LPARAM lParam);

void SIZE_FUNC(SysState* SState, HWND hwnd);

void PAINT_FUNC(SysState* SState, HWND hwnd);

void COMMAND_FUNC(SysState* SState, HWND hwnd, WPARAM wParam);

void VSCROLL_FUNC(SysState* SState, HWND hwnd, WPARAM wParam);

void HSCROLL_FUNC(SysState* SState, HWND hwnd, WPARAM wParam);

void AUTOvSCROLL_FUNC(SysState* SState, HWND hwnd, int vScrollDiff);

void AUTOhSCROLL_FUNC(SysState* SState, HWND hwnd, int hScrollDiff);

void DESTROY_FUNC(SysState* SState);

#endif

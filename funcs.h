#ifndef __FUNCS_H
#define __FUNCS_H

#include <Windows.h>

#define autoScroll 10 // - макрос, знач€щий часть окна при прокрутке колеском мыши или клавиатурой

//‘ункции обработки сообщений Windows

void CREATE_FUNC(SystemState* SState, HWND hwnd, LPARAM lParam);

void SIZE_FUNC(SystemState* SState, HWND hwnd, LPARAM lParam);

void PAINT_FUNC(SystemState* SState, HWND hwnd);

void COMMAND_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam);

void VSCROLL_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam);

void HSCROLL_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam);

void AUTOvSCROLL_FUNC(SystemState* SState, HWND hwnd, int vScrollDiff);

void AUTOhSCROLL_FUNC(SystemState* SState, HWND hwnd, int hScrollDiff);

void DESTROY_FUNC(SystemState* SState);

#endif
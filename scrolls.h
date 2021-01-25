#ifndef __SCROLLS_H
#define __SCROLLS_H

#include <Windows.h>

#define ScrollSize 65536 // - ������, ������������ ��� ������� �������� ������������ �������� ��������� ������ ���������

//������� ���������� ������ ������ ������

void PrintMetrixUpd(SystemState* SState);

//������� ���������� ������ ��������� ��������

void vScrollUpd(SystemState* SState, HWND hwnd);

void hScrollUpd(SystemState* SState, HWND hwnd);

void ScrollsReset(SystemState* SState, HWND hwnd);

//������� ������������� ��������� �����

void ToLayout(SystemState* SState, HWND hwnd);

void ToDefault(SystemState* SState, HWND hwnd);

#endif
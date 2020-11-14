#ifndef __SCROLLS_H
#define __SCROLLS_H

#include <Windows.h>

#define ScrollSize 65536 // - ������, ������������ ��� ������� ������� ������������ �������� ��������� ������ ���������

//������� ���������� ������ ������ ������

void PrintMetrixUpd(SysState* SState, RECT* RCT);

//������� ���������� ������ ��������� ��������

void vScrollUpd(SysState* SState, HWND hwnd);

void hScrollUpd(SysState* SState, HWND hwnd);

void ScrollsReset(SysState* SState, HWND hwnd);

//������� ��������� ��������� ���� � ����������� �� ��������� �����

void ChangeWndTitle(SysState* SState, HWND hwnd);

//������� ��������� ���������� ��������� ������

void Args(SysState* SState, HWND hwnd, LPARAM lParam);

#endif
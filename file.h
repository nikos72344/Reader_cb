#ifndef __FILE_H
#define __FILE_H

#include"scrolls.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#define Buffer 10  // - ������ ������ ��������� ������

//������� ������������� ��������� OPENFILENAME

void OFNInitialize(SystemState* SState, HWND hwnd);

//������� ��������� ���������� ��������� ������

void ArgumentsProcessing(SystemState* SState, HWND hwnd, LPARAM lParam);

//������� ������ � ������� ������

void OpenFileProcessing(SystemState* SState, HWND hwnd);

//������� ����������� ������ �����

void TextResizer(SystemState* SState, HWND hwnd);

#endif
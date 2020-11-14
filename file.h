#ifndef __FILE_H
#define __FILE_H

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

#define szBuffer 10  // - ������ ������ ��� ����������

//������� ������������ ������

void ReleaseMemory(String* Strings, unsigned int StrNum);

//������� ���������� ������ �� �����

void ReadText(SysState* SState);

//������� ������������� ��������� OPENFILENAME

void FileInitialize(SysState* SState, HWND hwnd);

#endif

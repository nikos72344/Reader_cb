#ifndef __LAB_1_H
#define __LAB_1_H

#include <Windows.h>

//������� ��������������� ������ ����

#define IDM_OPEN 1

#define IDM_EXIT 2

#define IDM_LAYOUT 3

#define IDM_ABOUT 4

//������������ ������� ����������� ������

typedef enum
{
  defaultV, // - �� ���������
  layoutV // - � ��������
}viewType;

//������ �������� ������

typedef struct
{
  char* data;  // - ������ ��������� ������
  int size; // - ������ �������
}String;

//�������� ���������� ������ ������ �������������:

typedef struct
{
  int vScrollPos, vScrollMax, vScrollCoef;
  int hScrollPos, hScrollMax, hScrollCoef;
}ScrlMetrix;  // - ��������� ������ ����� ���������

typedef struct
{
  int pntBeg, pntEnd;
  int maxLen;
}PrntMetrix;  // - ��������� ������ ���������� ������ �� �����

//������ �������������

typedef struct
{
  viewType vType; // - ����� �����������
  int xChar, yChar; // - ����� � ������ ������� ������
  int xClient, yClient; // - ������ ���������� ������� ����
  ScrlMetrix scrlMetrix; // - ������ ����� ���������
  String* strings;   // - ������ ����� ��������� ������
  int strNum, sumNum; // - ���������� ����� ��� � � ������ ������� ��������������
  PrntMetrix prntMetrix; // - ������� ���������� ������
  HFONT font; // - ������������ �����
  OPENFILENAME ofn; // - ��������� ��� ������������� ������� �������� �����
  char* flName; // - ��������� �� ������ �������� ������ ������������ �����
  char* ttlName; // - ��������� �� ������ �������� ������ �������� �����
}SysState;

#endif

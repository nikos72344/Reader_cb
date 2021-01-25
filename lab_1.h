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
}Text;

//��������� ������ ������

typedef struct {
  int begIndex; // - ������ ������ ������ � �������
  viewType type;  // - ��� ������ ������
}strBeg;

//��������� ������� ������ �����

typedef struct {
  strBeg* stringBeg; // - ������ ������� ������ �����
  int size; // - ���-�� �����
  int arraySize;  // - ������ �������
}ParserMetrix;

//�������� ���������� ������ ������ �������������:

typedef struct
{
  int pos, max, coef;
}ScrollMetrix;  // - ��������� ������ ����� ���������

typedef struct
{
  int beg, end;
  int maxLen;
}PrintMetrix;  // - ��������� ������ ���������� ������ �� �����

//������ �������������

typedef struct
{
  viewType vType; // - ����� �����������
  int xChar, yChar; // - ����� � ������ ������� ������
  int xClient, yClient; // - ������ ���������� ������� ����
  Text text;   // - ������ ��������� ������
  HFONT font; // - ������������ �����
  ScrollMetrix vScroll, hScroll; // - ������ ����� ���������
  ParserMetrix defParser, layParser;  // - ��������� �������� ��� ������ �� ��������� � ������ �������
  PrintMetrix printMetrix; // - ������� ���������� ������
  char* filePath; // - ��������� �� ������ �������� ������ ������������ �����
  char* fileTitle; // - ��������� �� ������ �������� ������ �������� �����
  OPENFILENAMEA ofn; // - ��������� ��� ������������� ������� �������� �����
}SystemState;

#endif

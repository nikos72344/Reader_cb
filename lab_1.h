#ifndef __LAB_1_H
#define __LAB_1_H

#include <Windows.h>

//Макросы идентификаторов кнопок меню

#define IDM_OPEN 1

#define IDM_EXIT 2

#define IDM_LAYOUT 3

#define IDM_ABOUT 4

//Перечисление режимов отображения текста

typedef enum
{
  defaultV, // - по умолчанию
  layoutV // - с версткой
}viewType;

//Модель хранения данных

typedef struct
{
  char* data;  // - массив текстовых данных
  int size; // - размер массива
}String;

//Стуктуры компановок данных модели представления:

typedef struct
{
  int vScrollPos, vScrollMax, vScrollCoef;
  int hScrollPos, hScrollMax, hScrollCoef;
}ScrlMetrix;  // - описывает данные полос прокрутки

typedef struct
{
  int pntBeg, pntEnd;
  int maxLen;
}PrntMetrix;  // - описывает данные выводимого текста на экран

//Модель представления

typedef struct
{
  viewType vType; // - режим отображения
  int xChar, yChar; // - длина и высота символа текста
  int xClient, yClient; // - размер клиентской области окна
  ScrlMetrix scrlMetrix; // - данные полос прокрутки
  String* strings;   // - массив строк текстовых данных
  int strNum, sumNum; // - количество строк без и с учетом метрики соответственно
  PrntMetrix prntMetrix; // - метрика выводимого текста
  HFONT font; // - используемый шрифт
  OPENFILENAME ofn; // - структура для инициализации диалога открытия файла
  char* flName; // - указатель на массив хранения строки расположения файла
  char* ttlName; // - указатель на массив хранения строки названия файла
}SysState;

#endif

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
}Text;

//Структура начала строки

typedef struct {
  int begIndex; // - индекс начала строки в массиве
  viewType type;  // - тип начала строки
}strBeg;

//Структура массива начала строк

typedef struct {
  strBeg* stringBeg; // - массив индеков начала строк
  int size; // - кол-во строк
  int arraySize;  // - размер массива
}ParserMetrix;

//Стуктуры компановок данных модели представления:

typedef struct
{
  int pos, max, coef;
}ScrollMetrix;  // - описывает данные полос прокрутки

typedef struct
{
  int beg, end;
  int maxLen;
}PrintMetrix;  // - описывает данные выводимого текста на экран

//Модель представления

typedef struct
{
  viewType vType; // - режим отображения
  int xChar, yChar; // - длина и высота символа текста
  int xClient, yClient; // - размер клиентской области окна
  Text text;   // - массив текстовых данных
  HFONT font; // - используемый шрифт
  ScrollMetrix vScroll, hScroll; // - данные полос прокрутки
  ParserMetrix defParser, layParser;  // - структуры парсинга для режима по умолчанию и режима верстки
  PrintMetrix printMetrix; // - метрика выводимого текста
  char* filePath; // - указатель на массив хранения строки расположения файла
  char* fileTitle; // - указатель на массив хранения строки названия файла
  OPENFILENAMEA ofn; // - структура для инициализации диалога открытия файла
}SystemState;

#endif

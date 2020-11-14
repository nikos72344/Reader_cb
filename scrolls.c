#include "lab_1.h"
#include"scrolls.h"
#include "file.h"

//Функция обновления данных вывода текста

void PrintMetrixUpd(SysState* SState, RECT* RCT)
{
  int width;

  //Нахождения индекса последней выводимой строки на видимую область экрана

  int fin = min(SState->strNum, SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef + RCT->bottom / SState->yChar);

  //Нахождение индекса первой выводимой строки на экран

  SState->prntMetrix.pntBeg = max(0, SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef + RCT->top / SState->yChar);

  //Нахождения индекса последней выводимой на экран строки

  SState->prntMetrix.pntEnd = min(SState->strNum, SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef + 2 * RCT->bottom / SState->yChar);

  //В программе выводятся дополнительные строки для исключения мерцания при пролистывании

  SState->prntMetrix.maxLen = 0; // - обнуление переменной размера строки максимальной длинны
  SState->sumNum = SState->strNum; // - сборс количества строк с учетом метрики для режима верстки
  for (int i = SState->prntMetrix.pntBeg; i < SState->prntMetrix.pntEnd; i++)
  {
    width = SState->xChar * (SState->strings[i].size + 1) / SState->xClient;
    SState->sumNum += width; // - учитывание отображения перенесенных строк в режиме верстки
    if (i < fin && SState->prntMetrix.maxLen < ((SState->strings[i].size + 1) * SState->xChar))
      SState->prntMetrix.maxLen = (SState->strings[i].size + 1) * SState->xChar; // - нахождение значения самой длинной строки, выводимой на экран
  }
}

//Функция обновления значений вертикальной полосы прокрутки

void vScrollUpd(SysState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - в случае режима верстки коэффициент и максимальное значение высчитываются исходя из количества строк с учетом метрики отображения
    SState->scrlMetrix.vScrollCoef = SState->sumNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->sumNum / SState->scrlMetrix.vScrollCoef - 1);
    break;
  case defaultV: // - в случае режима по умолчанию коэффициент и максимальное значение высчитываются исходя из количества строк без учета метрики отображения
    SState->scrlMetrix.vScrollCoef = SState->strNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->strNum / SState->scrlMetrix.vScrollCoef - 1);
    break;
  }

  //Предотвращение выхода значения положения ползунка за установленные границы

  SState->scrlMetrix.vScrollPos = min(SState->scrlMetrix.vScrollPos, SState->scrlMetrix.vScrollMax);

  //Установка диапазона и положения ползунка

  SetScrollRange(hwnd, SB_VERT, 0, SState->scrlMetrix.vScrollMax, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE);
}

//Функция обновления значений горизонтальной полосы прокрутки

void hScrollUpd(SysState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - в случае режима верстки горизонтальная полоса прокрутки "выключается"
    SState->scrlMetrix.hScrollMax = 0;
    SState->scrlMetrix.hScrollPos = 0;
    SState->scrlMetrix.hScrollCoef = 0;
    break;
  case defaultV:  // - в случае режима по умолчанию коэффициент, максимальное значение и положение ползунка высчитываются исходя из максимальной длины отображаемой строки
    SState->scrlMetrix.hScrollCoef = SState->prntMetrix.maxLen / ScrollSize + 1;
    SState->scrlMetrix.hScrollMax = max(0, (SState->prntMetrix.maxLen / SState->scrlMetrix.hScrollCoef - SState->xClient + SState->xChar) / SState->xChar);
    SState->scrlMetrix.hScrollPos = min(SState->scrlMetrix.hScrollPos, SState->scrlMetrix.hScrollMax);
    break;
  }

  //Установка диапазона и положения ползунка

  SetScrollRange(hwnd, SB_HORZ, 0, SState->scrlMetrix.hScrollMax, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE);
}

//Функция сбрасывающая положения горизонтальной и вертикальной полос прокрутки

void ScrollsReset(SysState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - в случае режима верстки

    //Вертикальная полоса прокрутки высчитывается с учетом количества строк с учетом метрики отображения

    SState->scrlMetrix.vScrollCoef = SState->sumNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->sumNum / SState->scrlMetrix.vScrollCoef - 1);

    //Горизонтальная полоса "выключается"

    SState->scrlMetrix.hScrollMax = 0;
    SState->scrlMetrix.hScrollCoef = 0;
    break;
  case defaultV: // - в случае режима по умолчанию

    //Вертикальная полоса прокрутки высчитывается с учетом количества строк без учета метрики отображения

    SState->scrlMetrix.vScrollCoef = SState->strNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->strNum / SState->scrlMetrix.vScrollCoef - 1);

    //Горизонтальная полоса прокрутки высчитывается с учетом максимальной длины отображаемой строки

    SState->scrlMetrix.hScrollCoef = SState->prntMetrix.maxLen / ScrollSize + 1;
    SState->scrlMetrix.hScrollMax = max(0, (SState->prntMetrix.maxLen / SState->scrlMetrix.hScrollCoef - SState->xClient + SState->xChar) / SState->xChar);
    break;
  }

  //Сброс позиции ползунков

  SState->scrlMetrix.vScrollPos = 0;
  SState->scrlMetrix.hScrollPos = 0;

  //Установка соответствующих диапазонов и положений ползунков

  SetScrollRange(hwnd, SB_VERT, 0, SState->scrlMetrix.vScrollMax, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE);

  SetScrollRange(hwnd, SB_HORZ, 0, SState->scrlMetrix.hScrollMax, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE);
}

//Функция, меняющая заголовок окна

void ChangeWndTitle(SysState* SState, HWND hwnd)
{
  const char postfix[] = " - Reader"; // - постфикс
  int size = strlen(SState->ofn.lpstrFileTitle) + strlen(postfix) + 1;
  char* title = malloc(size * sizeof(char));  // - выделение памяти для буфера
  if (!title) // - в случае неудачного выделения заголовок останется прежний, а пользователь будет уведомлен об этом звуковым сигналом
  {
    MessageBeep(0);
    return;
  }

  //Заполнение буфера

  strcpy(title, SState->ofn.lpstrFileTitle);
  strcat(title, postfix);
  SetWindowText(hwnd, title);  // - установка нового заголовка
  free(title);  // - освобождение буфера
}

//Функция, обрабатывающая аргументы строки

void Args(SysState* SState, HWND hwnd, LPARAM lParam)
{
  //Получение аргументов командной строки

  CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
  RECT rect;
  char* temp = cs->lpCreateParams;
  if (!strcmp(temp, ""))  // - выход из ф-ии в случае отсутствия аргументов
    return;

  //Заполнение соответствующих массивов

  for (int i = 0; i <= (int)strlen(temp); i++)
  {
    SState->flName[i] = temp[i];
    SState->ttlName[i] = temp[i];
  }
  ReadText(SState);  // - чтение текста из файла

  //Обновление данных

  GetClientRect(hwnd, &rect);
  PrintMetrixUpd(SState, &rect);
  vScrollUpd(SState, hwnd);
  hScrollUpd(SState, hwnd);
  ChangeWndTitle(SState, hwnd);
}

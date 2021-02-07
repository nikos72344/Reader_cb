#include "lab_1.h"
#include"scrolls.h"
#include "file.h"

//Функция обновления данных вывода текста

void PrintMetrixUpd(SystemState* SState)
{
  int stringSize;
  switch (SState->vType)
  {
  case layoutV: stringSize = SState->layParser.size; break;
  case defaultV: stringSize = SState->defParser.size; break;
  }

  //Нахождение индекса первой выводимой строки на экран

  SState->printMetrix.beg = max(0, SState->vScroll.pos * SState->vScroll.coef);

  //Нахождения индекса последней выводимой на экран строки
  //В программе выводятся дополнительные строки для исключения мерцания при пролистывании

  SState->printMetrix.end = min(stringSize, SState->vScroll.pos * SState->vScroll.coef + 2 * SState->yClient / SState->yChar);

}

//Функция обновления значений вертикальной полосы прокрутки

void vScrollUpd(SystemState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - в случае режима верстки коэффициент и максимальное значение высчитываются исходя из количества строк с учетом метрики отображения
    SState->vScroll.coef = SState->layParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, (SState->layParser.size - SState->yClient / SState->yChar) / SState->vScroll.coef);
    break;
  case defaultV: // - в случае режима по умолчанию коэффициент и максимальное значение высчитываются исходя из количества строк без учета метрики отображения
    SState->vScroll.coef = SState->defParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, (SState->defParser.size - SState->yClient / SState->yChar) / SState->vScroll.coef);
    break;
  }

  //Предотвращение выхода значения положения ползунка за установленные границы

  SState->vScroll.pos = min(SState->vScroll.pos, SState->vScroll.max);

  //Установка диапазона и положения ползунка

  SetScrollRange(hwnd, SB_VERT, 0, SState->vScroll.max, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE);
}

//Функция обновления значений горизонтальной полосы прокрутки

void hScrollUpd(SystemState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - в случае режима верстки горизонтальная полоса прокрутки "выключается"
    SState->hScroll.max = 0;
    SState->hScroll.pos = 0;
    SState->hScroll.coef = 0;
    break;
  case defaultV:  // - в случае режима по умолчанию коэффициент, максимальное значение и положение ползунка высчитываются исходя из максимальной длины отображаемой строки
    SState->hScroll.coef = SState->printMetrix.maxLen / ScrollSize + 1;
    SState->hScroll.max = max(0, (SState->printMetrix.maxLen - SState->xClient / SState->xChar + 1) / SState->hScroll.coef);
    SState->hScroll.pos = min(SState->hScroll.pos, SState->hScroll.max);
    break;
  }

  //Установка диапазона и положения ползунка

  SetScrollRange(hwnd, SB_HORZ, 0, SState->hScroll.max, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE);
}

//Функция сбрасывающая положения горизонтальной и вертикальной полос прокрутки

void ScrollsReset(SystemState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - в случае режима верстки

    //Вертикальная полоса прокрутки высчитывается с учетом количества строк с учетом метрики отображения

    SState->vScroll.coef = SState->layParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, (SState->layParser.size - SState->yClient / SState->yChar) / SState->vScroll.coef);

    //Горизонтальная полоса "выключается"

    SState->hScroll.max = 0;
    SState->hScroll.coef = 0;
    break;
  case defaultV: // - в случае режима по умолчанию

    //Вертикальная полоса прокрутки высчитывается с учетом количества строк без учета метрики отображения

    SState->vScroll.coef = SState->defParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, (SState->defParser.size - SState->yClient / SState->yChar) / SState->vScroll.coef);

    //Горизонтальная полоса прокрутки высчитывается с учетом максимальной длины отображаемой строки

    SState->hScroll.coef = SState->printMetrix.maxLen / ScrollSize + 1;
    SState->hScroll.max = max(0, (SState->printMetrix.maxLen - SState->xClient / SState->xChar + 1) / SState->hScroll.coef);
    break;
  }

  //Сброс позиции ползунков

  SState->vScroll.pos = 0;
  SState->hScroll.pos = 0;

  //Установка соответствующих диапазонов и положений ползунков

  SetScrollRange(hwnd, SB_VERT, 0, SState->vScroll.max, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE);

  SetScrollRange(hwnd, SB_HORZ, 0, SState->hScroll.max, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE);
}

//Функция синхронизации выводимых строк при включении режима верстки

void ToLayout(SystemState* SState, HWND hwnd)
{
    int i;
  for (i = 0; i < SState->layParser.size; i++)  // - нахождени первой выводимой строки
  {
    if (&SState->text.data[SState->layParser.stringBeg[i].begIndex] == &SState->text.data[SState->defParser.stringBeg[SState->printMetrix.beg].begIndex])
    {
      SState->vScroll.pos = min(i / SState->vScroll.coef, SState->vScroll.max); // - установка соответствующего положения ползунка
      SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE);
      break;
    }
  }
}

//Функция синхронизации выводимых строк при включении режима отображения по умолчанию

void ToDefault(SystemState* SState, HWND hwnd)
{
    int i;
  int ind = SState->printMetrix.beg;
  while (SState->layParser.stringBeg[ind].type != defaultV) // - нахождение начала ближайшей неперенесенной строки
    ind--;
  for (i = 0; i < SState->defParser.size; i++)
  {
    if (&SState->text.data[SState->defParser.stringBeg[i].begIndex] == &SState->text.data[SState->layParser.stringBeg[ind].begIndex])
    {
      SState->vScroll.pos = min(i / SState->vScroll.coef, SState->vScroll.max); // - установка соответствующего положения ползунка
      break;
    }
  }
}

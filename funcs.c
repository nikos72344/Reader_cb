#include "lab_1.h"
#include "file.h"
#include "funcs.h"
#include "scrolls.h"

//Функция создания окна

void CREATE_FUNC(SysState* SState, HWND hwnd, LPARAM lParam)
{
  HDC hdc;
  RECT rect;
  HMENU hMenu;
  TEXTMETRIC tm;
  static char fileName[_MAX_PATH], titleName[_MAX_FNAME + _MAX_EXT]; // - статические массивы для хранения расположения и названия файла

  hdc = GetDC(hwnd); // - получение описателя контекста устройства

  SState->vType = defaultV; // - установка режима отображения текста по умолчанию
  hMenu = GetMenu(hwnd); // - получение описателя меню
  CheckMenuItem(hMenu, IDM_LAYOUT, MF_UNCHECKED); // - "выключение" кнопки режима верстки

  GetTextMetrics(hdc, &tm); // - получение размера текстовых символов на экране
  SState->xChar = tm.tmAveCharWidth; // - запись в структуру ширины символа
  SState->yChar = tm.tmHeight + tm.tmExternalLeading; // - запись в структуру высоты символа: высота плюс внешний промежуток символьной ячейки

  ReleaseDC(hwnd, hdc); // - освобождение описателя контекста устройства

  //Создание шрифта вывода текста, будет удален перед уничтожением окна

  SState->font = CreateFont(SState->yChar, SState->xChar, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Consolas");

  //Получение размеров рабочей области окна и сохранение значений в структуре

  GetClientRect(hwnd, &rect);
  SState->xClient = rect.right;
  SState->yClient = rect.bottom;

  //Сохранение адресов массивов для расположения и имени файла

  SState->flName = fileName;
  SState->ttlName = titleName;

  //Инициализация структуры OPENFILENAME

  FileInitialize(SState, hwnd);

  //Обработка аргументов строки

  Args(SState, hwnd, lParam);
}

//Функция обработки изменнения размера окна

void SIZE_FUNC(SysState* SState, HWND hwnd)
{
  RECT rect;

  //Получение и обновление размеров рабочей области окна в структуре

  GetClientRect(hwnd, &rect);
  SState->xClient = rect.right;
  SState->yClient = rect.bottom;

  //Обновление метрики вывода текста

  PrintMetrixUpd(SState, &rect);

  //Обновление данных скроллов

  vScrollUpd(SState, hwnd);
  hScrollUpd(SState, hwnd);
}

//Функция отрисовки рабочей области окна

void PAINT_FUNC(SysState* SState, HWND hwnd)
{
  HDC hdc;
  PAINTSTRUCT ps;
  int k = SState->prntMetrix.pntBeg; // - индекс, используемый для вывода текста на экран
  int index = 0;

  hdc = BeginPaint(hwnd, &ps); // - подготовка окна к перерисовке

  PrintMetrixUpd(SState, &ps.rcPaint); // - обновление метрики вывода текста

  SetBkMode(hdc, TRANSPARENT); // - установка фона текста прозрачным

  SelectObject(hdc, SState->font); // - выбор используемого шрифта

  //Логика вывода текста на экран

  switch (SState->vType)
  {
  case layoutV: // - для режима верстки
    for (int i = SState->prntMetrix.pntBeg; i < SState->prntMetrix.pntEnd; i++) // - вывод строк на экран
    {
      while (SState->strings[i].size - index >= SState->xClient / SState->xChar - 1) // - вывод частей строки, занимающих полную длинну экрана
      {
        int x = SState->xChar * (1 - SState->scrlMetrix.hScrollPos * SState->scrlMetrix.hScrollCoef);
        int y = SState->yChar * (k - SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef);
        TextOut(hdc, x, y, &SState->strings[i].data[index], SState->xClient / SState->xChar - 1);
        index += SState->xClient / SState->xChar - 1;
        k++;
      }
      int x = SState->xChar * (1 - SState->scrlMetrix.hScrollPos * SState->scrlMetrix.hScrollCoef); // - вывод остатка строки, не занимающего всю длинну экрана
      int y = SState->yChar * (k - SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef);
      TextOut(hdc, x, y, &SState->strings[i].data[index], SState->strings[i].size - index);
      index = 0;
      k++;
    }
    break;
  case defaultV: // - для режима по умолчанию
    for (int i = SState->prntMetrix.pntBeg; i < SState->prntMetrix.pntEnd; i++) // - вывод строк текста
    {
      int x = SState->xChar * (1 - SState->scrlMetrix.hScrollPos * SState->scrlMetrix.hScrollCoef);
      int y = SState->yChar * (i - SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef);
      TextOut(hdc, x, y, SState->strings[i].data, SState->strings[i].size);
    }
    break;
  }

  EndPaint(hwnd, &ps); // - окончание перерисовки окна
}

//Функция обработки сообщений взаимодействия пользователя с меню

void COMMAND_FUNC(SysState* SState, HWND hwnd, WPARAM wParam)
{
  HMENU hMenu = GetMenu(hwnd); // - получение описателя меню
  RECT rect;

  //Логика обработки сообщений

  switch (LOWORD(wParam))
  {
  case IDM_OPEN: // - обработка нажатия на кнопку "Open..."
    if (GetOpenFileName(&SState->ofn)) // - в случае успешного выбора файла пользователем
    {
      ReleaseMemory(SState->strings, SState->strNum); // - освобождается предыдущий хранимый текст
      ReadText(SState); // - считывается содержимое нового файла
      ChangeWndTitle(SState, hwnd); // - изменяется заголовок окна

      //Обновление данных

      GetClientRect(hwnd, &rect);
      PrintMetrixUpd(SState, &rect);
      InvalidateRect(hwnd, &rect, TRUE);
      ScrollsReset(SState, hwnd);
      SendMessage(hwnd, WM_SIZE, 0, 0L);
    }
    else
      MessageBeep(0); // - в противном случае подается звуковой сигнал пользователю
    break;
  case IDM_EXIT: // - обработка нажатия кнопки "Exit"
    SendMessage(hwnd, WM_DESTROY, 0, 0L); // - отправка сообщения окну об уничтожении
    break;
  case IDM_LAYOUT: // - обработка нажатия кнопки режима отображения
    switch (SState->vType)
    {
    case layoutV: // - в случае активного режима верстки
      CheckMenuItem(hMenu, IDM_LAYOUT, MF_UNCHECKED); // - "выключается" кнопка "Layout"
      SState->vType = defaultV; // - устанавливается соответствующий режим отображения

      //Обновление данных

      GetClientRect(hwnd, &rect);
      PrintMetrixUpd(SState, &rect);
      vScrollUpd(SState, hwnd);
      hScrollUpd(SState, hwnd);
      InvalidateRect(hwnd, &rect, TRUE);
      break;
    case defaultV: // - в случае активного режима по умолчанию
      CheckMenuItem(hMenu, IDM_LAYOUT, MF_CHECKED); // - "включается" кнопка "Layout"
      SState->vType = layoutV; // - устанавливается соответствующий режим отображения

      //Обновление данных

      GetClientRect(hwnd, &rect);
      PrintMetrixUpd(SState, &rect);
      vScrollUpd(SState, hwnd);
      hScrollUpd(SState, hwnd);
      InvalidateRect(hwnd, &rect, TRUE);
      break;
    }
    break;
  case IDM_ABOUT: // - обработка нажатия кнопки "About"
    MessageBox(hwnd, "     Text file reader\nby Nikita Vezdenetsky", "About", MB_ICONINFORMATION | MB_OK); // - вывод информационного окна
    break;
  }
}

//Обработка сообщений вертикальной полосы прокрутки

void VSCROLL_FUNC(SysState* SState, HWND hwnd, WPARAM wParam)
{
  RECT rect;
  int vScrollDiff;

  switch (LOWORD(wParam))
  {
  case SB_TOP: // - сдвиг в начало документа
    vScrollDiff = -1 * SState->scrlMetrix.vScrollPos;
    break;

  case SB_BOTTOM: // - сдвиг в конец документа
    vScrollDiff = SState->scrlMetrix.vScrollMax - SState->scrlMetrix.vScrollPos;
    break;

  case SB_LINEUP: // - нажатие на кнопку стрелки вверх
    vScrollDiff = -1;
    break;

  case SB_LINEDOWN: // - нажатие на кнопку стрелки вниз
    vScrollDiff = 1;
    break;

  case SB_PAGEUP: // - сдвиг на одну страницу вверх
    vScrollDiff = min(-1, -SState->yClient / SState->yChar);
    break;

  case SB_PAGEDOWN: // - сдвиг на одну страницу вниз
    vScrollDiff = max(1, SState->yClient / SState->yChar);
    break;

  case SB_THUMBTRACK: // - перемещение ползунка
    vScrollDiff = (HIWORD(wParam) - SState->scrlMetrix.vScrollPos);
    break;

  default:
    vScrollDiff = 0;
  }

  //Исключение варианта выхода значения за установленные пределы

  vScrollDiff = max(-SState->scrlMetrix.vScrollPos, min(vScrollDiff, SState->scrlMetrix.vScrollMax - SState->scrlMetrix.vScrollPos));

  if (vScrollDiff != 0 || LOWORD(wParam) == SB_THUMBPOSITION)
  {
    SState->scrlMetrix.vScrollPos += vScrollDiff; // - установка нового значения положения ползунка в структуре
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff, NULL, NULL); // - прокрутка окна
    SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE); // - установка нового значения ползунка в полосе прокрутки
    UpdateWindow(hwnd);

    //Обновление данных

    GetClientRect(hwnd, &rect);
    PrintMetrixUpd(SState, &rect);
    vScrollUpd(SState, hwnd);
    hScrollUpd(SState, hwnd);
    InvalidateRect(hwnd, &rect, TRUE);
  }
}

//Обработка сообщений горизонтальной полосы прокрутки

void HSCROLL_FUNC(SysState* SState, HWND hwnd, WPARAM wParam)
{
  int hScrollDiff;

  switch (LOWORD(wParam))
  {
  case SB_LINEUP: // - нажатие кнопки стрелки влево
    hScrollDiff = -1;
    break;

  case SB_LINEDOWN: // - нажатие кнопки стрелки вправо
    hScrollDiff = 1;
    break;

  case SB_PAGEUP: // - сдвиг на одну страницу влево
    hScrollDiff = -8;
    break;

  case SB_PAGEDOWN: // - сдвиг на одну страницу вправо
    hScrollDiff = 8;
    break;

  case SB_THUMBTRACK: // - перемещение ползунка
    hScrollDiff = HIWORD(wParam) - SState->scrlMetrix.hScrollPos;
    break;

  default:
    hScrollDiff = 0;
  }

  //Исключение выхода прироста значения положения ползунка за установленные границы

  hScrollDiff = max(-SState->scrlMetrix.hScrollPos, min(hScrollDiff, SState->scrlMetrix.hScrollMax - SState->scrlMetrix.hScrollPos));

  if (hScrollDiff != 0)
  {
    SState->scrlMetrix.hScrollPos += hScrollDiff; // - установка нового положения ползунка в структуре
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff, 0, NULL, NULL); // - прокрутка окна
    SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE); // - установка нового положения ползунка в строке прокрутки
    UpdateWindow(hwnd);
  }
}

//Обработка сообщений от колесика мыши и клавиатуры для вертикальной полосы прокрутки

void AUTOvSCROLL_FUNC(SysState* SState, HWND hwnd, int vScrollDiff)
{
  RECT rect;

  //Исключение выхода прироста значения положения ползунка за установленные границы

  vScrollDiff = max(-SState->scrlMetrix.vScrollPos, min(vScrollDiff, SState->scrlMetrix.vScrollMax - SState->scrlMetrix.vScrollPos));

  if (vScrollDiff != 0)
  {
    SState->scrlMetrix.vScrollPos += vScrollDiff; // - установка нового положения ползунка в структуре
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff, NULL, NULL);  // - прокрутка окна
    SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE); // - установка нового положения ползунка в строке прокрутки
    UpdateWindow(hwnd);

    //Обновление данных

    GetClientRect(hwnd, &rect);
    PrintMetrixUpd(SState, &rect);
    vScrollUpd(SState, hwnd);
    hScrollUpd(SState, hwnd);
    InvalidateRect(hwnd, &rect, TRUE);
  }
}

//Обработка сообщений от колесика мыши и клавиатуры для горизонтальной полосы прокрутки

void AUTOhSCROLL_FUNC(SysState* SState, HWND hwnd, int hScrollDiff)
{

  //Исключение выхода прироста значения положения ползунка за установленные границы

  hScrollDiff = max(-SState->scrlMetrix.hScrollPos, min(hScrollDiff, SState->scrlMetrix.hScrollMax - SState->scrlMetrix.hScrollPos));

  if (hScrollDiff != 0)
  {
    SState->scrlMetrix.hScrollPos += hScrollDiff; // - установка нового положения ползунка в структуре
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff, 0, NULL, NULL);  // - прокрутка окна
    SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE); // - установка нового положения ползунка в строке прокрутки
    UpdateWindow(hwnd);
  }
}

//Функция проведения операций перед уничтожением окна

void DESTROY_FUNC(SysState* SState)
{
  ReleaseMemory(SState->strings, SState->strNum); // - освобождение прочитанных данных текста
  DeleteObject(SState->font); // - удаление шрифта
  PostQuitMessage(0);
}

#include "lab_1.h"
#include "file.h"
#include "funcs.h"
#include "scrolls.h"

//Функция создания окна

void CREATE_FUNC(SystemState* SState, HWND hwnd, LPARAM lParam)
{
  HMENU hMenu;
  TEXTMETRIC tm;
  HDC hdc;
  static char filePath[_MAX_PATH], fileTitle[_MAX_FNAME + _MAX_EXT]; // - статические массивы для хранения расположения и названия файла

  SState->vType = defaultV; // - установка режима отображения текста по умолчанию
  SState->filePath = filePath;  // - сохранение адреса массива для пути к файлу
  SState->fileTitle = fileTitle;  // - сохранение адреса массива для полного названия файла

  //Инициализация структуры OPENFILENAME

  OFNInitialize(SState, hwnd);

  //Обнуление метрики парсера

  SState->defParser.stringBeg = NULL;
  SState->defParser.size = 0;
  SState->layParser.stringBeg = NULL;
  SState->layParser.size = 0;

  hdc = GetDC(hwnd);  // - получение описателя контекста устройства
  GetTextMetrics(hdc, &tm); // - получение размера текстовых символов на экране
  SState->xChar = tm.tmAveCharWidth; // - запись в структуру ширины символа
  SState->yChar = tm.tmHeight + tm.tmExternalLeading; // - запись в структуру высоты символа: высота плюс внешний промежуток символьной ячейки

  //Создание шрифта вывода текста, будет удален перед уничтожением окна

  SState->font = CreateFontA(SState->yChar, SState->xChar, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Consolas");
  SetBkMode(hdc, TRANSPARENT); // - установка фона текста прозрачным
  SelectObject(hdc, SState->font); // - установка фона

  ReleaseDC(hwnd, hdc); // - освобождение описателя контекста устройства

  hMenu = GetMenu(hwnd); // - получение описателя меню
  CheckMenuItem(hMenu, IDM_LAYOUT, MF_UNCHECKED); // - "выключение" кнопки режима верстки

  //Обработка аргументов строки

  ArgumentsProcessing(SState, hwnd, lParam);
}

//Функция обработки изменнения размера окна

void SIZE_FUNC(SystemState* SState, HWND hwnd, LPARAM lParam)
{
  //Получение и обновление размеров рабочей области окна в структуре

  SState->xClient = LOWORD(lParam);
  SState->yClient = HIWORD(lParam);

  //Обновление данных начала строк

  TextResizer(SState, hwnd);

  //Обновление данных скроллов

  vScrollUpd(SState, hwnd);
  hScrollUpd(SState, hwnd);

  //Обновление метрики вывода текста

  PrintMetrixUpd(SState);
}

//Функция отрисовки рабочей области окна

void PAINT_FUNC(SystemState* SState, HWND hwnd)
{
  HDC hdc;
  PAINTSTRUCT ps;
  ParserMetrix* parser = NULL;
  int i;
  hdc = BeginPaint(hwnd, &ps);  // - начало перерисовки окна

  //Выбор соответствующего массива начал строк

  switch (SState->vType)
  {
  case layoutV: parser = &SState->layParser; break;
  case defaultV: parser = &SState->defParser; break;
  }

  //Вывод строк текста

  for (i = SState->printMetrix.beg; i < SState->printMetrix.end; i++)
  {
    int x = SState->xChar * (1 - SState->hScroll.pos * SState->hScroll.coef);
    int y = SState->yChar * (i - SState->vScroll.pos * SState->vScroll.coef);
    TextOutA(hdc, x, y, &SState->text.data[parser->stringBeg[i].begIndex], i == (parser->size - 1) ? (SState->text.size - parser->stringBeg[i].begIndex) : (parser->stringBeg[i + 1].begIndex - parser->stringBeg[i].begIndex));
  }

  EndPaint(hwnd, &ps); // - окончание перерисовки окна
}

//Функция обработки сообщений взаимодействия пользователя с меню

void COMMAND_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam)
{
  HMENU hMenu = GetMenu(hwnd); // - получение описателя меню

  //Логика обработки сообщений

  switch (LOWORD(wParam))
  {
  case IDM_OPEN: // - обработка нажатия на кнопку "Open..."
    if (GetOpenFileNameA(&SState->ofn)) // - в случае успешного выбора файла пользователем
    {
      OpenFileProcessing(SState, hwnd); // - чтение и обработка данных из файла
      InvalidateRect(hwnd, NULL, TRUE); // - перерисовыем всю область окна
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

      ToDefault(SState, hwnd);  // - синхронизация выводимых строк
      vScrollUpd(SState, hwnd); // - обновление данных вертикальной строки прокрутки
      hScrollUpd(SState, hwnd); // - обновление данных горизонтальной строки прокрутки
      PrintMetrixUpd(SState); // - обновсление данных вывода текста
      InvalidateRect(hwnd, NULL, TRUE); // - перерисовыем всю область окна
      break;
    case defaultV: // - в случае активного режима по умолчанию
      CheckMenuItem(hMenu, IDM_LAYOUT, MF_CHECKED); // - "включается" кнопка "Layout"
      SState->vType = layoutV; // - устанавливается соответствующий режим отображения

      //Обновление данных

      vScrollUpd(SState, hwnd); // - обновление данных вертикальной строки прокрутки
      hScrollUpd(SState, hwnd); // - обновление данных горизонтальной строки прокрутки
      ToLayout(SState, hwnd); // - синхронизация выводимых строк
      PrintMetrixUpd(SState); // - обновсление данных вывода текста
      InvalidateRect(hwnd, NULL, TRUE); // - перерисовыем всю область окна
      break;
    }
    break;
  case IDM_ABOUT: // - обработка нажатия кнопки "About"
    MessageBoxA(hwnd, "     Text file reader\nby Nikita Vezdenetsky", "About", MB_ICONINFORMATION | MB_OK); // - вывод информационного окна
    break;
  }
}

//Обработка сообщений вертикальной полосы прокрутки

void VSCROLL_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam)
{
  int vScrollDiff;

  switch (LOWORD(wParam))
  {
  case SB_TOP: // - сдвиг в начало документа
    vScrollDiff = -1 * SState->vScroll.pos;
    break;

  case SB_BOTTOM: // - сдвиг в конец документа
    vScrollDiff = SState->vScroll.max - SState->vScroll.pos;
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
    vScrollDiff = (HIWORD(wParam) - SState->vScroll.pos);
    break;

  default:
    vScrollDiff = 0;
  }

  //Исключение варианта выхода значения за установленные пределы

  vScrollDiff = max(-SState->vScroll.pos, min(vScrollDiff, SState->vScroll.max - SState->vScroll.pos));

  if (vScrollDiff != 0 || LOWORD(wParam) == SB_THUMBPOSITION)
  {
    SState->vScroll.pos += vScrollDiff; // - установка нового значения положения ползунка в структуре
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff * SState->vScroll.coef, NULL, NULL); // - прокрутка окна
    SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE); // - установка нового значения ползунка в полосе прокрутки
    PrintMetrixUpd(SState); // - обновсление данных вывода текста
    UpdateWindow(hwnd); // - обновление окна
  }
}

//Обработка сообщений горизонтальной полосы прокрутки

void HSCROLL_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam)
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
    hScrollDiff = HIWORD(wParam) - SState->hScroll.pos;
    break;

  default:
    hScrollDiff = 0;
  }

  //Исключение выхода прироста значения положения ползунка за установленные границы

  hScrollDiff = max(-SState->hScroll.pos, min(hScrollDiff, SState->hScroll.max - SState->hScroll.pos));

  if (hScrollDiff != 0)
  {
    SState->hScroll.pos += hScrollDiff; // - установка нового положения ползунка в структуре
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff * SState->hScroll.coef, 0, NULL, NULL); // - прокрутка окна
    SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE); // - установка нового положения ползунка в строке прокрутки
    UpdateWindow(hwnd); // - обновление окна
  }
}

//Обработка сообщений от колесика мыши и клавиатуры для вертикальной полосы прокрутки

void AUTOvSCROLL_FUNC(SystemState* SState, HWND hwnd, int vScrollDiff)
{
  //Исключение выхода прироста значения положения ползунка за установленные границы

  vScrollDiff = max(-SState->vScroll.pos, min(vScrollDiff, SState->vScroll.max - SState->vScroll.pos));

  if (vScrollDiff != 0)
  {
    SState->vScroll.pos += vScrollDiff; // - установка нового значения положения ползунка в структуре
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff * SState->vScroll.coef, NULL, NULL); // - прокрутка окна
    SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE); // - установка нового значения ползунка в полосе прокрутки
    PrintMetrixUpd(SState); // - обновсление данных вывода текста
    UpdateWindow(hwnd); // - обновление окна
  }
}

//Обработка сообщений от колесика мыши и клавиатуры для горизонтальной полосы прокрутки

void AUTOhSCROLL_FUNC(SystemState* SState, HWND hwnd, int hScrollDiff)
{

  //Исключение выхода прироста значения положения ползунка за установленные границы

  hScrollDiff = max(-SState->hScroll.pos, min(hScrollDiff, SState->hScroll.max - SState->hScroll.pos));

  if (hScrollDiff != 0)
  {
    SState->hScroll.pos += hScrollDiff; // - установка нового положения ползунка в структуре
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff * SState->hScroll.coef, 0, NULL, NULL); // - прокрутка окна
    SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE); // - установка нового положения ползунка в строке прокрутки
    UpdateWindow(hwnd); // - обновление окна
  }
}

//Функция проведения операций перед уничтожением окна

void DESTROY_FUNC(SystemState* SState)
{
  DeleteObject(SState->font); // - удаление шрифта
  if (SState->text.data != NULL) // - освобождение памяти для текста
    free(SState->text.data);
  if (SState->defParser.stringBeg != NULL)  // - освобождение памяти массива начал строк для режима отображения по умолчанию
    free(SState->defParser.stringBeg);
  if (SState->layParser.stringBeg != NULL)  // - освобождение памяти массива начал строк для режима верстки
    free(SState->layParser.stringBeg);
  PostQuitMessage(0);
}

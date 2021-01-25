#include "lab_1.h"
#include "file.h"

//Функция считывания текста из файла

int ReadText(SystemState* SState)
{
  FILE* file = NULL;
  if (fopen_s(&file, SState->ofn.lpstrFile, "rb"))  // - открытие файла на чтение
  {
    MessageBeep(0);
    return 0;
  }

  fseek(file, 0, SEEK_END); // - переход в конец файла
  SState->text.size = ftell(file) / sizeof(char); // - установка размера файла
  if (SState->text.size == -1L)
  {
    fclose(file);
    MessageBeep(0);
    return 0;
  }
  fseek(file, 0, SEEK_SET); // - переход в начало файла

  SState->text.size++;
  SState->text.data = malloc(sizeof(char) * SState->text.size); // - выделение памяти для текста
  if (SState->text.data == NULL)
  {
    fclose(file);
    MessageBeep(0);
    return 0;
  }

  int count = fread(SState->text.data, sizeof(char), SState->text.size, file);  // - чтение текста из файла
  SState->text.data[count] = '\0';
  fclose(file);
  return 1;
}

//Установка начала строк для режимов по умолчанию и верстки

int PrimaryParser(SystemState* SState) 
{
  //Выделение куска памяти под массив структур начала строк и заполнение начальных данных

  SState->defParser.stringBeg = (strBeg*)malloc(Buffer * sizeof(strBeg));
  if (SState->defParser.stringBeg == NULL)
    return 0;
  SState->defParser.stringBeg[0].begIndex = 0;
  SState->defParser.stringBeg[0].type = defaultV;
  SState->defParser.size = 1;
  SState->defParser.arraySize = Buffer;

  SState->layParser.stringBeg = (strBeg*)malloc(Buffer * sizeof(strBeg));
  if (SState->layParser.stringBeg == NULL)
    return 0;
  SState->layParser.stringBeg[0].begIndex = 0;
  SState->layParser.stringBeg[0].begIndex = defaultV;
  SState->layParser.size = 1;
  SState->layParser.arraySize = Buffer;

  //Сам процесс нахождения начала строк

  int stringLength = 0;
  int maxLen = 0;
  SState->printMetrix.maxLen = 0;
  for (int i = 0; i < SState->text.size; i++)
  {
    stringLength++;
    maxLen++;
    if (stringLength >= SState->xClient / SState->xChar - 1)  // - перенесенная строка в режиме верстки
    {
      if (SState->layParser.size % Buffer == 0)
      {
        strBeg* temp = (strBeg*)realloc(SState->layParser.stringBeg, (SState->layParser.size / Buffer + 1) * Buffer * sizeof(strBeg));
        if (temp == NULL)
          return 0;
        SState->layParser.stringBeg = temp;
        SState->layParser.arraySize = (SState->layParser.size / Buffer + 1) * Buffer;
      }
      SState->layParser.stringBeg[SState->layParser.size].begIndex = i + 1;
      SState->layParser.stringBeg[SState->layParser.size].type = layoutV;
      SState->layParser.size++;
      stringLength = 0;
    }
    if (SState->text.data[i] == '\n') // - конец строки
    {
      if (SState->defParser.size % Buffer == 0) // - при необходимости, выделяем новый кусок памяти
      {
        strBeg* temp = (strBeg*)realloc(SState->defParser.stringBeg, (SState->defParser.size / Buffer + 1) * Buffer * sizeof(strBeg));
        if (temp == NULL)
          return 0;
        SState->defParser.stringBeg = temp;
        SState->defParser.arraySize = (SState->defParser.size / Buffer + 1) * Buffer;
      }
      SState->defParser.stringBeg[SState->defParser.size].begIndex = i + 1;
      SState->defParser.stringBeg[SState->defParser.size].type = defaultV;
      SState->defParser.size++;

      if (SState->layParser.size % Buffer == 0) // - при необходимости, выделяем новый кусок памяти
      {
        strBeg* temp = (strBeg*)realloc(SState->layParser.stringBeg, (SState->layParser.size / Buffer + 1) * Buffer * sizeof(strBeg));
        if (temp == NULL)
          return 0;
        SState->layParser.stringBeg = temp;
        SState->layParser.arraySize = (SState->layParser.size / Buffer + 1) * Buffer;
      }
      SState->layParser.stringBeg[SState->layParser.size].begIndex = i + 1;
      SState->layParser.stringBeg[SState->layParser.size].type = defaultV;
      SState->layParser.size++;
      if (maxLen > SState->printMetrix.maxLen)  // - нахождение строки, максимальной длины для горизонтальной прокрутки
        SState->printMetrix.maxLen = maxLen;
      maxLen = 0;
      stringLength = 0;
    }
  }
  return 1;
}

//Обновление начала строк для режима верстки при изменении размера экрана

int SecondaryParser(SystemState* SState) {
  int stringLength = 0;
  int ind = SState->printMetrix.beg;
  char* beg = &SState->text.data[SState->layParser.stringBeg[SState->printMetrix.beg].begIndex];

  //Запись новых данных происходит поверх старых

  SState->layParser.stringBeg[0].begIndex = 0;
  SState->layParser.stringBeg[0].begIndex = defaultV;
  SState->layParser.size = 1;
  for (int i = 0; i < SState->text.size; i++)
  {
    stringLength++;
    if (stringLength >= SState->xClient / SState->xChar - 1)  // - перенесенная строка в режиме верстки
    {
      if (SState->layParser.size == SState->layParser.arraySize)  // - при необходимости, выделяем новый кусок памяти
      {
        strBeg* temp = (strBeg*)realloc(SState->layParser.stringBeg, (SState->layParser.size / Buffer + 1) * Buffer * sizeof(strBeg));
        if (temp == NULL)
          return 0;
        SState->layParser.stringBeg = temp;
        SState->layParser.arraySize = (SState->layParser.size / Buffer + 1) * Buffer;
      }
      SState->layParser.stringBeg[SState->layParser.size].begIndex = i + 1;
      SState->layParser.stringBeg[SState->layParser.size].type = layoutV;
      SState->layParser.size++;
      stringLength = 0;
      if (&SState->text.data[SState->layParser.stringBeg[SState->layParser.size - 1].begIndex] == beg)  // - нахождение первой выводимой строки при старых данных верстки
        ind = SState->layParser.size - 1;
    }
    if (SState->text.data[i] == '\n') // - конец строки
    {
      if (SState->layParser.size == SState->layParser.arraySize)  // - при необходимости, выделяем новый кусок памяти
      {
        strBeg* temp = (strBeg*)realloc(SState->layParser.stringBeg, (SState->layParser.size / Buffer + 1) * Buffer * sizeof(strBeg));
        if (temp == NULL)
          return 0;
        SState->layParser.stringBeg = temp;
        SState->layParser.arraySize = (SState->layParser.size / Buffer + 1) * Buffer;
      }
      SState->layParser.stringBeg[SState->layParser.size].begIndex = i + 1;
      SState->layParser.stringBeg[SState->layParser.size].type = defaultV;
      SState->layParser.size++;
      stringLength = 0;
      if (&SState->text.data[SState->layParser.stringBeg[SState->layParser.size - 1].begIndex] == beg)  // - нахождение первой выводимой строки при старых данных верстки
        ind = SState->layParser.size - 1;
    }
  }
  if (SState->vType == layoutV) // - установка нового положения ползунка для синхронизации выводимых строк
    SState->vScroll.pos = min(ind / SState->vScroll.coef, SState->vScroll.max);
  return 1;
}

//Функция инициализации структуры OPENFILENAME

void OFNInitialize(SystemState* SState, HWND hwnd)
{
  static char szFilter[] = "Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
  ZeroMemory(&SState->ofn, sizeof(OPENFILENAMEA));
  SState->ofn.lStructSize = sizeof(OPENFILENAMEA);
  SState->ofn.hwndOwner = hwnd;
  SState->ofn.hInstance = NULL;
  SState->ofn.lpstrFilter = szFilter;
  SState->ofn.lpstrCustomFilter = NULL;
  SState->ofn.nMaxCustFilter = 0;
  SState->ofn.nFilterIndex = 0;
  SState->ofn.lpstrFile = SState->filePath;
  SState->ofn.nMaxFile = _MAX_PATH;
  SState->ofn.lpstrFileTitle = SState->fileTitle;
  SState->ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
  SState->ofn.lpstrInitialDir = NULL;
  SState->ofn.lpstrTitle = NULL;
  SState->ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  SState->ofn.nFileOffset = 0;
  SState->ofn.nFileExtension = 0;
  SState->ofn.lpstrDefExt = "txt";
  SState->ofn.lCustData = 0L;
  SState->ofn.lpfnHook = NULL;
  SState->ofn.lpTemplateName = NULL;
}

//Функция, меняющая заголовок окна

void ChangeWndTitle(SystemState* SState, HWND hwnd)
{
  const char postfix[] = " - Reader"; // - постфикс
  int size = strlen(SState->ofn.lpstrFileTitle) + strlen(postfix) + 1;  // - необходимый размер памяти для буфера
  char* title = malloc(size * sizeof(char));  // - выделение памяти для буфера
  if (!title) // - в случае неудачного выделения заголовок останется прежний, а пользователь будет уведомлен об этом звуковым сигналом
  {
    MessageBeep(0);
    return;
  }

  //Заполнение буфера

  strcpy_s(title, size, SState->ofn.lpstrFileTitle);
  strcat_s(title, size, postfix);
  SetWindowTextA(hwnd, title);  // - установка нового заголовка
  free(title);  // - освобождение буфера
}

//Функция считывания и обработки входного файла

void OpenFileProcessing(SystemState* SState, HWND hwnd)
{
  //Резервные копии данных старого выводимого текста
  Text backUpText = SState->text;
  ParserMetrix defBackUp = SState->defParser, layBackUp = SState->layParser;

  if (!ReadText(SState)) // - считывается содержимое нового файла
  {
    SState->text = backUpText;  // - восстановление данных старого текста, в случае ошибки
    SState->defParser = defBackUp;
    SState->layParser = layBackUp;
    return;
  }
  if (!PrimaryParser(SState)) // - парсинг текста
  {
    MessageBeep(0); // - освобождение ненужных данных и восстановление данных старого текста, в случае ошибки
    free(SState->text.data);
    if (SState->defParser.stringBeg != NULL)
      free(SState->defParser.stringBeg);
    if (SState->layParser.stringBeg != NULL)
      free(SState->layParser.stringBeg);
    SState->text = backUpText;
    SState->defParser = defBackUp;
    SState->layParser = layBackUp;
    return;
  }
  free(backUpText.data);  // - удаление резервной копии при успешном считывании и парсинге файла
  free(defBackUp.stringBeg);
  free(layBackUp.stringBeg);

  ChangeWndTitle(SState, hwnd); // - изменяется заголовок окна
  ScrollsReset(SState, hwnd); // - сброс данных ползунков
  PrintMetrixUpd(SState); // - обновление данных вывода текста
}

//Функция, обрабатывающая аргументы строки

void ArgumentsProcessing(SystemState* SState, HWND hwnd, LPARAM lParam)
{
  //Получение аргументов командной строки

  CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
  RECT rect;
  char* temp = cs->lpCreateParams;
  if (!strcmp(temp, ""))  // - выход из ф-ии в случае отсутствия аргументов
    return;

  //Заполнение соответствующих массивов

  strcpy_s(SState->filePath, _MAX_PATH, temp);
  strcpy_s(SState->fileTitle, _MAX_FNAME + _MAX_EXT, temp);

  //Получение размеров рабочей области окна и сохранение значений в структуре

  GetClientRect(hwnd, &rect);
  SState->xClient = rect.right;
  SState->yClient = rect.bottom;

  //Работа с входным файлом

  OpenFileProcessing(SState, hwnd);
}

//Функция пересчета начала строк в случае изменения размеров окна

void TextResizer(SystemState* SState, HWND hwnd)
{
  if (SState->text.data == NULL)
    return;
  if (!SecondaryParser(SState))
    MessageBeep(0);
}
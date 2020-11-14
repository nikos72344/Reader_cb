#include "lab_1.h"
#include "file.h"

//������� ������������ ������

void ReleaseMemory(String* Strings, unsigned int StrNum)
{
  for (unsigned int i = 0; i < StrNum; i++)
    free(Strings[i].data);
  free(Strings);
}

//������� ��������� ������ ��� ����������

void ReadError(SysState* SState)
{
  SState->strings = NULL;
  SState->strNum = 0;
  SState->sumNum = 0;
  MessageBeep(0);
}

//������� ���������� ������ �� �����

void ReadText(SysState* SState)
{
    FILE* file = fopen(SState->ofn.lpstrFile, "r"); // - �������� ����� �� ������
  if (!file)
  {
    ReadError(SState);
    return;
  }

  String* strings = (String*)malloc(sizeof(String));  // - ��������� ������ ��� ������ �����
  if (strings == NULL)
  {
    fclose(file);
    ReadError(SState);
    return;
  }
  strings[0].data = (char*)malloc(szBuffer * sizeof(char));  // - ��������� ������ ��� ������ ����������� ������
  if (strings[0].data == NULL)
  {
    free(strings);
    fclose(file);
    ReadError(SState);
    return;
  }
  int i = 0, j = 0, flag = 0;
  while (1)
  {
    flag = fscanf(file, "%c", &strings[j].data[i]);  // - ���������� ����������� �� �����
    if (strings[j].data[i] == '\n')  // - ��������� ����� ������
    {
      strings[j].data[i] = '\0';
      strings[j].size = i;
      j++;
      i = -1;
      String* temp = (String*)realloc(strings, (j + 1) * sizeof(String));
      if (temp == NULL)
      {
        ReleaseMemory(strings, j);
        fclose(file);
        ReadError(SState);
        return;
      }
      strings = temp;
      strings[j].data = (char*)malloc(szBuffer * sizeof(char));
      if (strings[j].data == NULL)
      {
        ReleaseMemory(strings, j);
        fclose(file);
        ReadError(SState);
        return;
      }
    }
    i++;
    if (i % szBuffer == 0)  // - ��������� ���������� ������
    {
      char* temp = (char*)realloc(strings[j].data, (i + szBuffer) * sizeof(char));
      if (temp == NULL)
      {
        ReleaseMemory(strings, j);
        fclose(file);
        ReadError(SState);
        return;
      }
      strings[j].data = temp;
    }
    if (flag == EOF)  // - ��������� ���������� ����� �����
    {
      strings[j].data[i - 1] = '\0';
      strings[j].size = i - 1;
      break;
    }
  }
  fclose(file);  // - �������� �����

  //���������� ��������������� �������� ���������

  SState->strings = strings;
  SState->strNum = j + 1;
  SState->sumNum = j + 1;
}

//������� ������������� ��������� OPENFILENAME

void FileInitialize(SysState* SState, HWND hwnd)
{
  static char szFilter[] = "Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
  ZeroMemory(&SState->ofn, sizeof(OPENFILENAME));
  SState->ofn.lStructSize = sizeof(OPENFILENAME);
  SState->ofn.hwndOwner = hwnd;
  SState->ofn.hInstance = NULL;
  SState->ofn.lpstrFilter = szFilter;
  SState->ofn.lpstrCustomFilter = NULL;
  SState->ofn.nMaxCustFilter = 0;
  SState->ofn.nFilterIndex = 0;
  SState->ofn.lpstrFile = SState->flName;
  SState->ofn.nMaxFile = _MAX_PATH;
  SState->ofn.lpstrFileTitle = SState->ttlName;
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

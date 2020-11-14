#include "lab_1.h"
#include"scrolls.h"
#include "file.h"

//������� ���������� ������ ������ ������

void PrintMetrixUpd(SysState* SState, RECT* RCT)
{
  int width;

  //���������� ������� ��������� ��������� ������ �� ������� ������� ������

  int fin = min(SState->strNum, SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef + RCT->bottom / SState->yChar);

  //���������� ������� ������ ��������� ������ �� �����

  SState->prntMetrix.pntBeg = max(0, SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef + RCT->top / SState->yChar);

  //���������� ������� ��������� ��������� �� ����� ������

  SState->prntMetrix.pntEnd = min(SState->strNum, SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef + 2 * RCT->bottom / SState->yChar);

  //� ��������� ��������� �������������� ������ ��� ���������� �������� ��� �������������

  SState->prntMetrix.maxLen = 0; // - ��������� ���������� ������� ������ ������������ ������
  SState->sumNum = SState->strNum; // - ����� ���������� ����� � ������ ������� ��� ������ �������
  for (int i = SState->prntMetrix.pntBeg; i < SState->prntMetrix.pntEnd; i++)
  {
    width = SState->xChar * (SState->strings[i].size + 1) / SState->xClient;
    SState->sumNum += width; // - ���������� ����������� ������������ ����� � ������ �������
    if (i < fin && SState->prntMetrix.maxLen < ((SState->strings[i].size + 1) * SState->xChar))
      SState->prntMetrix.maxLen = (SState->strings[i].size + 1) * SState->xChar; // - ���������� �������� ����� ������� ������, ��������� �� �����
  }
}

//������� ���������� �������� ������������ ������ ���������

void vScrollUpd(SysState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - � ������ ������ ������� ����������� � ������������ �������� ������������� ������ �� ���������� ����� � ������ ������� �����������
    SState->scrlMetrix.vScrollCoef = SState->sumNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->sumNum / SState->scrlMetrix.vScrollCoef - 1);
    break;
  case defaultV: // - � ������ ������ �� ��������� ����������� � ������������ �������� ������������� ������ �� ���������� ����� ��� ����� ������� �����������
    SState->scrlMetrix.vScrollCoef = SState->strNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->strNum / SState->scrlMetrix.vScrollCoef - 1);
    break;
  }

  //�������������� ������ �������� ��������� �������� �� ������������� �������

  SState->scrlMetrix.vScrollPos = min(SState->scrlMetrix.vScrollPos, SState->scrlMetrix.vScrollMax);

  //��������� ��������� � ��������� ��������

  SetScrollRange(hwnd, SB_VERT, 0, SState->scrlMetrix.vScrollMax, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE);
}

//������� ���������� �������� �������������� ������ ���������

void hScrollUpd(SysState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - � ������ ������ ������� �������������� ������ ��������� "�����������"
    SState->scrlMetrix.hScrollMax = 0;
    SState->scrlMetrix.hScrollPos = 0;
    SState->scrlMetrix.hScrollCoef = 0;
    break;
  case defaultV:  // - � ������ ������ �� ��������� �����������, ������������ �������� � ��������� �������� ������������� ������ �� ������������ ����� ������������ ������
    SState->scrlMetrix.hScrollCoef = SState->prntMetrix.maxLen / ScrollSize + 1;
    SState->scrlMetrix.hScrollMax = max(0, (SState->prntMetrix.maxLen / SState->scrlMetrix.hScrollCoef - SState->xClient + SState->xChar) / SState->xChar);
    SState->scrlMetrix.hScrollPos = min(SState->scrlMetrix.hScrollPos, SState->scrlMetrix.hScrollMax);
    break;
  }

  //��������� ��������� � ��������� ��������

  SetScrollRange(hwnd, SB_HORZ, 0, SState->scrlMetrix.hScrollMax, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE);
}

//������� ������������ ��������� �������������� � ������������ ����� ���������

void ScrollsReset(SysState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - � ������ ������ �������

    //������������ ������ ��������� ������������� � ������ ���������� ����� � ������ ������� �����������

    SState->scrlMetrix.vScrollCoef = SState->sumNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->sumNum / SState->scrlMetrix.vScrollCoef - 1);

    //�������������� ������ "�����������"

    SState->scrlMetrix.hScrollMax = 0;
    SState->scrlMetrix.hScrollCoef = 0;
    break;
  case defaultV: // - � ������ ������ �� ���������

    //������������ ������ ��������� ������������� � ������ ���������� ����� ��� ����� ������� �����������

    SState->scrlMetrix.vScrollCoef = SState->strNum / ScrollSize + 1;
    SState->scrlMetrix.vScrollMax = max(0, SState->strNum / SState->scrlMetrix.vScrollCoef - 1);

    //�������������� ������ ��������� ������������� � ������ ������������ ����� ������������ ������

    SState->scrlMetrix.hScrollCoef = SState->prntMetrix.maxLen / ScrollSize + 1;
    SState->scrlMetrix.hScrollMax = max(0, (SState->prntMetrix.maxLen / SState->scrlMetrix.hScrollCoef - SState->xClient + SState->xChar) / SState->xChar);
    break;
  }

  //����� ������� ���������

  SState->scrlMetrix.vScrollPos = 0;
  SState->scrlMetrix.hScrollPos = 0;

  //��������� ��������������� ���������� � ��������� ���������

  SetScrollRange(hwnd, SB_VERT, 0, SState->scrlMetrix.vScrollMax, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE);

  SetScrollRange(hwnd, SB_HORZ, 0, SState->scrlMetrix.hScrollMax, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE);
}

//�������, �������� ��������� ����

void ChangeWndTitle(SysState* SState, HWND hwnd)
{
  const char postfix[] = " - Reader"; // - ��������
  int size = strlen(SState->ofn.lpstrFileTitle) + strlen(postfix) + 1;
  char* title = malloc(size * sizeof(char));  // - ��������� ������ ��� ������
  if (!title) // - � ������ ���������� ��������� ��������� ��������� �������, � ������������ ����� ��������� �� ���� �������� ��������
  {
    MessageBeep(0);
    return;
  }

  //���������� ������

  strcpy(title, SState->ofn.lpstrFileTitle);
  strcat(title, postfix);
  SetWindowText(hwnd, title);  // - ��������� ������ ���������
  free(title);  // - ������������ ������
}

//�������, �������������� ��������� ������

void Args(SysState* SState, HWND hwnd, LPARAM lParam)
{
  //��������� ���������� ��������� ������

  CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
  RECT rect;
  char* temp = cs->lpCreateParams;
  if (!strcmp(temp, ""))  // - ����� �� �-�� � ������ ���������� ����������
    return;

  //���������� ��������������� ��������

  for (int i = 0; i <= (int)strlen(temp); i++)
  {
    SState->flName[i] = temp[i];
    SState->ttlName[i] = temp[i];
  }
  ReadText(SState);  // - ������ ������ �� �����

  //���������� ������

  GetClientRect(hwnd, &rect);
  PrintMetrixUpd(SState, &rect);
  vScrollUpd(SState, hwnd);
  hScrollUpd(SState, hwnd);
  ChangeWndTitle(SState, hwnd);
}

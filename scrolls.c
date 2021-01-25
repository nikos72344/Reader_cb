#include "lab_1.h"
#include"scrolls.h"
#include "file.h"

//������� ���������� ������ ������ ������

void PrintMetrixUpd(SystemState* SState)
{
  int stringSize;
  switch (SState->vType)
  {
  case layoutV: stringSize = SState->layParser.size; break;
  case defaultV: stringSize = SState->defParser.size; break;
  }

  //���������� ������� ������ ��������� ������ �� �����

  SState->printMetrix.beg = max(0, SState->vScroll.pos * SState->vScroll.coef);

  //���������� ������� ��������� ��������� �� ����� ������
  //� ��������� ��������� �������������� ������ ��� ���������� �������� ��� �������������

  SState->printMetrix.end = min(stringSize, SState->vScroll.pos * SState->vScroll.coef + 2 * SState->yClient / SState->yChar);

}

//������� ���������� �������� ������������ ������ ���������

void vScrollUpd(SystemState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - � ������ ������ ������� ����������� � ������������ �������� ������������� ������ �� ���������� ����� � ������ ������� �����������
    SState->vScroll.coef = SState->layParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, SState->layParser.size / SState->vScroll.coef - SState->yClient / SState->yChar);
    break;
  case defaultV: // - � ������ ������ �� ��������� ����������� � ������������ �������� ������������� ������ �� ���������� ����� ��� ����� ������� �����������
    SState->vScroll.coef = SState->defParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, SState->defParser.size / SState->vScroll.coef - SState->yClient / SState->yChar);
    break;
  }

  //�������������� ������ �������� ��������� �������� �� ������������� �������

  SState->vScroll.pos = min(SState->vScroll.pos, SState->vScroll.max);

  //��������� ��������� � ��������� ��������

  SetScrollRange(hwnd, SB_VERT, 0, SState->vScroll.max, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE);
}

//������� ���������� �������� �������������� ������ ���������

void hScrollUpd(SystemState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - � ������ ������ ������� �������������� ������ ��������� "�����������"
    SState->hScroll.max = 0;
    SState->hScroll.pos = 0;
    SState->hScroll.coef = 0;
    break;
  case defaultV:  // - � ������ ������ �� ��������� �����������, ������������ �������� � ��������� �������� ������������� ������ �� ������������ ����� ������������ ������
    SState->hScroll.coef = SState->printMetrix.maxLen / ScrollSize + 1;
    SState->hScroll.max = max(0, SState->printMetrix.maxLen / SState->hScroll.coef - (SState->xClient + SState->xChar) / SState->xChar);
    SState->hScroll.pos = min(SState->hScroll.pos, SState->hScroll.max);
    break;
  }

  //��������� ��������� � ��������� ��������

  SetScrollRange(hwnd, SB_HORZ, 0, SState->hScroll.max, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE);
}

//������� ������������ ��������� �������������� � ������������ ����� ���������

void ScrollsReset(SystemState* SState, HWND hwnd)
{
  switch (SState->vType)
  {
  case layoutV: // - � ������ ������ �������

    //������������ ������ ��������� ������������� � ������ ���������� ����� � ������ ������� �����������

    SState->vScroll.coef = SState->layParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, SState->layParser.size / SState->vScroll.coef - SState->yClient / SState->yChar);

    //�������������� ������ "�����������"

    SState->hScroll.max = 0;
    SState->hScroll.coef = 0;
    break;
  case defaultV: // - � ������ ������ �� ���������

    //������������ ������ ��������� ������������� � ������ ���������� ����� ��� ����� ������� �����������

    SState->vScroll.coef = SState->defParser.size / ScrollSize + 1;
    SState->vScroll.max = max(0, SState->defParser.size / SState->vScroll.coef - SState->yClient / SState->yChar);

    //�������������� ������ ��������� ������������� � ������ ������������ ����� ������������ ������

    SState->hScroll.coef = SState->printMetrix.maxLen / ScrollSize + 1;
    SState->hScroll.max = max(0, SState->printMetrix.maxLen / SState->hScroll.coef - (SState->xClient + SState->xChar) / SState->xChar);
    break;
  }

  //����� ������� ���������

  SState->vScroll.pos = 0;
  SState->hScroll.pos = 0;

  //��������� ��������������� ���������� � ��������� ���������

  SetScrollRange(hwnd, SB_VERT, 0, SState->vScroll.max, FALSE);
  SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE);

  SetScrollRange(hwnd, SB_HORZ, 0, SState->hScroll.max, FALSE);
  SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE);
}

//������� ������������� ��������� ����� ��� ��������� ������ �������

void ToLayout(SystemState* SState, HWND hwnd)
{
  for (int i = 0; i < SState->layParser.size; i++)  // - ��������� ������ ��������� ������
  {
    if (&SState->text.data[SState->layParser.stringBeg[i].begIndex] == &SState->text.data[SState->defParser.stringBeg[SState->printMetrix.beg].begIndex])
    {
      SState->vScroll.pos = min(i / SState->vScroll.coef, SState->vScroll.max); // - ��������� ���������������� ��������� ��������
      SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE);
      break;
    }
  }
}

//������� ������������� ��������� ����� ��� ��������� ������ ����������� �� ���������

void ToDefault(SystemState* SState, HWND hwnd)
{
  int ind = SState->printMetrix.beg;
  while (SState->layParser.stringBeg[ind].type != defaultV) // - ���������� ������ ��������� �������������� ������
    ind--;
  for (int i = 0; i < SState->defParser.size; i++)
  {
    if (&SState->text.data[SState->defParser.stringBeg[i].begIndex] == &SState->text.data[SState->layParser.stringBeg[ind].begIndex])
    {
      SState->vScroll.pos = min(i / SState->vScroll.coef, SState->vScroll.max); // - ��������� ���������������� ��������� ��������
      break;
    }
  }
}
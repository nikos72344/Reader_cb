#include "lab_1.h"
#include "file.h"
#include "funcs.h"
#include "scrolls.h"

//������� �������� ����

void CREATE_FUNC(SysState* SState, HWND hwnd, LPARAM lParam)
{
  HDC hdc;
  RECT rect;
  HMENU hMenu;
  TEXTMETRIC tm;
  static char fileName[_MAX_PATH], titleName[_MAX_FNAME + _MAX_EXT]; // - ����������� ������� ��� �������� ������������ � �������� �����

  hdc = GetDC(hwnd); // - ��������� ��������� ��������� ����������

  SState->vType = defaultV; // - ��������� ������ ����������� ������ �� ���������
  hMenu = GetMenu(hwnd); // - ��������� ��������� ����
  CheckMenuItem(hMenu, IDM_LAYOUT, MF_UNCHECKED); // - "����������" ������ ������ �������

  GetTextMetrics(hdc, &tm); // - ��������� ������� ��������� �������� �� ������
  SState->xChar = tm.tmAveCharWidth; // - ������ � ��������� ������ �������
  SState->yChar = tm.tmHeight + tm.tmExternalLeading; // - ������ � ��������� ������ �������: ������ ���� ������� ���������� ���������� ������

  ReleaseDC(hwnd, hdc); // - ������������ ��������� ��������� ����������

  //�������� ������ ������ ������, ����� ������ ����� ������������ ����

  SState->font = CreateFont(SState->yChar, SState->xChar, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Consolas");

  //��������� �������� ������� ������� ���� � ���������� �������� � ���������

  GetClientRect(hwnd, &rect);
  SState->xClient = rect.right;
  SState->yClient = rect.bottom;

  //���������� ������� �������� ��� ������������ � ����� �����

  SState->flName = fileName;
  SState->ttlName = titleName;

  //������������� ��������� OPENFILENAME

  FileInitialize(SState, hwnd);

  //��������� ���������� ������

  Args(SState, hwnd, lParam);
}

//������� ��������� ���������� ������� ����

void SIZE_FUNC(SysState* SState, HWND hwnd)
{
  RECT rect;

  //��������� � ���������� �������� ������� ������� ���� � ���������

  GetClientRect(hwnd, &rect);
  SState->xClient = rect.right;
  SState->yClient = rect.bottom;

  //���������� ������� ������ ������

  PrintMetrixUpd(SState, &rect);

  //���������� ������ ��������

  vScrollUpd(SState, hwnd);
  hScrollUpd(SState, hwnd);
}

//������� ��������� ������� ������� ����

void PAINT_FUNC(SysState* SState, HWND hwnd)
{
  HDC hdc;
  PAINTSTRUCT ps;
  int k = SState->prntMetrix.pntBeg; // - ������, ������������ ��� ������ ������ �� �����
  int index = 0;

  hdc = BeginPaint(hwnd, &ps); // - ���������� ���� � �����������

  PrintMetrixUpd(SState, &ps.rcPaint); // - ���������� ������� ������ ������

  SetBkMode(hdc, TRANSPARENT); // - ��������� ���� ������ ����������

  SelectObject(hdc, SState->font); // - ����� ������������� ������

  //������ ������ ������ �� �����

  switch (SState->vType)
  {
  case layoutV: // - ��� ������ �������
    for (int i = SState->prntMetrix.pntBeg; i < SState->prntMetrix.pntEnd; i++) // - ����� ����� �� �����
    {
      while (SState->strings[i].size - index >= SState->xClient / SState->xChar - 1) // - ����� ������ ������, ���������� ������ ������ ������
      {
        int x = SState->xChar * (1 - SState->scrlMetrix.hScrollPos * SState->scrlMetrix.hScrollCoef);
        int y = SState->yChar * (k - SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef);
        TextOut(hdc, x, y, &SState->strings[i].data[index], SState->xClient / SState->xChar - 1);
        index += SState->xClient / SState->xChar - 1;
        k++;
      }
      int x = SState->xChar * (1 - SState->scrlMetrix.hScrollPos * SState->scrlMetrix.hScrollCoef); // - ����� ������� ������, �� ����������� ��� ������ ������
      int y = SState->yChar * (k - SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef);
      TextOut(hdc, x, y, &SState->strings[i].data[index], SState->strings[i].size - index);
      index = 0;
      k++;
    }
    break;
  case defaultV: // - ��� ������ �� ���������
    for (int i = SState->prntMetrix.pntBeg; i < SState->prntMetrix.pntEnd; i++) // - ����� ����� ������
    {
      int x = SState->xChar * (1 - SState->scrlMetrix.hScrollPos * SState->scrlMetrix.hScrollCoef);
      int y = SState->yChar * (i - SState->scrlMetrix.vScrollPos * SState->scrlMetrix.vScrollCoef);
      TextOut(hdc, x, y, SState->strings[i].data, SState->strings[i].size);
    }
    break;
  }

  EndPaint(hwnd, &ps); // - ��������� ����������� ����
}

//������� ��������� ��������� �������������� ������������ � ����

void COMMAND_FUNC(SysState* SState, HWND hwnd, WPARAM wParam)
{
  HMENU hMenu = GetMenu(hwnd); // - ��������� ��������� ����
  RECT rect;

  //������ ��������� ���������

  switch (LOWORD(wParam))
  {
  case IDM_OPEN: // - ��������� ������� �� ������ "Open..."
    if (GetOpenFileName(&SState->ofn)) // - � ������ ��������� ������ ����� �������������
    {
      ReleaseMemory(SState->strings, SState->strNum); // - ������������� ���������� �������� �����
      ReadText(SState); // - ����������� ���������� ������ �����
      ChangeWndTitle(SState, hwnd); // - ���������� ��������� ����

      //���������� ������

      GetClientRect(hwnd, &rect);
      PrintMetrixUpd(SState, &rect);
      InvalidateRect(hwnd, &rect, TRUE);
      ScrollsReset(SState, hwnd);
      SendMessage(hwnd, WM_SIZE, 0, 0L);
    }
    else
      MessageBeep(0); // - � ��������� ������ �������� �������� ������ ������������
    break;
  case IDM_EXIT: // - ��������� ������� ������ "Exit"
    SendMessage(hwnd, WM_DESTROY, 0, 0L); // - �������� ��������� ���� �� �����������
    break;
  case IDM_LAYOUT: // - ��������� ������� ������ ������ �����������
    switch (SState->vType)
    {
    case layoutV: // - � ������ ��������� ������ �������
      CheckMenuItem(hMenu, IDM_LAYOUT, MF_UNCHECKED); // - "�����������" ������ "Layout"
      SState->vType = defaultV; // - ��������������� ��������������� ����� �����������

      //���������� ������

      GetClientRect(hwnd, &rect);
      PrintMetrixUpd(SState, &rect);
      vScrollUpd(SState, hwnd);
      hScrollUpd(SState, hwnd);
      InvalidateRect(hwnd, &rect, TRUE);
      break;
    case defaultV: // - � ������ ��������� ������ �� ���������
      CheckMenuItem(hMenu, IDM_LAYOUT, MF_CHECKED); // - "����������" ������ "Layout"
      SState->vType = layoutV; // - ��������������� ��������������� ����� �����������

      //���������� ������

      GetClientRect(hwnd, &rect);
      PrintMetrixUpd(SState, &rect);
      vScrollUpd(SState, hwnd);
      hScrollUpd(SState, hwnd);
      InvalidateRect(hwnd, &rect, TRUE);
      break;
    }
    break;
  case IDM_ABOUT: // - ��������� ������� ������ "About"
    MessageBox(hwnd, "     Text file reader\nby Nikita Vezdenetsky", "About", MB_ICONINFORMATION | MB_OK); // - ����� ��������������� ����
    break;
  }
}

//��������� ��������� ������������ ������ ���������

void VSCROLL_FUNC(SysState* SState, HWND hwnd, WPARAM wParam)
{
  RECT rect;
  int vScrollDiff;

  switch (LOWORD(wParam))
  {
  case SB_TOP: // - ����� � ������ ���������
    vScrollDiff = -1 * SState->scrlMetrix.vScrollPos;
    break;

  case SB_BOTTOM: // - ����� � ����� ���������
    vScrollDiff = SState->scrlMetrix.vScrollMax - SState->scrlMetrix.vScrollPos;
    break;

  case SB_LINEUP: // - ������� �� ������ ������� �����
    vScrollDiff = -1;
    break;

  case SB_LINEDOWN: // - ������� �� ������ ������� ����
    vScrollDiff = 1;
    break;

  case SB_PAGEUP: // - ����� �� ���� �������� �����
    vScrollDiff = min(-1, -SState->yClient / SState->yChar);
    break;

  case SB_PAGEDOWN: // - ����� �� ���� �������� ����
    vScrollDiff = max(1, SState->yClient / SState->yChar);
    break;

  case SB_THUMBTRACK: // - ����������� ��������
    vScrollDiff = (HIWORD(wParam) - SState->scrlMetrix.vScrollPos);
    break;

  default:
    vScrollDiff = 0;
  }

  //���������� �������� ������ �������� �� ������������� �������

  vScrollDiff = max(-SState->scrlMetrix.vScrollPos, min(vScrollDiff, SState->scrlMetrix.vScrollMax - SState->scrlMetrix.vScrollPos));

  if (vScrollDiff != 0 || LOWORD(wParam) == SB_THUMBPOSITION)
  {
    SState->scrlMetrix.vScrollPos += vScrollDiff; // - ��������� ������ �������� ��������� �������� � ���������
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff, NULL, NULL); // - ��������� ����
    SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE); // - ��������� ������ �������� �������� � ������ ���������
    UpdateWindow(hwnd);

    //���������� ������

    GetClientRect(hwnd, &rect);
    PrintMetrixUpd(SState, &rect);
    vScrollUpd(SState, hwnd);
    hScrollUpd(SState, hwnd);
    InvalidateRect(hwnd, &rect, TRUE);
  }
}

//��������� ��������� �������������� ������ ���������

void HSCROLL_FUNC(SysState* SState, HWND hwnd, WPARAM wParam)
{
  int hScrollDiff;

  switch (LOWORD(wParam))
  {
  case SB_LINEUP: // - ������� ������ ������� �����
    hScrollDiff = -1;
    break;

  case SB_LINEDOWN: // - ������� ������ ������� ������
    hScrollDiff = 1;
    break;

  case SB_PAGEUP: // - ����� �� ���� �������� �����
    hScrollDiff = -8;
    break;

  case SB_PAGEDOWN: // - ����� �� ���� �������� ������
    hScrollDiff = 8;
    break;

  case SB_THUMBTRACK: // - ����������� ��������
    hScrollDiff = HIWORD(wParam) - SState->scrlMetrix.hScrollPos;
    break;

  default:
    hScrollDiff = 0;
  }

  //���������� ������ �������� �������� ��������� �������� �� ������������� �������

  hScrollDiff = max(-SState->scrlMetrix.hScrollPos, min(hScrollDiff, SState->scrlMetrix.hScrollMax - SState->scrlMetrix.hScrollPos));

  if (hScrollDiff != 0)
  {
    SState->scrlMetrix.hScrollPos += hScrollDiff; // - ��������� ������ ��������� �������� � ���������
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff, 0, NULL, NULL); // - ��������� ����
    SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE); // - ��������� ������ ��������� �������� � ������ ���������
    UpdateWindow(hwnd);
  }
}

//��������� ��������� �� �������� ���� � ���������� ��� ������������ ������ ���������

void AUTOvSCROLL_FUNC(SysState* SState, HWND hwnd, int vScrollDiff)
{
  RECT rect;

  //���������� ������ �������� �������� ��������� �������� �� ������������� �������

  vScrollDiff = max(-SState->scrlMetrix.vScrollPos, min(vScrollDiff, SState->scrlMetrix.vScrollMax - SState->scrlMetrix.vScrollPos));

  if (vScrollDiff != 0)
  {
    SState->scrlMetrix.vScrollPos += vScrollDiff; // - ��������� ������ ��������� �������� � ���������
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff, NULL, NULL);  // - ��������� ����
    SetScrollPos(hwnd, SB_VERT, SState->scrlMetrix.vScrollPos, TRUE); // - ��������� ������ ��������� �������� � ������ ���������
    UpdateWindow(hwnd);

    //���������� ������

    GetClientRect(hwnd, &rect);
    PrintMetrixUpd(SState, &rect);
    vScrollUpd(SState, hwnd);
    hScrollUpd(SState, hwnd);
    InvalidateRect(hwnd, &rect, TRUE);
  }
}

//��������� ��������� �� �������� ���� � ���������� ��� �������������� ������ ���������

void AUTOhSCROLL_FUNC(SysState* SState, HWND hwnd, int hScrollDiff)
{

  //���������� ������ �������� �������� ��������� �������� �� ������������� �������

  hScrollDiff = max(-SState->scrlMetrix.hScrollPos, min(hScrollDiff, SState->scrlMetrix.hScrollMax - SState->scrlMetrix.hScrollPos));

  if (hScrollDiff != 0)
  {
    SState->scrlMetrix.hScrollPos += hScrollDiff; // - ��������� ������ ��������� �������� � ���������
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff, 0, NULL, NULL);  // - ��������� ����
    SetScrollPos(hwnd, SB_HORZ, SState->scrlMetrix.hScrollPos, TRUE); // - ��������� ������ ��������� �������� � ������ ���������
    UpdateWindow(hwnd);
  }
}

//������� ���������� �������� ����� ������������ ����

void DESTROY_FUNC(SysState* SState)
{
  ReleaseMemory(SState->strings, SState->strNum); // - ������������ ����������� ������ ������
  DeleteObject(SState->font); // - �������� ������
  PostQuitMessage(0);
}

#include "lab_1.h"
#include "file.h"
#include "funcs.h"
#include "scrolls.h"

//������� �������� ����

void CREATE_FUNC(SystemState* SState, HWND hwnd, LPARAM lParam)
{
  HMENU hMenu;
  TEXTMETRIC tm;
  HDC hdc;
  static char filePath[_MAX_PATH], fileTitle[_MAX_FNAME + _MAX_EXT]; // - ����������� ������� ��� �������� ������������ � �������� �����

  SState->vType = defaultV; // - ��������� ������ ����������� ������ �� ���������
  SState->filePath = filePath;  // - ���������� ������ ������� ��� ���� � �����
  SState->fileTitle = fileTitle;  // - ���������� ������ ������� ��� ������� �������� �����

  //������������� ��������� OPENFILENAME

  OFNInitialize(SState, hwnd);

  //��������� ������� �������

  SState->defParser.stringBeg = NULL;
  SState->defParser.size = 0;
  SState->layParser.stringBeg = NULL;
  SState->layParser.size = 0;

  hdc = GetDC(hwnd);  // - ��������� ��������� ��������� ����������
  GetTextMetrics(hdc, &tm); // - ��������� ������� ��������� �������� �� ������
  SState->xChar = tm.tmAveCharWidth; // - ������ � ��������� ������ �������
  SState->yChar = tm.tmHeight + tm.tmExternalLeading; // - ������ � ��������� ������ �������: ������ ���� ������� ���������� ���������� ������

  //�������� ������ ������ ������, ����� ������ ����� ������������ ����

  SState->font = CreateFontA(SState->yChar, SState->xChar, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Consolas");
  SetBkMode(hdc, TRANSPARENT); // - ��������� ���� ������ ����������
  SelectObject(hdc, SState->font); // - ��������� ����

  ReleaseDC(hwnd, hdc); // - ������������ ��������� ��������� ����������

  hMenu = GetMenu(hwnd); // - ��������� ��������� ����
  CheckMenuItem(hMenu, IDM_LAYOUT, MF_UNCHECKED); // - "����������" ������ ������ �������

  //��������� ���������� ������

  ArgumentsProcessing(SState, hwnd, lParam);
}

//������� ��������� ���������� ������� ����

void SIZE_FUNC(SystemState* SState, HWND hwnd, LPARAM lParam)
{
  //��������� � ���������� �������� ������� ������� ���� � ���������

  SState->xClient = LOWORD(lParam);
  SState->yClient = HIWORD(lParam);

  //���������� ������ ������ �����

  TextResizer(SState, hwnd);

  //���������� ������ ��������

  vScrollUpd(SState, hwnd);
  hScrollUpd(SState, hwnd);

  //���������� ������� ������ ������

  PrintMetrixUpd(SState);
}

//������� ��������� ������� ������� ����

void PAINT_FUNC(SystemState* SState, HWND hwnd)
{
  HDC hdc;
  PAINTSTRUCT ps;
  ParserMetrix* parser = NULL;
  int i;
  hdc = BeginPaint(hwnd, &ps);  // - ������ ����������� ����

  //����� ���������������� ������� ����� �����

  switch (SState->vType)
  {
  case layoutV: parser = &SState->layParser; break;
  case defaultV: parser = &SState->defParser; break;
  }

  //����� ����� ������

  for (i = SState->printMetrix.beg; i < SState->printMetrix.end; i++)
  {
    int x = SState->xChar * (1 - SState->hScroll.pos * SState->hScroll.coef);
    int y = SState->yChar * (i - SState->vScroll.pos * SState->vScroll.coef);
    TextOutA(hdc, x, y, &SState->text.data[parser->stringBeg[i].begIndex], i == (parser->size - 1) ? (SState->text.size - parser->stringBeg[i].begIndex) : (parser->stringBeg[i + 1].begIndex - parser->stringBeg[i].begIndex));
  }

  EndPaint(hwnd, &ps); // - ��������� ����������� ����
}

//������� ��������� ��������� �������������� ������������ � ����

void COMMAND_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam)
{
  HMENU hMenu = GetMenu(hwnd); // - ��������� ��������� ����

  //������ ��������� ���������

  switch (LOWORD(wParam))
  {
  case IDM_OPEN: // - ��������� ������� �� ������ "Open..."
    if (GetOpenFileNameA(&SState->ofn)) // - � ������ ��������� ������ ����� �������������
    {
      OpenFileProcessing(SState, hwnd); // - ������ � ��������� ������ �� �����
      InvalidateRect(hwnd, NULL, TRUE); // - ������������ ��� ������� ����
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

      ToDefault(SState, hwnd);  // - ������������� ��������� �����
      vScrollUpd(SState, hwnd); // - ���������� ������ ������������ ������ ���������
      hScrollUpd(SState, hwnd); // - ���������� ������ �������������� ������ ���������
      PrintMetrixUpd(SState); // - ����������� ������ ������ ������
      InvalidateRect(hwnd, NULL, TRUE); // - ������������ ��� ������� ����
      break;
    case defaultV: // - � ������ ��������� ������ �� ���������
      CheckMenuItem(hMenu, IDM_LAYOUT, MF_CHECKED); // - "����������" ������ "Layout"
      SState->vType = layoutV; // - ��������������� ��������������� ����� �����������

      //���������� ������

      vScrollUpd(SState, hwnd); // - ���������� ������ ������������ ������ ���������
      hScrollUpd(SState, hwnd); // - ���������� ������ �������������� ������ ���������
      ToLayout(SState, hwnd); // - ������������� ��������� �����
      PrintMetrixUpd(SState); // - ����������� ������ ������ ������
      InvalidateRect(hwnd, NULL, TRUE); // - ������������ ��� ������� ����
      break;
    }
    break;
  case IDM_ABOUT: // - ��������� ������� ������ "About"
    MessageBoxA(hwnd, "     Text file reader\nby Nikita Vezdenetsky", "About", MB_ICONINFORMATION | MB_OK); // - ����� ��������������� ����
    break;
  }
}

//��������� ��������� ������������ ������ ���������

void VSCROLL_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam)
{
  int vScrollDiff;

  switch (LOWORD(wParam))
  {
  case SB_TOP: // - ����� � ������ ���������
    vScrollDiff = -1 * SState->vScroll.pos;
    break;

  case SB_BOTTOM: // - ����� � ����� ���������
    vScrollDiff = SState->vScroll.max - SState->vScroll.pos;
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
    vScrollDiff = (HIWORD(wParam) - SState->vScroll.pos);
    break;

  default:
    vScrollDiff = 0;
  }

  //���������� �������� ������ �������� �� ������������� �������

  vScrollDiff = max(-SState->vScroll.pos, min(vScrollDiff, SState->vScroll.max - SState->vScroll.pos));

  if (vScrollDiff != 0 || LOWORD(wParam) == SB_THUMBPOSITION)
  {
    SState->vScroll.pos += vScrollDiff; // - ��������� ������ �������� ��������� �������� � ���������
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff * SState->vScroll.coef, NULL, NULL); // - ��������� ����
    SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE); // - ��������� ������ �������� �������� � ������ ���������
    PrintMetrixUpd(SState); // - ����������� ������ ������ ������
    UpdateWindow(hwnd); // - ���������� ����
  }
}

//��������� ��������� �������������� ������ ���������

void HSCROLL_FUNC(SystemState* SState, HWND hwnd, WPARAM wParam)
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
    hScrollDiff = HIWORD(wParam) - SState->hScroll.pos;
    break;

  default:
    hScrollDiff = 0;
  }

  //���������� ������ �������� �������� ��������� �������� �� ������������� �������

  hScrollDiff = max(-SState->hScroll.pos, min(hScrollDiff, SState->hScroll.max - SState->hScroll.pos));

  if (hScrollDiff != 0)
  {
    SState->hScroll.pos += hScrollDiff; // - ��������� ������ ��������� �������� � ���������
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff * SState->hScroll.coef, 0, NULL, NULL); // - ��������� ����
    SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE); // - ��������� ������ ��������� �������� � ������ ���������
    UpdateWindow(hwnd); // - ���������� ����
  }
}

//��������� ��������� �� �������� ���� � ���������� ��� ������������ ������ ���������

void AUTOvSCROLL_FUNC(SystemState* SState, HWND hwnd, int vScrollDiff)
{
  //���������� ������ �������� �������� ��������� �������� �� ������������� �������

  vScrollDiff = max(-SState->vScroll.pos, min(vScrollDiff, SState->vScroll.max - SState->vScroll.pos));

  if (vScrollDiff != 0)
  {
    SState->vScroll.pos += vScrollDiff; // - ��������� ������ �������� ��������� �������� � ���������
    ScrollWindow(hwnd, 0, -SState->yChar * vScrollDiff * SState->vScroll.coef, NULL, NULL); // - ��������� ����
    SetScrollPos(hwnd, SB_VERT, SState->vScroll.pos, TRUE); // - ��������� ������ �������� �������� � ������ ���������
    PrintMetrixUpd(SState); // - ����������� ������ ������ ������
    UpdateWindow(hwnd); // - ���������� ����
  }
}

//��������� ��������� �� �������� ���� � ���������� ��� �������������� ������ ���������

void AUTOhSCROLL_FUNC(SystemState* SState, HWND hwnd, int hScrollDiff)
{

  //���������� ������ �������� �������� ��������� �������� �� ������������� �������

  hScrollDiff = max(-SState->hScroll.pos, min(hScrollDiff, SState->hScroll.max - SState->hScroll.pos));

  if (hScrollDiff != 0)
  {
    SState->hScroll.pos += hScrollDiff; // - ��������� ������ ��������� �������� � ���������
    ScrollWindow(hwnd, -SState->xChar * hScrollDiff * SState->hScroll.coef, 0, NULL, NULL); // - ��������� ����
    SetScrollPos(hwnd, SB_HORZ, SState->hScroll.pos, TRUE); // - ��������� ������ ��������� �������� � ������ ���������
    UpdateWindow(hwnd); // - ���������� ����
  }
}

//������� ���������� �������� ����� ������������ ����

void DESTROY_FUNC(SystemState* SState)
{
  DeleteObject(SState->font); // - �������� ������
  if (SState->text.data != NULL) // - ������������ ������ ��� ������
    free(SState->text.data);
  if (SState->defParser.stringBeg != NULL)  // - ������������ ������ ������� ����� ����� ��� ������ ����������� �� ���������
    free(SState->defParser.stringBeg);
  if (SState->layParser.stringBeg != NULL)  // - ������������ ������ ������� ����� ����� ��� ������ �������
    free(SState->layParser.stringBeg);
  PostQuitMessage(0);
}

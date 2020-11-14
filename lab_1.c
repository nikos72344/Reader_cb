#include "lab_1.h"
#include "funcs.h"

//�������� ����������� �������

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//������� �����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
  char appName[] = "Reader"; // - �������� ���� ���������
  HWND        hwnd;
  MSG         msg;
  WNDCLASSEX  wndclass;

  //��������� ��������� ����

  wndclass.cbSize = sizeof(wndclass);
  wndclass.style = CS_HREDRAW | CS_VREDRAW; // - ����������� ���� ��� ��������� ��������
  wndclass.lpfnWndProc = WndProc; // - �������� ����������� �������
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIcon(hInstance, appName); // - ��������� ������ �� ��������
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // - ���������� ���� ����� ������
  wndclass.lpszMenuName = appName;
  wndclass.lpszClassName = appName;
  wndclass.hIconSm = LoadIcon(hInstance, appName); // - ��������� ��������� ������

  //����������� ������ ����

  RegisterClassEx(&wndclass);

  //��������� ��������� ����

  hwnd = CreateWindow(
    appName,
    appName,
    WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, // - ������������� ���� � �������������� � ������������ �������� ���������
    CW_USEDEFAULT, CW_USEDEFAULT,
    CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, NULL, hInstance, szCmdLine); // - �������� �������� ��������� ������

  //������������� ����� ���� � ��� �����������

  ShowWindow(hwnd, iCmdShow);
  UpdateWindow(hwnd);

  //���� ��������� ���������

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

//���� ����������� �������

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  static SysState sysState; // - �������� ���������� ������ �������������

  //������ ��������� ��������� �� Windows

  switch (iMsg)
  {
  case WM_CREATE: // - �������� ����
    CREATE_FUNC(&sysState, hwnd, lParam);
    return 0;

  case WM_SIZE: // - ��������� ������� ����
    SIZE_FUNC(&sysState, hwnd);
    return 0;

  case WM_PAINT: // - ��������� ����
    PAINT_FUNC(&sysState, hwnd);
    return 0;

  case WM_COMMAND: // - ��������� ������ ������ ����
    COMMAND_FUNC(&sysState, hwnd, wParam);
    return 0;

  case WM_VSCROLL: // - ��������� ������������� �������
    VSCROLL_FUNC(&sysState, hwnd, wParam);
    return 0;

  case WM_HSCROLL: // - ��������� ��������������� �������
    HSCROLL_FUNC(&sysState, hwnd, wParam);
    return 0;

   case WM_MOUSEWHEEL: // - ��������� �������� ����
     AUTOvSCROLL_FUNC(&sysState, hwnd, -1 * GET_WHEEL_DELTA_WPARAM(wParam) / abs(GET_WHEEL_DELTA_WPARAM(wParam)) * sysState.yClient / sysState.yChar / autoScroll * sysState.scrlMetrix.vScrollCoef);
     return 0;

   case WM_KEYDOWN: // - ��������� ���������� ������
     switch (wParam)
     {
     case VK_UP: // - ������� "������� �����"
       AUTOvSCROLL_FUNC(&sysState, hwnd, -1 * sysState.yClient / sysState.yChar / autoScroll * sysState.scrlMetrix.vScrollCoef);
       break;

     case VK_DOWN: // - ������� "������� ����"
       AUTOvSCROLL_FUNC(&sysState, hwnd, sysState.yClient / sysState.yChar / autoScroll * sysState.scrlMetrix.vScrollCoef);
       break;

     case VK_LEFT: // - ������� "������� �����"
       AUTOhSCROLL_FUNC(&sysState, hwnd, -sysState.xClient / sysState.xChar / autoScroll * sysState.scrlMetrix.hScrollCoef);
       break;

     case VK_RIGHT: // - ������� "������� ������"
       AUTOhSCROLL_FUNC(&sysState, hwnd, sysState.xClient / sysState.xChar / autoScroll * sysState.scrlMetrix.hScrollCoef);
       break;
     }
     return 0;

  case WM_DESTROY: // - ����������� ����
    DESTROY_FUNC(&sysState);
    return 0;
  }
  return DefWindowProc(hwnd, iMsg, wParam, lParam); // - �-�, �������������� ������������ ���� ���������� ���������
}

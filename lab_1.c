#include "lab_1.h"
#include "funcs.h"

//Прототип процедурной функции

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Функция входа

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
  char appName[] = "Reader"; // - название окна программы
  HWND        hwnd;
  MSG         msg;
  WNDCLASSEX  wndclass;

 //Заполняем структуру окна

  wndclass.cbSize = sizeof(wndclass);
  wndclass.style = CS_HREDRAW | CS_VREDRAW;  //Заполняем структуру окна
  wndclass.lpfnWndProc = WndProc; // - указание процедурной функции
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIcon(hInstance, appName); // - установка иконки из ресурсов
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // - заполнение фона белой кистью
  wndclass.lpszMenuName = appName;
  wndclass.lpszClassName = appName;
  wndclass.hIconSm = LoadIcon(hInstance, appName); // - установка маленькой иконки

//Регистрация класса окна

  RegisterClassEx(&wndclass);

  //Получение описателя окна

  hwnd = CreateWindow(
    appName,
    appName,
    WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, // - перекрывающее окно с горизонтальной и вертикальной полосами прокрутки
    CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, NULL, hInstance, szCmdLine); // - передаем аргумент командной строки

  //Устанваливаем показ окна и его перерисовку

  ShowWindow(hwnd, iCmdShow);
  UpdateWindow(hwnd);

  //Цикл обработки сообщений

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

//Тело процедурной функции

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  static SysState sysState; // - основная струкутура модели представления
  
  //Логика обработки сообщений от Windows

  switch (iMsg)
  {
  case WM_CREATE: // - Создание окна
    CREATE_FUNC(&sysState, hwnd, lParam);
    return 0;

  case WM_SIZE: // - Изменение размера окна
    SIZE_FUNC(&sysState, hwnd);
    return 0;

  case WM_PAINT: // - Отрисовка окна
    PAINT_FUNC(&sysState, hwnd);
    return 0;

  case WM_COMMAND: // - Обработка выбора пункта меню
    COMMAND_FUNC(&sysState, hwnd, wParam);
    return 0;

  case WM_VSCROLL: // - Обработка вертикального скролла
    VSCROLL_FUNC(&sysState, hwnd, wParam);
    return 0;

  case WM_HSCROLL: // - Обработка горизонтального скролла
    HSCROLL_FUNC(&sysState, hwnd, wParam);
    return 0;

   case WM_MOUSEWHEEL: // - Обработка колесика мыши
     AUTOvSCROLL_FUNC(&sysState, hwnd, -1 * GET_WHEEL_DELTA_WPARAM(wParam) / abs(GET_WHEEL_DELTA_WPARAM(wParam)) * sysState.yClient / sysState.yChar / autoScroll * sysState.scrlMetrix.vScrollCoef);
     return 0;

   case WM_KEYDOWN: // - Обработка нажимаемых клавиш
     switch (wParam)
     {
     case VK_UP: // - Клавиши "Стрелка вверх"
       AUTOvSCROLL_FUNC(&sysState, hwnd, -1 * sysState.yClient / sysState.yChar / autoScroll * sysState.scrlMetrix.vScrollCoef);
       break;

     case VK_DOWN: // - Клавиши "Стрелка вниз"
       AUTOvSCROLL_FUNC(&sysState, hwnd, sysState.yClient / sysState.yChar / autoScroll * sysState.scrlMetrix.vScrollCoef);
       break;

     case VK_LEFT: // - Клавиши "Стрелка влево"
       AUTOhSCROLL_FUNC(&sysState, hwnd, -sysState.xClient / sysState.xChar / autoScroll * sysState.scrlMetrix.hScrollCoef);
       break;

     case VK_RIGHT: // - Клавиши "Стрелка вправо"
       AUTOhSCROLL_FUNC(&sysState, hwnd, sysState.xClient / sysState.xChar / autoScroll * sysState.scrlMetrix.hScrollCoef);
       break;
     }
     return 0;

  case WM_DESTROY: // - Уничтожение окна
    DESTROY_FUNC(&sysState);
    return 0;
  }
  return DefWindowProc(hwnd, iMsg, wParam, lParam); // - Ф-я, обрабатывающая игнорируемые моей программой сообщения
}

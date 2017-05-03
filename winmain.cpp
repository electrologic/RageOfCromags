// SmartEdit.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include <Mmsystem.h>	// contains timeGetTime();
#include "winmain.h"
#include "Antares\\antares.h"
#include "Antares\\timer.h"
#include "Antares\\controls.h"
#include "game.h"

#include <stdio.h>

//#define SET_VSYNC
//#define FPS_LOG

#ifdef FPS_LOG
FILE *fp_fpslog;
#endif

const unsigned int	screenWidth = 800;
const unsigned int	screenHeight = 600;

CAntares engine;
CGame game;
extern CMouse mouse;
extern CKeys keys;
CExitControl exitControl;
extern CTimer timer;
extern CGameView gameView;

#define MAX_LOADSTRING 100

// Глобальные переменные:
HDC hDC;
HWND hWnd;

// Frame Timing
DWORD t0, t0fps, dt, nFixedDelta;
unsigned int cfps, nFPS;
float fTime, fDeltaTime;

//
bool pauseKeyLock = false;
bool escKeyLock = false;
//bool spaceKeyLock = false;
bool fpsKeyLock = false;

//
bool isCloseWindowButton = false;
bool isWinInitOver = false;


HINSTANCE hInst;						// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];			// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];	// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;
	long int savedTickCount = 0;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SMARTEDIT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SMARTEDIT));
	
	//
	// Main loop:
	//
	while (1)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == FALSE)
		{
			do{ dt = timeGetTime() - t0; } while (dt<1);

			if (dt >= nFixedDelta)
			{
				fDeltaTime = dt / 1000.0f;
				timer.setDeltaTime(fDeltaTime); // save delta time

				if (fDeltaTime>0.2f)
				{
					//
					//... FPS too low - 
					// need to write code for fix
					// this case
					//
				}

				fTime += fDeltaTime;
				timer.setTime(fTime); // save time

				t0 = timeGetTime();

				if (t0 - t0fps <= 1000)
					cfps++;
				else
				{
					nFPS = cfps; cfps = 0; t0fps = t0;
					timer.setFPS(nFPS); // save FPS

#ifdef FPS_LOG
					fprintf(fp_fpslog, "%u\n", nFPS);
#endif

				}
			}


			// Redraw & Render

			hDC = GetDC(hWnd);

			engine.startRedraw();

			game.render();

			engine.stopRedraw(hDC);

			// processing control
			game.controller();

            // проверка параметра разрешения на выход из программы. Параметр передан из диалога, порождаемого в game.cpp 
            if (exitControl.getState() == true)
            {
                DestroyWindow(hWnd); // закрыть окно, выйти из программы
            }

            if (gameView.getGameStartError() == true)
            {
                MessageBox(NULL, L"File corruped!", L"Error!", MB_OK | MB_TOPMOST);
                DestroyWindow(hWnd);
            }

            // проверка триггера переключения в режим выхода из игры (триггер устанавливается в активное состояние 
            // после нажатия на кнопку закрытия окна). В данном режиме производится вывод MessageBox с запросом подтсверждения
            if (isCloseWindowButton == true)
            {
                int messageResult = MessageBox(NULL, L"Закрыть игру?", L"Выход", MB_ICONQUESTION | MB_YESNO | MB_TOPMOST);

                switch (messageResult)
                {
                case IDYES:
                    DestroyWindow(hWnd); // закрыть окно, выйти из программы
                    break;

                case IDNO:
                    exitControl.setState(false);
                    // снять с паузы путем имитации нажатия соответствующей кнопки
                    keys.setPressed(P_BUTTON, true);

                    // снятие запрета обрабатывать нажатие ESC, поскольку в данном месте можем быть до того, как 
                    // будет произведена обработка отпускания кнопки ESC 
                    isCloseWindowButton = false;

                    break;
                }

            }

		}

		if (GetMessage(&msg, NULL, 0, 0) != TRUE)
		{
			// Deinitialization Antares
			engine.uninit();
			return msg.wParam;
		}

//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//		{
//		нужно-ли ?

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

/*
	// Старая версия основного цикла:

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
*/

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // was only CS_HREDRAW | CS_VREDRAW
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMARTEDIT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SMARTEDIT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
//   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   HDC hDCScreen;	// Display DC for detecting resolution 
   unsigned int winX, winY;	// Window coordinates
   unsigned int hRes, vRes; // Screen resolution
   unsigned int deltaX, deltaY; // Window borders
   RECT	rct;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
	   100, 100, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);


   // detect window border size
   GetClientRect(hWnd, &rct); // Save client rectangle of window
   deltaX = screenWidth - (rct.right - rct.left);
   deltaY = screenHeight - (rct.bottom - rct.top);

   // Locate window at center of the screen
   hDCScreen = GetDC(NULL);
   hRes = GetDeviceCaps(hDCScreen, HORZRES);
   vRes = GetDeviceCaps(hDCScreen, VERTRES);
   ReleaseDC(NULL, hDCScreen);
   winX = (int)(hRes - (screenWidth + deltaX)) / 2;
   winY = (int)(vRes - (screenHeight + deltaY)) / 2;

   // Change window size after init screen
   SetWindowPos(hWnd, HWND_TOP, winX, winY, screenWidth + deltaX, screenHeight + deltaY, SWP_SHOWWINDOW);

   if (!hWnd)
   {
      return FALSE;
   }

   // initialize OpenGL rendering 
   hDC = GetDC(hWnd);
   engine.setupPixelFormat(hDC);

   engine.init(screenWidth, screenHeight, hDC);

#ifdef SET_VSYNC
   engine.setVSync(true);	// disable vertical synchronization
#endif

#ifndef SET_VSYNC
   engine.setVSync(false);	// disable vertical synchronization
#endif

   game.init();

   timeBeginPeriod(1); // requests a minimum resolution for periodic timers

   // init Frame Timing Vars
   fTime = 0.0f;
   t0 = t0fps = timeGetTime();
   dt = cfps = 0;
   nFPS = 0;
   nFixedDelta = 0;	// if > 0 then fixed FPS mode ON


#ifdef FPS_LOG
   fp_fpslog = fopen("fps_log.txt", "wt");
#endif

   keys.initKeyArray();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   isWinInitOver = true;

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
//	HDC hDC;

	switch (message)
	{
	case WM_CREATE:

		// window initialization
		// empty, becouse window size & position will change

		return 0;
		break;

    case WM_WINDOWPOSCHANGING:

        // включение паузы происходит только если перетаскиваем окно в состоянии игры 
        if (gameView.getState() == PLAY)
        {
            // проверка - окончена ли инициализации окна приложения? Поскольку при нициализации приложения 
            // срабатывает данное сообщение из-за вызова изменения размера окна 
            if (isWinInitOver == true)
                exitControl.setPause(true);

            break;
        }

	case WM_PAINT:

		hDC = BeginPaint(hWnd, &ps);
		
		//
		// ???
		// Отсутствуе проверка (?) создания контекста, как в старом примере
		//
		// Antares redrawing 
		engine.startRedraw();

		game.render();

		engine.stopRedraw(hDC);

		EndPaint(hWnd, &ps);

		// processing control
		// не обязательно, т.к. врядли требуется обрабатывать события от 
		// мыши и клавиатуры при перетаскивании окна
		game.controller();

		break;

	//// Mouse message processing
	////
	case WM_MOUSEMOVE:

		// Координаты мыши при перемещении курсора в пределах окна, 
		// где 0,0 - верхний левый угол:

		// Передача координат мыши (с преобразованием по Y) в объект game (класса CGame)
		// координата мыши x = (int)(lParam & 0xFFFF)
		// координата мыши y = (int)(lParam >> 16) , относительно вернего левого угла окна
		mouse.setCoordinate((int)(lParam & 0xFFFF), screenHeight - (int)(lParam >> 16) - 1);

		break;

	case WM_LBUTTONDOWN:

		mouse.setLeftButtonPressed(true);
		
		break;

	case WM_LBUTTONUP:

		mouse.setLeftButtonPressed(false);

		break;

    case WM_CLOSE:

        // установка игры на паузу, чтобы при следующем вызове ф-ии render() приостановить 
        // работу таймера, поскольку будет вызван MessageBox для проверки подтверждения о намеренье выхода, 
        // нужно уже на это время находиться в состоянии паузы  
        keys.setPressed(P_BUTTON, true);
        // установка триггера перехода в режим выхода из программы
        //exitControl.setState(true);

        isCloseWindowButton = true;

        break;

	//// Keboard message processing
	////
	case WM_KEYDOWN:
		// handle keyboard input 
		switch ((int)wParam)
		{
		case VK_ESCAPE:

            if (escKeyLock == false)
            {
                escKeyLock = true;
                keys.setPressed(ESC_BUTTON, true);
            }

			break;
/*
        case VK_SPACE:

            if (spaceKeyLock == false)
            {
                spaceKeyLock = true;
                keys.setPressed(SPACE_BUTTON, true);
            }

            break;
*/
		case VK_DOWN:
			keys.setPressed(DOWN_ARROW_BUTTON, true);
			break;

		case VK_UP:
			keys.setPressed(UP_ARROW_BUTTON, true);
			break;

		case VK_LEFT:
			keys.setPressed(LEFT_ARROW_BUTTON, true);
			break;

		case VK_RIGHT:
			keys.setPressed(RIGHT_ARROW_BUTTON, true);
			break;

        case VK_RETURN:
            keys.setPressed(ENTER_BUTTON, true);
            break;

		case 0x5A: // 'Z' button
			keys.setPressed(Z_BUTTON, true);
			break;

        case 0x50:      // 'P' button
        case VK_PAUSE:  // 'Pause/Break' button

            if (pauseKeyLock == false)
            {
                pauseKeyLock = true;
                keys.setPressed(P_BUTTON, true);
            }

		    break;

        case 0x46: // 'F' button

            if (fpsKeyLock == false)
            {
                fpsKeyLock = true;

                if (keys.isPressed(F_BUTTON) == false)
                    keys.setPressed(F_BUTTON, true);
                else
                    keys.setPressed(F_BUTTON, false);
            }

            break;

//		case 0x50: // 'P' button
//		    keys.setPressed(P_BUTTON, true);
//			break;

		default:

			break;
		}

		break;

	case WM_KEYUP:
		// handle keyboard input 
		switch ((int)wParam)
		{
        case VK_ESCAPE:
            escKeyLock = false;
            break;
/*
        case VK_SPACE:
            spaceKeyLock = false;
            break;
*/
		case VK_DOWN:
			keys.setPressed(DOWN_ARROW_BUTTON, false);
			break;

		case VK_UP:
			keys.setPressed(UP_ARROW_BUTTON, false);
			break;

		case VK_LEFT:
			keys.setPressed(LEFT_ARROW_BUTTON, false);
			break;

		case VK_RIGHT:
			keys.setPressed(RIGHT_ARROW_BUTTON, false);
			break;

        case VK_RETURN:
            keys.setPressed(ENTER_BUTTON, false);
            break;

		case 0x5A: // 'Z' button
			keys.setPressed(Z_BUTTON, false);
			break;

        case 0x50:      // 'P' button
        case VK_PAUSE:  // 'Pause/Break' button
            pauseKeyLock = false;
            break;

        case 0x46: // 'F' button
            fpsKeyLock = false;
            break;

/*
		case 0x50: // 'P' button
			if (keys.isPressed(P_BUTTON) == false)
				keys.setPressed(P_BUTTON, true);
			else
				keys.setPressed(P_BUTTON, false);
			break;
*/
		default:
			break;
		}

		break;

	////
	case WM_DESTROY:

#ifdef FPS_LOG
		fclose(fp_fpslog);
#endif

		timeEndPeriod(1); // clears a previously set minimum timer resolution

		// Deinitialization Antares
		engine.uninit();

		game.uninit();

		ReleaseDC(hWnd, hDC);

		PostQuitMessage(0);
		
		break;

	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



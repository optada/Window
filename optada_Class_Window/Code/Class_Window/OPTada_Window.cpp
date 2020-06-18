// Created by OPTada // Free for use //
// - - - - - - - - - - - - - - - - - //

#include "OPTada_Window.h"


void OPTada_Window::Update_WindowSizeWithBorders()
{
	RECT window_rect = { 0, 0, workplaceSize.width, workplaceSize.height }; // work surface structure

	// The AdjustWindowRectEx function calculates the required size of the rectangle of the extended style window (directly of the working surface without outlines and menus)
	AdjustWindowRectEx(
		&window_rect,                        // pointer to the structure of the workspace
		GetWindowStyle(main_window_handle),  // window style
		GetMenu(main_window_handle) != NULL, // menu display flag
		GetWindowExStyle(main_window_handle) // extended style
		);

	if (windowSize.width != window_rect.right - window_rect.left || windowSize.height != window_rect.bottom - window_rect.top) {

		// changes the position and "dimensions" of the window to get the full size of the workspace - MoveWindow ()
		MoveWindow(main_window_handle,             // window handle
			0,                                     // x screen position
			0,                                     // y screen position
			window_rect.right  - window_rect.left, // width ... setting the full width of the window (taking into account the edges of the frame)
			window_rect.bottom - window_rect.top,  // height ... setting the full height of the window (taking into account the edges of the frame)
			FALSE);								   // repainting flag
	}

	// save global variables
	if (windowState != OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_FullScreen) {
		windowSize.width  = window_rect.right  - window_rect.left; // window X
		windowSize.height = window_rect.bottom - window_rect.top;  // window �
	}
	else {
		windowSize.width  = workplaceSize.width;  // window X
		windowSize.height = workplaceSize.height; // window �
	}

	ShowWindow(main_window_handle, SW_SHOW); // ������� ���������� ���� (���� ��� �� ���� �������� ����� ��� ��������� ���)
	UpdateWindow(main_window_handle);
}


bool OPTada_Window::InitAndCreateStandartWindow(HINSTANCE hinstance_)
{
	WNDCLASSEX winclass; // this will hold the class we create
	HWND	   hwnd;	 // generic window handle              
	//MSG	     msg;	   // generic message                    
	//HDC        hdc;      // graphics device context            

	// filling the class structure
	winclass.cbSize        = sizeof(WNDCLASSEX);                              // structure value (in bytes)
	winclass.style         = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // window style, declared by flags
	winclass.lpfnWndProc   = WindowProc;                                      // pointer to the name of the user-defined function (for processing window messages)
	winclass.cbClsExtra    = 0;										          // the number of bytes freed at the end of the structure
	winclass.cbWndExtra    = 0;                                               // number of bytes freed when creating an instance of the structure
	winclass.hInstance     = hinstance_;                                      // ... application instance
	winclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);                 // ... icon handle
	winclass.hCursor       = LoadCursor(NULL, IDC_ARROW);                     // ... cursor
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);             // ... window background color
	winclass.lpszMenuName  = NULL;                                            // pointer to a string containing the name of the menu used for the class
	winclass.lpszClassName = L"WIN3DCLASS";                                   // pointer to a string containing the class name
	winclass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);                 // ... small icon (in tray)

	// class registration
	if (!RegisterClassEx(&winclass)) {
		MessageBox(NULL, L"Class registration ERROR!", L"ERROR optada_window", MB_OK);
		return(0);
	}

	// create window
	if (!(hwnd = CreateWindowEx( 
		NULL,                                                                  // styles
		L"WIN3DCLASS",                                                         // class name
		L"WINDOW NAME",                                                        // ... window name(inscription above)
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE, // ... window display modes 
		0, 0,	                                                               // window display along x and y axes respectively
		OPTADA_CLASSWINDOW_DEFAULT_WIDTH, OPTADA_CLASSWINDOW_DEFAULT_HEIGHT,   // ... the width and height of the window (width || height), respectively
		NULL,	                                                               // parent window handle
		NULL,	                                                               // setting the menu handle
		hinstance_,                                                            // ... install an application instance
		NULL))                                                                 // (do not pass anything from WndProc)
		) 
	{
		MessageBox(NULL, L"Creating window ERROR!", L"ERROR optada_window", MB_OK);
		return(0);
	}

	main_window_hinstance = hinstance_; // save window indicator
	main_window_handle    = hwnd;       // save window handle
	//main_window_msg       = msg;        // saving the window message descriptor (for processing window messages)

	// update window size with borders
	windowSize.width  = OPTADA_CLASSWINDOW_DEFAULT_WIDTH;
	windowSize.height = OPTADA_CLASSWINDOW_DEFAULT_HEIGHT;

	workplaceSize.width  = OPTADA_CLASSWINDOW_DEFAULT_WIDTH;
	workplaceSize.height = OPTADA_CLASSWINDOW_DEFAULT_HEIGHT;

	Change_DisplayOfWindow(OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_Windowed, workplaceSize);

	return (1);
}

bool OPTada_Window::Change_DisplayOfWindow(OPTadaE_WindowState_ForClassWindow new_WindowState_, OPTadaS_Window_Size& new_WorkplaceSize_)
{

	if (new_WorkplaceSize_.width < OPTADA_CLASSWINDOW_MINIMUM_WORKPLACE_WIDTH || new_WorkplaceSize_.height < OPTADA_CLASSWINDOW_MINIMUM_WORKPLACE_HEIGHT) {
		return false;
	}

	workplaceSize.width  = new_WorkplaceSize_.width;
	workplaceSize.height = new_WorkplaceSize_.height;

	if (new_WindowState_ != windowState) {

		// switch OPTadaE_WindowState_ForClassWindow
		switch (new_WindowState_)
		{

		case NONE: break;

		case ENUM_WindowState_Windowed: {

			// set monitor - windowed
			ChangeDisplaySettings(NULL, 0);

			// set styles for window mode with borders
			SetWindowLong(main_window_handle, GWL_STYLE, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE);
			// se window reaction
			SetWindowPos(main_window_handle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

			// restore the location of the window with the frame
			SetWindowPlacement(main_window_handle, &windowPlacement_Windowed);

			windowState = new_WindowState_; // save new window state
			Update_WindowSizeWithBorders();

		} break;

		case ENUM_WindowState_WindowedWithNoBurders: {

			// set monitor - windowed
			ChangeDisplaySettings(NULL, 0);

			// set styles for window mode with borders
			SetWindowLong(main_window_handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			// se window reaction
			SetWindowPos(main_window_handle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

			// restore the location of the window with the frame
			SetWindowPlacement(main_window_handle, &windowPlacement_Windowed);

			windowState = new_WindowState_; // save new window state
			Update_WindowSizeWithBorders();

		} break;

		case ENUM_WindowState_FullScreen: {

			// save the location of the window with the frame
			windowPlacement_Windowed.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(main_window_handle, &windowPlacement_Windowed);

			// set monitor - fullScreen
			DEVMODE devMode = { 0 };
			// OPTadaS_Window_Size monitorSize;
			// Get_MonitorSize(monitorSize);
			devMode.dmSize       = sizeof(DEVMODE);
			devMode.dmPelsWidth  = workplaceSize.width;  // set max size of workplace // monitorSize.width;  - you can chage it on set max size of monitor
			devMode.dmPelsHeight = workplaceSize.height; // set max size of workplace // monitorSize.height; - you can chage it on set max size of monitor
			devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
			ChangeDisplaySettings(&devMode/*0*/, CDS_FULLSCREEN);

			// set styles for full screen mode
			SetWindowLong(main_window_handle, GWL_STYLE, WS_POPUP | WS_VISIBLE); 
			// set window reaction and position on screen
			SetWindowPos(main_window_handle, HWND_TOP, 0, 0, workplaceSize.width, workplaceSize.height, SWP_SHOWWINDOW);

			// save the window layout in full screen mode
			windowPlacement_FullScreen.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(main_window_handle, &windowPlacement_FullScreen);

			windowState = new_WindowState_; // save new window state

		} break;

		default: {
			return false; // error - have no new window state
		} break;

		} // end switch
	}

}


void OPTada_Window::Get_MonitorSize(OPTadaS_Window_Size& monitorSize_)
{
	DEVMODE devMode = { 0 };

	devMode.dmSize = sizeof(DEVMODE);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);

	monitorSize_.width  = devMode.dmPelsWidth;
	monitorSize_.height = devMode.dmPelsHeight;
}


void OPTada_Window::Get_WindowState(OPTadaE_WindowState_ForClassWindow& windowState_)
{
	windowState_ = windowState;
}

void OPTada_Window::Get_WindowSize(OPTadaS_Window_Size& windowSize_)
{
	windowSize_ = windowSize;
}

void OPTada_Window::Get_WorkplaceSize(OPTadaS_Window_Size& workplaceSize_)
{
	this;
	workplaceSize_.width = workplaceSize.width;
	workplaceSize_.height = workplaceSize.height;
}

HWND OPTada_Window::Get_MainWindowHandle()
{
	return main_window_handle;
}

void OPTada_Window::Do_FockusInFullScreenMode(bool haveFocus_)
{
	if (windowState != OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_FullScreen) {
		return;
	}

	if (haveFocus_) { // got focus

		DEVMODE devMode = { 0 };

		// OPTadaS_Window_Size monitorSize;
		// Get_MonitorSize(monitorSize);
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth  = workplaceSize.width;  // set max size of workplace // monitorSize.width;  - you can chage it on set max size of monitor
		devMode.dmPelsHeight = workplaceSize.height; // set max size of workplace // monitorSize.height; - you can chage it on set max size of monitor
		devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode/*0*/, CDS_FULLSCREEN);

		SetWindowPlacement(main_window_handle, &windowPlacement_FullScreen);
	}
	else { // loose focus

		ShowWindow(main_window_handle, SW_MINIMIZE);
	}
}


// --------------------------------------------------------------------------------------------



LRESULT CALLBACK WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{
	PAINTSTRUCT	ps;  // using in WM_PAINT
	HDC			hdc; // getting device context

	// get massage
	switch (msg)
	{

	case WM_KEYDOWN: {
		switch (wparam)
		{

		case 0x46: { // pressed 'F' button

			// change window mode
			OPTadaE_WindowState_ForClassWindow WindowProc_windowState;
			OPTadaS_Window_Size WindowProc_workPlaceSize;
			//(&global_Window)->Get_WindowState(WindowProc_windowState);
			//global_Window.Get_WorkplaceSize(WindowProc_workPlaceSize);
			WindowProc_windowState = OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_Windowed;
			WindowProc_workPlaceSize.width = 1280;
			WindowProc_workPlaceSize.height = 720;

			if (WindowProc_windowState == OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_FullScreen) {

				if (!global_Window.Change_DisplayOfWindow(OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_Windowed, WindowProc_workPlaceSize)) {
					MessageBox(NULL, L"Swich mode to window error", L"wind", NULL);
				};
			}
			else {

				if (!global_Window.Change_DisplayOfWindow(OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_FullScreen, WindowProc_workPlaceSize)) {
					MessageBox(NULL, L"Swich mode to fullscreen error", L"wind", NULL);
				};
			}
			
		} break;

		default: break;

		}
	} break;

	case WM_ACTIVATE: {

		OPTadaE_WindowState_ForClassWindow windowState;
		global_Window.Get_WindowState(windowState);

		// if fullscreen and focus occurs
		if (windowState == OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_FullScreen
			&& IsIconic(hwnd) && WA_ACTIVE == LOWORD(wparam)) {

			global_Window.Do_FockusInFullScreenMode(true);			
		}
		else {
			// if fullscreen and loss of focus occurs
			if (windowState == OPTadaE_WindowState_ForClassWindow::ENUM_WindowState_FullScreen && WA_INACTIVE == LOWORD(wparam)) {

				global_Window.Do_FockusInFullScreenMode(false);		
			}
		}
		
	} break;

	case WM_CREATE: {
		// all necessary initialization when creating a window occurs here
		return(0);
	} break;

	case WM_PAINT: {
		// start drawing
		hdc = BeginPaint(hwnd, &ps);
		// end of drawing
		EndPaint(hwnd, &ps);
		return(0);
	} break;

	case WM_DESTROY: {
		// delete window	
		PostQuitMessage(0);
		return(0);
	} break;

	default:break;

	} // end switch

	// window completionand windows control return
	return (DefWindowProc(hwnd, msg, wparam, lparam));
} 


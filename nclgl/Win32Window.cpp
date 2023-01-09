#include "Win32Window.h"
#ifdef _WIN32
#include "Windowsx.h"
#include "imgui.h"

Win32Window::Win32Window(const std::string& title, int sizeX, int sizeY, bool fullScreen, int offsetX, int offsetY)	{
	forceQuit		= false;
	init			= false;
	mouseLeftWindow	= false;
	lockMouse		= false;
	showMouse		= true;
	active			= true;

	this->fullScreen = fullScreen;

	size = Vector2((float)sizeX, (float)sizeY);
	fullscreenSize = Vector2(::GetDeviceCaps(::GetWindowDC(NULL), HORZRES), ::GetDeviceCaps(::GetWindowDC(NULL), VERTRES));
	defaultPosition = Vector2((float)offsetX, (float)offsetY);
	defaultSize = size;

	fullScreen ? position = Vector2(0, 0) : position = defaultPosition;
	fullScreen ? size = fullscreenSize : size = defaultSize;

	windowInstance = GetModuleHandle(NULL);

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	if (!GetClassInfoEx(windowInstance, WINDOWCLASS, &windowClass))	{
		windowClass.cbSize		= sizeof(WNDCLASSEX);
	    windowClass.style		= CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc	= (WNDPROC)WindowProc;
		windowClass.hInstance	= windowInstance;
		windowClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		windowClass.lpszClassName = WINDOWCLASS;

		if(!RegisterClassEx(&windowClass)) {
			std::cout << "Window::Window(): Failed to register class!" << std::endl;
			return;
		}
	}

	if(fullScreen) {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared

		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth		= sizeX;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight		= sizeY;			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel		= 32;				// Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = 60;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)	{
			std::cout << "Window::Window(): Failed to switch to fullscreen!" << std::endl;
			return;
		}
	}

	windowHandle = CreateWindowEx(fullScreen ? WS_EX_TOPMOST : NULL,
	WINDOWCLASS,    // name of the window class
	title.c_str(),   // title of the window
	fullScreen ? WS_POPUP|WS_VISIBLE : WS_OVERLAPPEDWINDOW|WS_POPUP|WS_VISIBLE|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,    // window style
						(int)position.x,	// x-position of the window
                        (int)position.y,	// y-position of the window
                        (int)size.x,		// width of the window
                        (int)size.y,		// height of the window
                        NULL,				// No parent window!
                        NULL,				// No Menus!
						windowInstance,		// application handle
                        NULL);				// No multiple windows!

 	if(!windowHandle) {
		std::cout << "Window::Window(): Failed to create window!" << std::endl;
		return;
	}

	winMouse	= new Win32Mouse(windowHandle);
	winKeyboard = new Win32Keyboard(windowHandle);

	keyboard	= winKeyboard;
	mouse		= winMouse;
	winMouse->SetAbsolutePositionBounds(size);

	winMouse->Wake();
	winKeyboard->Wake();

	LockMouseToWindow(lockMouse);
	ShowOSPointer(showMouse);

	SetConsolePosition(200, 200);

	windowTitle = title;

	init		= true;
	maximised	= false;
}

Win32Window::~Win32Window(void)	{
	init = false;
}

bool	Win32Window::IsFullScreen() {
	return fullScreen;
}


bool	Win32Window::InternalUpdate() {
	MSG		msg;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(windowHandle, &pt);
	winMouse->SetAbsolutePosition(Vector2((float)pt.x, (float)pt.y));

	while(PeekMessage(&msg,windowHandle,0,0,PM_REMOVE)) {
		CheckMessages(msg); 
	}

	return !forceQuit;
}

void	Win32Window::UpdateTitle()	{
	SetWindowText(windowHandle, windowTitle.c_str());
}

void	Win32Window::SetFullScreen(bool fullScreen) {
	Vector2 newSize, newPos;
	if (fullScreen)
	{
		newSize = fullscreenSize;
		newPos = Vector2(0, 0);
		SetWindowLong(windowHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	}
	else
	{
		newSize = defaultSize;
		newPos = defaultPosition;
		SetWindowLong(windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
	}
	::SetWindowPos(windowHandle, NULL, newPos.x, newPos.y, newSize.x, newSize.y, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	{
		RECT cRect;
		::GetClientRect(windowHandle, &cRect);
		size.x = cRect.right - cRect.left;
		size.y = cRect.bottom - cRect.top;
		position.x = cRect.left;
		position.y = cRect.top;
	}
	//winMouse->UpdateWindowPosition(position);
	//SetActiveWindow(windowHandle);
	winMouse->SetAbsolutePositionBounds(size);
	ResizeRenderer();
	if (init && lockMouse) LockMouseToWindow(true);
	return;
}

void Win32Window::ImguiInputUpdate(RAWINPUT* data) {
	ImGuiIO& io = ImGui::GetIO();
	RAWINPUT* rawInput = data;

	if (rawInput->header.dwType == RIM_TYPEMOUSE)
	{
		static int btDowns[5] = { RI_MOUSE_BUTTON_1_DOWN,
								  RI_MOUSE_BUTTON_2_DOWN,
								  RI_MOUSE_BUTTON_3_DOWN,
								  RI_MOUSE_BUTTON_4_DOWN,
								  RI_MOUSE_BUTTON_5_DOWN };

		static int btUps[5] = { RI_MOUSE_BUTTON_1_UP,
								RI_MOUSE_BUTTON_2_UP,
								RI_MOUSE_BUTTON_3_UP,
								RI_MOUSE_BUTTON_4_UP,
								RI_MOUSE_BUTTON_5_UP };

		for (int i = 0; i < 5; ++i) {
			if (rawInput->data.mouse.usButtonFlags & btDowns[i]) {
				io.MouseDown[i] = true;
			}
			else if (rawInput->data.mouse.usButtonFlags & btUps[i]) {
				io.MouseDown[i] = false;
			}
		}
	}
	else if (rawInput->header.dwType == RIM_TYPEKEYBOARD)
	{
		USHORT key = rawInput->data.keyboard.VKey;
		bool down = !(rawInput->data.keyboard.Flags & RI_KEY_BREAK);

		if (key < 256)
			io.KeysDown[key] = down;
		if (key == VK_CONTROL)
			io.KeyCtrl = down;
		if (key == VK_SHIFT)
			io.KeyShift = down;
		if (key == VK_MENU)
			io.KeyAlt = down;

	}
}

void Win32Window::CheckMessages(MSG &msg)	{
	Win32Window* thisWindow = (Win32Window*)window;
	switch (msg.message)	{				// Is There A Message Waiting?
		case (WM_QUIT):
		case (WM_CLOSE): {					// Have We Received A Quit Message?
			thisWindow->ShowOSPointer(true);
			thisWindow->LockMouseToWindow(false);
			forceQuit = true;
		}break;
		case (WM_INPUT): {
			UINT dwSize;
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize,sizeof(RAWINPUTHEADER));

			BYTE* lpb = new BYTE[dwSize];
	
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize,sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			ImguiInputUpdate(raw);

			if (keyboard && raw->header.dwType == RIM_TYPEKEYBOARD && active) {
				thisWindow->winKeyboard->UpdateRAW(raw);
			}

			if (mouse && raw->header.dwType == RIM_TYPEMOUSE && active) {			
				thisWindow->winMouse->UpdateRAW(raw);
			}

			delete lpb;
		}break;

		default: {								// If Not, Deal With Window Messages
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)	{
	Win32Window* thisWindow = (Win32Window*)window;

	bool applyResize = false;

    switch(message)	 {
        case(WM_DESTROY):	{
			thisWindow->ShowOSPointer(true);
			thisWindow->LockMouseToWindow(false);

			PostQuitMessage(0);
			thisWindow->forceQuit = true;
		} break;
		case (WM_ACTIVATE): {
			if(LOWORD(wParam) == WA_INACTIVE)	{
				thisWindow->active = false;
				ReleaseCapture();
				ClipCursor(NULL);
				if (thisWindow->init && mouse && keyboard) {
					thisWindow->winMouse->Sleep();
					thisWindow->winKeyboard->Sleep();
				}
			}
			else{
				thisWindow->active = true;
				if(thisWindow->init) {
					thisWindow->winMouse->Wake();
					thisWindow->winKeyboard->Wake();

					if(thisWindow->lockMouse) {
						thisWindow->LockMouseToWindow(true);
					}
				}
			}
			return 0;
		}break;
		case (WM_LBUTTONDOWN): {
			if(thisWindow->init && thisWindow->lockMouse) {
				thisWindow->LockMouseToWindow(true);
			}
		}break;
		case (WM_MOUSEMOVE): {
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = thisWindow->windowHandle;
			TrackMouseEvent(&tme);

			if (mouse) {
				Win32Mouse*realMouse = (Win32Mouse*)mouse;
				realMouse->UpdateWindowPosition(
					Vector2((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam))
				);
			}

			if(thisWindow->mouseLeftWindow) {
				thisWindow->mouseLeftWindow = false;
				if (thisWindow->init) {
					thisWindow->winMouse->Wake();
					thisWindow->winKeyboard->Wake();
				}
			}
		}break;
		case(WM_MOUSELEAVE):{
			thisWindow->mouseLeftWindow = true;
			if (thisWindow->init) {
				thisWindow->winMouse->Sleep();
				thisWindow->winKeyboard->Sleep();
			}
		}break;
		case(WM_SIZE): {
			float newX = (float)LOWORD(lParam);
			float newY = (float)HIWORD(lParam);
			if (newX > 0 && newY > 0 && (newX != thisWindow->size.x || newY != thisWindow->size.y)) {
				thisWindow->size.x = (float)LOWORD(lParam);
				thisWindow->size.y = (float)HIWORD(lParam);
			}
			if (wParam == SIZE_MAXIMIZED) {
				thisWindow->maximised = true;
				applyResize = true;
			}
			else if (wParam == SIZE_RESTORED && thisWindow->maximised) {
				thisWindow->maximised = false;
				applyResize = true;
			}
		}break;
		case(WM_ENTERSIZEMOVE): {
		}break;
		case(WM_EXITSIZEMOVE): {
			applyResize = true;
		}break;
    }

	if (applyResize) {
		thisWindow->ResizeRenderer();
		if (thisWindow->init) {
			thisWindow->winMouse->SetAbsolutePositionBounds(thisWindow->size);
			thisWindow->LockMouseToWindow(thisWindow->lockMouse);
		}
	}

    return DefWindowProc (hWnd, message, wParam, lParam);
}

void	Win32Window::LockMouseToWindow(bool lock)	{
	lockMouse = lock;
	if(lock) {
		RECT		windowRect;
		GetWindowRect (windowHandle, &windowRect);

		//windowRect.top -= 1; //adjust until something happens?!

		SetCapture(windowHandle);
		ClipCursor(&windowRect);
	}
	else{
		ReleaseCapture();
		ClipCursor(NULL);
	}
}

void	Win32Window::ShowOSPointer(bool show)	{
	if(show == showMouse) {
		return;	//ShowCursor does weird things, due to being a counter internally...
	}

	showMouse = show;
	if(show) {
		ShowCursor(1);
	}
	else{
		ShowCursor(0);
	}
}

void	Win32Window::SetConsolePosition(int x, int y)	{
	HWND consoleWindow = GetConsoleWindow();

	SetWindowPos(consoleWindow, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	SetActiveWindow(windowHandle);
}

void	Win32Window::SetWindowPosition(int x, int y) {
	SetWindowPos(windowHandle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	SetActiveWindow(windowHandle);
}

void	Win32Window::ShowConsole(bool state)				{
	HWND consoleWindow = GetConsoleWindow();

	ShowWindow(consoleWindow, state ? SW_RESTORE : SW_HIDE);

	SetActiveWindow(windowHandle);
}

#endif //_WIN32
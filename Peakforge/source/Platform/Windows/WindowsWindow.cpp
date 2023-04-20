#include "pfpch.h"
#include "WindowsWindow.h"

#include <atlbase.h>
#include <atlconv.h>

#define CLASS_NAME L"ApplicationWindowClass"

namespace PF 
{
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		bool success = Init(props);
		PF_CORE_ASSERT(success, "Could not initialize WinApi Window!");
	}
	
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}
	
	void WindowsWindow::OnUpdate()
	{
		MSG msg = {};
		if (GetMessage(&msg, NULL, 0, 0) > 0) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// TODO: need implement VSync
	void WindowsWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		#ifdef UNICODE
		std::wstring stemp = StrToWStr(props.Title);
		LPCWSTR title = stemp.c_str();
		#else
		LPCWSTR title = props.Title.c_str();
		#endif

		WNDCLASSEX wc;
		wc.cbClsExtra = NULL;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.cbWndExtra = NULL;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wc.hInstance = NULL;
		wc.lpszClassName = CLASS_NAME;
		wc.lpszMenuName = L"";
		wc.style = NULL;
		wc.lpfnWndProc = &WindowProc;


		if (!RegisterClassEx(&wc)) return false;

		m_Hwnd = CreateWindowEx(
			0,						// Optional window styles
			CLASS_NAME,				// Window class
			title,					// Window text
			WS_OVERLAPPEDWINDOW,	// Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, props.Width, props.Height,
			
			NULL, // Parent window 
			NULL, // Menu
			NULL, // Instance handle
			NULL // Additional application data
		);

		if (!m_Hwnd) return false;

		ShowWindow(m_Hwnd, SW_SHOW);
		UpdateWindow(m_Hwnd);

		LOG_CORE_INFO("Window {0} Initialized ({1}, {2})", props.Title, props.Width, props.Height);
		return true;
	}

	void WindowsWindow::Shutdown()
	{
		DestroyWindow(m_Hwnd);
	}

	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg) 
		{
		case WM_CREATE:
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	std::wstring WindowsWindow::StrToWStr(const std::string& str)
	{
		int len;
		int slength = (int)str.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}
}
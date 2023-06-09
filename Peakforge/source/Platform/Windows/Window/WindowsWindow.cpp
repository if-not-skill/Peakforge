#include "pfpch.h"
#include "WindowsWindow.h"

#include "Peakforge/Events/KeyEvent.h"
#include "Peakforge/Events/MouseEvent.h"
#include "Peakforge/Events/ApplicationEvent.h"

#include <windowsx.h>
#include <objbase.h>

#define CLASS_NAME L"ApplicationWindowClass"

// Forward declare message handler from imgui_impl_win32.cpp
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace PF 
{
	WindowsWindow* WindowsWindow::s_Instance = nullptr;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		PF_CORE_ASSERT(!s_Instance, "Window must be instance");
		s_Instance = this;

		const bool success = WindowsWindow::Init(props);
		PF_CORE_ASSERT(success, "Could not initialize WinApi Window!");
	}
	
	WindowsWindow::~WindowsWindow() 
	{
		Shutdown();
	}
	
	void WindowsWindow::OnUpdate()
	{
		MSG msg = {};
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				s_Instance->OnWindowDestroy();
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
		HRESULT hr = CoInitialize(NULL);
		PF_CORE_ASSERT(SUCCEEDED(hr), "Failed CoInitialized");

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		#ifdef UNICODE
		const std::wstring stemp = StrToWStr(props.Title);
		LPCWSTR title = stemp.c_str();
		#else
		LPCWSTR title = props.Title.c_str();
		#endif

		const HANDLE hIcon = LoadImage
		(
			nullptr,
			L"resources\\AppIcon.ico",
			IMAGE_ICON,
			NULL, NULL,
			LR_LOADFROMFILE
		);

		wc = 
		{
			sizeof(wc),
			CS_CLASSDC,
			& WindowProc,
			0L,
			0L,
			GetModuleHandle(nullptr),
			static_cast<HICON>(hIcon),
			nullptr,
			nullptr,
			nullptr,
			CLASS_NAME,
			nullptr
		};

		if (!RegisterClassExW(&wc)) return false;

		int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - props.Width / 2;
		int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - props.Height / 2;

		auto windowStyles = WS_OVERLAPPEDWINDOW;

		RECT wr;
		wr.left = centerScreenX;
		wr.top = centerScreenY;
		wr.right = wr.left + props.Width;
		wr.bottom = wr.top + props.Height;
		AdjustWindowRect(&wr, windowStyles, FALSE);

		m_Hwnd = ::CreateWindowW
		(
			wc.lpszClassName,
			title,
			windowStyles,
			wr.left, wr.top, 
			wr.right - wr.left, wr.bottom - wr.top,
			nullptr, nullptr, 
			wc.hInstance, 
			nullptr
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
		RegisterClassExW(&wc);

		LOG_CORE_INFO("WinAPI window Shitdowned");
	}

	void WindowsWindow::OnWindowSize(WPARAM wparam, LPARAM lparam)
	{
		UINT width = LOWORD(lparam);
		UINT height = HIWORD(lparam);

		OnWindowSize((LONG)width, (LONG)height);
	}

	void WindowsWindow::OnWindowSize(LONG width, LONG height)
	{
		m_Data.Width = width;
		m_Data.Height = height;

		WindowResizeEvent event(width, height);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnWindowDestroy()
	{
		WindowCloseEvent event;
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnAppActivate()
	{
		AppActivateEvent event;
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnAppDeactivate()
	{
		AppDeactivateEvent event;
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnAppSuspending()
	{
		AppSuspendingEvent event;
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnAppResuming()
	{
		AppResumingEvent event;
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnKeyHandler(UINT msg, WPARAM wparam, LPARAM lparam)
	{
		const UINT keycode = wparam;

		switch (msg) 
		{
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			OnKeyReleasedHandler(keycode);
			break;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			const bool repeats = (lparam & (1 << 30));
			OnKeyPressedHandler(keycode, repeats);
			break;
		}
		}
	}

	void WindowsWindow::OnKeyPressedHandler(UINT keycode, bool repeats)
	{
		

		KeyPressedEvent event(keycode, repeats);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnKeyReleasedHandler(UINT keycode)
	{
		

		KeyReleasedEvent event(keycode);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnKeyTypedHandler(WPARAM wparam, LPARAM lparam)
	{
		UINT keycode = wparam;

		KeyTypedEvent event(keycode);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnMouseButtonHandler(UINT msg, WPARAM wparam, LPARAM lparam)
	{
		UINT keycode = GetMouseKeyCode(msg, wparam);
		switch (msg) 
		{
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
		case WM_MBUTTONUP:
		{
			OnMouseButtonReleasedHandler(keycode);
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			OnMouseButtonPressedHandler(keycode);
			break;
		}
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		{
			OnMouseButtonDoubleClickHandler(keycode);
			break;
		}
		}
	}

	void WindowsWindow::OnMouseButtonPressedHandler(UINT keycode)
	{
		MouseButtonPressedEvent event(keycode);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnMouseButtonReleasedHandler(UINT keycode)
	{
		MouseButtonReleasedEvent event(keycode);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnMouseButtonDoubleClickHandler(UINT keycode)
	{
		MouseButtonDoubleClickEvent event(keycode);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnMouseWheelHandler(WPARAM wparam)
	{
		float zDelta = GET_WHEEL_DELTA_WPARAM(wparam);
		MouseScrolledEvent event(zDelta, zDelta);
		m_Data.EventCallback(event);
	}

	void WindowsWindow::OnMouseMoveHandler(LPARAM lparam)
	{
		auto xPos = GET_X_LPARAM(lparam);
		auto yPos = GET_Y_LPARAM(lparam);

		MouseMovedEvent event(xPos, yPos);
		m_Data.EventCallback(event);
	}

	UINT WindowsWindow::GetMouseKeyCode(UINT msg, WPARAM wparam)
	{
		UINT keycode;
		
		switch (msg)
		{
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			keycode = VK_LBUTTON;
			break;
		}

		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		{
			keycode = VK_RBUTTON;
			break;
		}

		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		{
			keycode = VK_MBUTTON;
			break;
		}

		case WM_XBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		{
			keycode = GET_XBUTTON_WPARAM(wparam);
			break;
		}
		}

		return keycode;
	}
	
	LRESULT WindowsWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return true;

		static bool s_InSizeMove = false;
		static bool s_InSuspend = false;
		static bool s_Minimized = false;
		static bool s_Fullscreen = false;
		// TODO: Set s_Fullscreen to true if defaulting to fullscreen.

		if (!s_Instance->SetupedCallback()) 
			return DefWindowProc(hwnd, msg, wparam, lparam);

		switch (msg)
		{
#pragma region MOUSE
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:

		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:

		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:

		case WM_XBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		{
			s_Instance->OnMouseButtonHandler(msg, wparam, lparam);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			s_Instance->OnMouseWheelHandler(wparam);
			break;
		}
		case WM_MOUSEMOVE:
			s_Instance->OnMouseMoveHandler(lparam);
			break;
#pragma endregion

#pragma region KEYBOARD
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			s_Instance->OnKeyHandler(msg, wparam, lparam);
			break;
		}
		case WM_CHAR:
		{
			s_Instance->OnKeyTypedHandler(wparam, lparam);
			break;
		}
#pragma endregion

#pragma region APPLICATION
		case WM_CREATE:
			break;
		case WM_SIZE:
		{
			if (wparam == SIZE_MINIMIZED)
			{
				if (!s_Minimized)
				{
					s_Minimized = true;
					if (!s_InSuspend)
						s_Instance->OnAppSuspending();
					s_InSuspend = true;
				}
			}
			else if (s_Minimized)
			{
				s_Minimized = false;
				if (s_InSuspend)
					s_Instance->OnAppResuming();
				s_InSuspend = false;
			}
			else if (!s_InSizeMove)
			{
				s_Instance->OnWindowSize(wparam, lparam);
			}
			break;
		}
		case WM_ENTERSIZEMOVE:
		{
			s_InSizeMove = true;
			break;
		}
		case WM_EXITSIZEMOVE:
		{
			s_InSizeMove = false;

			RECT rc;
			GetClientRect(hwnd, &rc);

			s_Instance->OnWindowSize(rc.right - rc.left, rc.bottom - rc.top);

			break;
		}
		case WM_ACTIVATEAPP:
		{
			if (wparam)
			{
				s_Instance->OnAppActivate();
			}
			else
			{
				s_Instance->OnAppDeactivate();
			}
			break;
		}
		case WM_POWERBROADCAST:
		{
			switch (wparam)
			{
			case PBT_APMQUERYSUSPEND:
				if (!s_InSuspend)
					s_Instance->OnAppSuspending();
				s_InSuspend = true;
				return TRUE;

			case PBT_APMRESUMESUSPEND:
				if (!s_Minimized)
				{
					if (s_InSuspend)
						s_Instance->OnAppResuming();
					s_InSuspend = false;
				}
				return TRUE;
			}
			break;
		}
		case WM_DESTROY:
			s_Instance->OnWindowDestroy();
			PostQuitMessage(0);
			return 0;
		}
#pragma endregion

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
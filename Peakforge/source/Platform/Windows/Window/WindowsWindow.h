#pragma once

#include "Peakforge/Core/Window.h"

namespace PF 
{
	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props);
		~WindowsWindow() override;

		virtual void OnUpdate() override;
		virtual void SetVSync(bool enabled) override;

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		inline HWND GetHWND() const { return m_Hwnd; }
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; };

		inline bool IsVSync() const override { return m_Data.VSync; }
	
		void* GetNativeWindow() const override { return s_Instance; };

	private:
		inline bool SetupedCallback() const { return m_Data.EventCallback != nullptr; }

		virtual bool Init(const WindowProps& props);
		virtual void Shutdown();

		// === Event Handlers =========================================================
		// Window Handlers
		void OnWindowSize(WPARAM wparam, LPARAM lparam);
		void OnWindowSize(LONG width, LONG height);
		void OnWindowDestroy();
		
		// App Handlers
		void OnAppActivate();
		void OnAppDeactivate();
		void OnAppSuspending();
		void OnAppResuming();

		// Keyboard Handlers
		void OnKeyHandler(UINT msg, WPARAM wparam, LPARAM lparam);
		void OnKeyPressedHandler(UINT keycode, bool repeate);
		void OnKeyReleasedHandler(UINT keycode);
		void OnKeyTypedHandler(WPARAM wparam, LPARAM lparam);
		
		// Mouse Handlers
		void OnMouseButtonHandler(UINT msg, WPARAM wparam, LPARAM lparam);
		void OnMouseButtonPressedHandler(UINT keycode);
		void OnMouseButtonReleasedHandler(UINT keycode);
		void OnMouseButtonDoubleClickHandler(UINT keycode);
		void OnMouseWheelHandler(WPARAM wparam);
		void OnMouseMoveHandler(LPARAM lparam);
		// ===========================================================================

		UINT GetMouseKeyCode(UINT msg, WPARAM wparam);

		static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	private:
		std::wstring StrToWStr(const std::string& str);

	private:
		static WindowsWindow* s_Instance;
		HWND m_Hwnd;

		struct WindowData 
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback = nullptr;
		};

		WindowData m_Data;
	};
}
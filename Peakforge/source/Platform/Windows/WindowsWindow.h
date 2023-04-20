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

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; };

		inline bool IsVSync() const override { return m_Data.VSync; }
	
	private:
		virtual bool Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		std::wstring StrToWStr(const std::string& str);

	private:
		HWND m_Hwnd;

		struct WindowData 
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
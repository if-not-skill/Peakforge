#include "pfpch.h"
#include "WindowsInput.h"

#include <Windows.h>

#include "Peakforge/Core/Application.h"
#include "Platform/Windows/Window/WindowsWindow.h"

namespace PF::Input
{
	Input* Input::s_Instance = new Windows::WindowsInput();

	namespace Windows

	{

		bool WindowsInput::IsKeyPressedImpl(KeyboardCode keycode)
		{
			return IsKeyDown(static_cast<int>(keycode));
		}

		bool WindowsInput::IsMouseButtonPressedImpl(MouseButtonCode button)
		{
			return IsKeyDown(static_cast<int>(button));
		}

		std::pair<float, float> WindowsInput::GetMousePositionImpl()
		{
			POINT pos;
			GetCursorPos(&pos);

			const auto window = static_cast<WindowsWindow*>(Application::Get().GetWindow().GetNativeWindow());
			ScreenToClient(window->GetHWND(), &pos);

			return { pos.x, pos.y };
		}

		float WindowsInput::GetMouseXImpl()
		{
			auto [x, y] = GetMousePositionImpl();
			return x;
		}

		float WindowsInput::GetMouseYImpl()
		{
			auto [x, y] = GetMousePositionImpl();
			return y;
		}

		bool WindowsInput::IsKeyDown(int keycode)
		{
			const SHORT keyState = GetKeyState(keycode);
			return keyState & 0x8000;
		}
	}
}

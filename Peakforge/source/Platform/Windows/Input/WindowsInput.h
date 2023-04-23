#pragma once

#include "Peakforge/Input/Input.h"

namespace PF::Input::Windows
{
	class WindowsInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(KeyboardCode keycode) override;
		bool IsMouseButtonPressedImpl(MouseButtonCode button) override;

		std::pair<float, float> GetMousePositionImpl() override;
		float GetMouseXImpl() override;
		float GetMouseYImpl() override;

	private:
		static bool IsKeyDown(int keycode);
	};

}
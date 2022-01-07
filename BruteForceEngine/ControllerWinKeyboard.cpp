#include "ControllerWinKeyboard.h"
#include "Windows.h"

namespace BruteForce
{
	namespace Controller
	{
		bool ControllerWinKey::GetKeyPressed(Keys key)
		{
			switch (key)
			{
			case Keys::MoveLeft:
				return GetKeyState(VK_LEFT) &0x80;
				break;

			case Keys::MoveRight:
				return GetKeyState(VK_RIGHT) & 0x80;
				break;
			case Keys::MoveForward:
				return GetKeyState(VK_UP) & 0x80;
				break;

			case Keys::MoveBack:
				return GetKeyState(VK_DOWN) & 0x80;
				break;

			case Keys::RotateLeft:
				return GetKeyState(VK_NUMPAD4) & 0x80;
				break;

			case Keys::RotateRight:
				return GetKeyState(VK_NUMPAD6) & 0x80;
				break;

			default:
				break;
			}
			return false;
		}

		float ControllerWinKey::GetAxeState(Axes axe)
		{
			return 0.0f;
		}
	}
}

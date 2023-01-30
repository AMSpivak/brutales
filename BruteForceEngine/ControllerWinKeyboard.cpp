#include "ControllerWinKeyboard.h"
#include "Windows.h"
#include <algorithm>

namespace BruteForce
{
	namespace Controller
	{
		bool ControllerWinKey::GetKeyPressed(Keys key)
		{
			switch (key)
			{
			case Keys::MoveLeft:
				return GetKeyState('A') & 0x8000;
				break;

			case Keys::MoveRight:
				return GetKeyState('D') & 0x8000;
				break;
			case Keys::MoveForward:
				return GetKeyState('W'/*VK_UP*/) & 0x8000;
				break;

			case Keys::MoveBack:
				return GetKeyState('S') & 0x8000;
				break;

			case Keys::MoveUp:
				return GetKeyState(VK_CONTROL) & 0x8000;
				break;
			case Keys::MoveDown:
				return GetKeyState(VK_SPACE) & 0x8000;
				break;

			case Keys::RotateLeft:
				return GetKeyState(VK_NUMPAD4) & 0x8000;
				break;

			case Keys::RotateRight:
				return GetKeyState(VK_NUMPAD6) & 0x8000;
				break;
			case Keys::DbgInrease:
				return GetKeyState(VK_ADD) & 0x8000;
				break;
			case Keys::DbgDecrease:
				return GetKeyState(VK_SUBTRACT) & 0x8000;
				break;
			case Keys::DbgSwitch1:
				return GetKeyState(VK_F5) & 0x8000;
				break;
			case Keys::DbgSwitch2:
				return GetKeyState(VK_F6) & 0x8000;
				break;
			case Keys::DbgSwitch3:
				return GetKeyState(VK_F7) & 0x8000;
				break;
			case Keys::DbgSwitch4:
				return GetKeyState(VK_F8) & 0x8000;
				break;

			default:
				break;
			}
			return false;
		}

		float ControllerWinKey::GetAxeState(Axes axe)
		{
			switch (axe)
			{
			case Axes::CameraHorizontal:
				return CameraX;
				break;
			case Axes::CameraVertical:
				return CameraY;
				break;
			default:
				break;
			}
			return 0.0f;
		}
		void ControllerWinKey::Update()
		{
			

			POINT mouse;
			GetCursorPos(&mouse);

			if (!camera_look)
			{
				LockX = mouse.x;
				LockY = mouse.y;
			}

			bool current_camera_look_key = GetKeyPressed(Keys::DbgSwitch1);
			if (current_camera_look_key && !camera_look_key)
			{
				camera_look = !camera_look;
			}
			camera_look_key = current_camera_look_key;

			//camera_look = GetKeyState(VK_RBUTTON) & 0x8000;
	
			if (camera_look)
			{
				float val = static_cast<float>(std::clamp<int>(mouse.x - LockX, -MouseSensetivity, MouseSensetivity));
				val /= MouseSensetivity;
				CameraX = val;
				val = static_cast<float>(std::clamp<int>(mouse.y - LockY, -MouseSensetivity, MouseSensetivity));
				val /= MouseSensetivity;
				CameraY = val;
				SetCursorPos(LockX, LockY);
			}
			else
			{
				CameraX = 0.0f;
				CameraY = 0.0f;
			}
			
		}
	}
}

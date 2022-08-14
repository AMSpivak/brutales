#ifndef _BRUTEFORCE_CONTROLLER_WIN_KEY_H
#define _BRUTEFORCE_CONTROLLER_WIN_KEY_H
#include "Controller.h"
namespace BruteForce
{
	namespace Controller
	{
		class ControllerWinKey : public Controller
		{
		private:
			bool camera_look;
			bool camera_look_key;
			int MouseSensetivity;
			int MouseX;
			int MouseY;
			int LockX;
			int LockY;
			float CameraX;
			float CameraY;

		public:
			ControllerWinKey() : camera_look{ false }, camera_look_key{false},
				MouseSensetivity{ 200 }
				, MouseX{ 0 }, MouseY{ 0 }
				, LockX{ 0 }, LockY{ 0 }
				, CameraX{ 0.0f }, CameraY{ 0.0f }
			{}
			bool GetKeyPressed(Keys key) override;
			float GetAxeState(Axes axe) override;
			void Update() override;
		};
	}
}

#endif

#ifndef _BRUTEFORCE_CONTROLLER_WIN_KEY_H
#define _BRUTEFORCE_CONTROLLER_WIN_KEY_H
#include "Controller.h"
namespace BruteForce
{
	namespace Controller
	{
		class ControllerWinKey : public Controller
		{
		public:
			bool GetKeyPressed(Keys key) override;
			float GetAxeState(Axes axe) override;
		};
	}
}

#endif

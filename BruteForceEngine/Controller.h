#ifndef _BRUTEFORCE_CONTROLLER_H
#define _BRUTEFORCE_CONTROLLER_H
namespace BruteForce
{
	namespace Controller
	{
		enum class  Keys
		{
			MoveForward = 0,
			MoveBack,
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,
			RotateLeft,
			RotateRight,
			DbgInrease,
			DbgDecrease
		};

		enum class Axes
		{
			CameraHorizontal = 0,
			CameraVertical,
		};

		class Controller
		{
		public:
			virtual bool GetKeyPressed(Keys key) = 0;
			virtual float GetAxeState(Axes axe) = 0;
			virtual void Update() = 0;
		};
	}
}

#endif

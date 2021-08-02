#ifndef VIDEO_DRIVER_DX12_H
#define VIDEO_DRIVER_DX12_H
#include "VideoDriverInterface.h"
namespace BruteForce
{
	class WindowDX12 : public Window
	{
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		HWND mhWnd;
		bool CheckTearingSupport();
		void Resize();
	public:
		WindowDX12(HWND hWnd) : mhWnd(hWnd) {};
		virtual void Show();
		virtual ~WindowDX12() {};
		virtual SwapChain CreateSwapChain(CommandQueue& commandQueue, uint32_t bufferCount);
		virtual SwapChain CreateSwapChain(SmartCommandQueue& commandQueue, uint32_t bufferCount);

	};

	class VideoDriverDX12 : public VideoDriverInterface
	{
	public:
		virtual Window * CreateWindow(const wchar_t* windowClassName, const wchar_t* windowTitle, uint32_t width, uint32_t height);
	};
}
#endif


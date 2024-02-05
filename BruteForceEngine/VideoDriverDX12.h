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
		bool m_IsFullscreen;
		RECT m_WinRect;
		bool CheckTearingSupport();
		void OnResize(UINT width, UINT height);
	public:
		WindowDX12(HWND hWnd) : mhWnd(hWnd), m_IsFullscreen(false) {};
		virtual void Show();
		virtual ~WindowDX12() {};
		virtual SwapChain CreateSwapChain(CommandQueue& commandQueue, uint32_t bufferCount, TargetFormat format );
		virtual SwapChain CreateSwapChain(SmartCommandQueue& commandQueue, uint32_t bufferCount, TargetFormat format);
		virtual void SetFullscreen(bool value);
		virtual bool IsOnHDRDisplay(Adapter& adapter);
		virtual bool SetHDRMode(HDRMode::HDRMode mode);
		virtual void SetHDRMetaData(HDRMode::HDRMode mode, float MaxOutputNits, float MinOutputNits, float MaxCLL, float MaxFALL);
	};

	class VideoDriverDX12 : public VideoDriverInterface
	{
	public:
		virtual Window * CreateWindow(const wchar_t* windowClassName, const wchar_t* windowTitle, uint32_t width, uint32_t height);
	};
}
#endif


#ifndef VIDEO_DRIVER_INTERFACE_H
#define VIDEO_DRIVER_INTERFACE_H

#include <functional>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

///////////////////////////////////
#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// D3D12 extension library.
#include "d3dx12.h"
///////////////////////////////////
#if defined(CreateWindow)
#undef CreateWindow
#endif
namespace RenderInterface
{	
	using SimpleFunctionPtr = std::function<void()>;
	class Window
	{
	protected:
		SimpleFunctionPtr funcOnPaint;
	public:
		Window(){};
		virtual ~Window() {};
		virtual void Show() = 0;
		virtual ComPtr<IDXGISwapChain4> CreateSwapChain(
			ComPtr<ID3D12CommandQueue> commandQueue,
			uint32_t width, uint32_t height, uint32_t bufferCount) = 0;
		void SetOnPaint(SimpleFunctionPtr func) { funcOnPaint = func;}
	};

	class VideoDriverInterface
	{
	public:
		virtual ~VideoDriverInterface() {}
		virtual Window * CreateWindow(const wchar_t* windowClassName, const wchar_t* windowTitle, uint32_t width, uint32_t height) = 0;
	};
}
#endif


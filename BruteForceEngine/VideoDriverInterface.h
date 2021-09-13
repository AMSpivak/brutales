#ifndef VIDEO_DRIVER_INTERFACE_H
#define VIDEO_DRIVER_INTERFACE_H

#include <functional>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

///////////////////////////////////
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"
///////////////////////////////////
#if defined(CreateWindow)
#undef CreateWindow
#endif
namespace BruteForce
{	
	class Window;
	using SimpleFunctionPtr = std::function<void()>;
	using ResizeFunctionPtr = std::function<void(uint32_t, uint32_t, Window *)>;
	class Window
	{
	protected:
		SimpleFunctionPtr funcOnPaint;
		ResizeFunctionPtr funcOnResize;
		SwapChain m_SwapChain;
		uint32_t Width;
		uint32_t Height;
		bool m_VSync;
		bool m_Tearing;
	public:
		Window(): Width(0), Height(0), m_VSync(true), m_Tearing(false){};
		virtual ~Window() {};
		virtual void Show() = 0;
		virtual SwapChain CreateSwapChain(CommandQueue& commandQueue, uint32_t bufferCount) = 0;
		virtual SwapChain CreateSwapChain(SmartCommandQueue& commandQueue, uint32_t bufferCount) = 0;
		void SetOnPaint(SimpleFunctionPtr func) { funcOnPaint = func; }
		void SetOnResize(ResizeFunctionPtr func) { funcOnResize = func; }
		void SetSize(uint32_t width, uint32_t height) { Width = width; uint32_t Height = height; }
		void SetVSync(bool value) { m_VSync = value; }
		bool GetVSync() { return m_VSync; }
		bool GetTearing() { return m_Tearing; }
		SwapChain& GetSwapChainReference() { return m_SwapChain; }
		uint32_t GetWidth() { return Width; }
		uint32_t GetHeight() { return Height; }

		virtual void SetFullscreen(bool value) = 0;
	};

	class VideoDriverInterface
	{
	public:
		virtual ~VideoDriverInterface() {}
		virtual Window * CreateWindow(const wchar_t* windowClassName, const wchar_t* windowTitle, uint32_t width, uint32_t height) = 0;
	};

	Adapter GetAdapter(bool useWarp);
	Device CreateDevice(Adapter adapter);
	DescriptorHeap CreateDescriptorHeap(Device device, DescriptorHeapType type, uint32_t numDescriptors);

	void EnableDebugLayer();
	void UpdateRenderTargetViews(Device device, SwapChain swapChain, DescriptorHeap descriptorHeap, Resource* BackBuffers, uint8_t NumFrames);

	//void WaitForFenceValue(Fence fence, uint64_t fenceValue, FenceEvent fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
}
#endif


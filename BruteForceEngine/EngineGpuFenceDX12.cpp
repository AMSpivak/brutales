//#include "VideoDriverDX12.h"
//#include <algorithm>
//#include <cassert>
//#include <cstdint>
//#include <chrono>
#include "EngineGpuFence.h"

#include "Helpers.h"

namespace BruteForce
{
	void WaitForFenceValue(Fence fence, uint64_t fenceValue, HANDLE fenceEvent,
		std::chrono::milliseconds duration)
	{
		if (fence->GetCompletedValue() < fenceValue)
		{
			ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
		}
	}

	uint64_t Signal(CommandQueue commandQueue, Fence fence,
		uint64_t& fenceValue)
	{
		uint64_t fenceValueForSignal = ++fenceValue;
		ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValueForSignal));

		return fenceValueForSignal;
	}

	Fence CreateFence(Device device)
	{
		Fence fence;

		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		return fence;
	}

	HANDLE CreateEventHandle()
	{
		HANDLE fenceEvent;

		fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent && "Failed to create fence event.");

		return fenceEvent;
	}

	void CloseEventHandle(EventHandle& handle)
	{
		::CloseHandle(handle);
	}


	SmartFence::SmartFence(Device device) : m_fence_value(0)
	{
		m_fence = CreateFence(device);
		m_handle = CreateEventHandle();
	}

	SmartFence::~SmartFence()
	{
		CloseEventHandle(m_handle);
	}

	uint64_t SmartFence::Signal(CommandQueue commandQueue)
	{
		return BruteForce::Signal(commandQueue, m_fence, m_fence_value);
	}

	bool SmartFence::IsCompleted()
	{
		return IsCompleted(m_fence_value);
	}

	bool SmartFence::IsCompleted(uint64_t fenceValue)
	{
		return m_fence->GetCompletedValue() >= fenceValue;
	}

	void SmartFence::WaitForFenceValue(uint64_t fenceValue, std::chrono::milliseconds duration)
	{
		BruteForce::WaitForFenceValue(m_fence, fenceValue, m_handle, duration);
	}
}
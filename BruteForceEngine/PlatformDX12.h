#ifndef PLATFORM_DX12_H
#define PLATFORM_DX12_H

#include <wrl.h>
#include <chrono>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <Windows.h>
// D3D12 extension library.
#include "d3dx12.h"

namespace BruteForce
{
	using Adapter = Microsoft::WRL::ComPtr<IDXGIAdapter4>;
	using Device = Microsoft::WRL::ComPtr<ID3D12Device2>;
	using CommandQueue = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;
	using WindowHandle = HWND;

	using SwapChain = Microsoft::WRL::ComPtr<IDXGISwapChain4>;
	using SwapChainDesc = DXGI_SWAP_CHAIN_DESC;

	using Resource = Microsoft::WRL::ComPtr<ID3D12Resource>;

	using GraphicsCommandList = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>;
	using CommandAllocator = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>;
	using CommandListType = D3D12_COMMAND_LIST_TYPE;
	constexpr CommandListType CommandListTypeDirect = D3D12_COMMAND_LIST_TYPE_DIRECT;

	using DescriptorHeap = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>;
	using Fence = Microsoft::WRL::ComPtr<ID3D12Fence>;
	using EventHandle = HANDLE;

	using DescriptorHeap = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>;
	using DescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE;
	constexpr DescriptorHeapType DescriptorHeapRTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	using KeyParameter = WPARAM;

	using FenceEvent = HANDLE;

}

#endif

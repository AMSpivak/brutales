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
	using pResource = ID3D12Resource*;
	using ResourceDesc = CD3DX12_RESOURCE_DESC;

	using GraphicsCommandList = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>;
	using GraphicsCommandList2 = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>;
	using CommandAllocator = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>;
	using CommandListType = D3D12_COMMAND_LIST_TYPE;
	constexpr CommandListType CommandListTypeDirect = D3D12_COMMAND_LIST_TYPE_DIRECT;
	constexpr CommandListType CommandListTypeCopy = D3D12_COMMAND_LIST_TYPE_COPY;
	using DescriptorHeap = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>;
	using DescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE;
	using Fence = Microsoft::WRL::ComPtr<ID3D12Fence>;
	using EventHandle = HANDLE;

	using DescriptorHeapDesc = D3D12_DESCRIPTOR_HEAP_DESC;

	using HeapProperties = CD3DX12_HEAP_PROPERTIES;
	using HeapFlags = D3D12_HEAP_FLAGS;
	constexpr HeapFlags HeapFlagsNone = D3D12_HEAP_FLAG_NONE;
	using HeapType = D3D12_HEAP_TYPE;
	constexpr HeapType HeapTypeDefault = D3D12_HEAP_TYPE_DEFAULT;


	using DescriptorHeapType = D3D12_DESCRIPTOR_HEAP_TYPE;
	constexpr DescriptorHeapType DescriptorHeapRTV = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	constexpr DescriptorHeapType DescriptorHeapDSV = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	using DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAGS;
	constexpr DescriptorHeapFlags DescriptorHeapFlagsNone = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	using VertexBufferView = D3D12_VERTEX_BUFFER_VIEW;
	using IndexBufferView = D3D12_INDEX_BUFFER_VIEW;

	using ResourceBarrier = CD3DX12_RESOURCE_BARRIER;
	using ResourceStates = D3D12_RESOURCE_STATES;

	constexpr ResourceStates ResourceStatesRenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET;
	constexpr ResourceStates ResourceStatesPresent = D3D12_RESOURCE_STATE_PRESENT;
	constexpr ResourceStates ResourceStateDepthWrite = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	using ResourceFlags = D3D12_RESOURCE_FLAGS;

	constexpr ResourceFlags ResourceFlagsNone = D3D12_RESOURCE_FLAG_NONE;
	constexpr ResourceFlags ResourceFlagsDepthStencil = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


	using TargetFormat = DXGI_FORMAT;
	constexpr TargetFormat TargetFormat_D32_Float = DXGI_FORMAT_D32_FLOAT;

	constexpr UINT AllowTearing = DXGI_PRESENT_ALLOW_TEARING;

	using RootSignature = Microsoft::WRL::ComPtr<ID3D12RootSignature>;
	using PipelineState = Microsoft::WRL::ComPtr<ID3D12PipelineState>;

	using Viewport = D3D12_VIEWPORT;
	using CreateViewport = CD3DX12_VIEWPORT;
	using ScissorRect = D3D12_RECT;

	using DataBlob = Microsoft::WRL::ComPtr<ID3DBlob>;

	using KeyParameter = WPARAM;

	using FenceEvent = HANDLE;

	namespace Math
	{
		using Matrix = DirectX::XMMATRIX;
		using Vec3Float = DirectX::XMFLOAT3;
	}
}

#endif

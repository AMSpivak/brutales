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
#include "DXMemoryAllocator\D3D12MemAlloc.h"

namespace BruteForce
{
	using Adapter = Microsoft::WRL::ComPtr<IDXGIAdapter4>;
	using Device = Microsoft::WRL::ComPtr<ID3D12Device2>;
	using GpuAllocator = Microsoft::WRL::ComPtr < D3D12MA::Allocator>;

	using CommandQueue = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;
	using WindowHandle = HWND;

	using SwapChain = Microsoft::WRL::ComPtr<IDXGISwapChain4>;
	using SwapChainDesc = DXGI_SWAP_CHAIN_DESC;

	using Resource = Microsoft::WRL::ComPtr<ID3D12Resource>;
	using pResource = ID3D12Resource*;
	using CResourceDesc = CD3DX12_RESOURCE_DESC;
	using ResourceDesc = D3D12_RESOURCE_DESC;
	using SubresourceData = D3D12_SUBRESOURCE_DATA;

	using GraphicsCommandList = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>;
	using GraphicsCommandList2 = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>;
	using CommandAllocator = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>;
	using CommandListType = D3D12_COMMAND_LIST_TYPE;
	constexpr CommandListType CommandListTypeDirect = D3D12_COMMAND_LIST_TYPE_DIRECT;
	constexpr CommandListType CommandListTypeCopy = D3D12_COMMAND_LIST_TYPE_COPY;
	constexpr CommandListType CommandListTypeCompute = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	using DescriptorHeap = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>;
	using pDescriptorHeap = ID3D12DescriptorHeap*;
	using CDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE;
	using DescriptorHandle = D3D12_CPU_DESCRIPTOR_HANDLE;
	using DescriptorHandleGpu = D3D12_GPU_DESCRIPTOR_HANDLE;
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
	constexpr DescriptorHeapType DescriptorHeapCvbSrvUav = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	constexpr DescriptorHeapType DescriptorHeapSampler = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

	using DescriptorHeapFlags = D3D12_DESCRIPTOR_HEAP_FLAGS;
	constexpr DescriptorHeapFlags DescriptorHeapFlagsNone = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	constexpr DescriptorHeapFlags DescriptorHeapShaderVisible = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	using DescriptorRange = CD3DX12_DESCRIPTOR_RANGE1;
	using DescriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE;
	constexpr DescriptorRangeType DescriptorRangeTypeSrv = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	constexpr DescriptorRangeType DescriptorRangeTypeUav = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	constexpr DescriptorRangeType DescriptorRangeTypeCvb = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	constexpr DescriptorRangeType DescriptorRangeTypeSampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

	using DescriptorUAV = D3D12_UNORDERED_ACCESS_VIEW_DESC;


	using VertexBufferView = D3D12_VERTEX_BUFFER_VIEW;
	using IndexBufferView = D3D12_INDEX_BUFFER_VIEW;

	using ResourceBarrier = CD3DX12_RESOURCE_BARRIER;
	using ResourceStates = D3D12_RESOURCE_STATES;

	constexpr ResourceStates ResourceStatesRenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET;
	constexpr ResourceStates ResourceStatesPresent = D3D12_RESOURCE_STATE_PRESENT;
	constexpr ResourceStates ResourceStateDepthWrite = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	constexpr ResourceStates ResourceStateCommon = D3D12_RESOURCE_STATE_COMMON;
	constexpr ResourceStates ResourceStateRead = D3D12_RESOURCE_STATE_GENERIC_READ;
	constexpr ResourceStates ResourceStateCopyDest = D3D12_RESOURCE_STATE_COPY_DEST;
	constexpr ResourceStates ResourceStatePixelShader = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	using ResourceFlags = D3D12_RESOURCE_FLAGS;

	constexpr ResourceFlags ResourceFlagsNone = D3D12_RESOURCE_FLAG_NONE;
	constexpr ResourceFlags ResourceFlagsDepthStencil = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	using SrvDesc = D3D12_SHADER_RESOURCE_VIEW_DESC;
	using SamplerDesc = D3D12_SAMPLER_DESC;


	using TargetFormat = DXGI_FORMAT;
	constexpr TargetFormat TargetFormat_Unknown = DXGI_FORMAT_UNKNOWN;
	constexpr TargetFormat TargetFormat_D32_Float = DXGI_FORMAT_D32_FLOAT;
	constexpr TargetFormat TargetFormat_R8G8B8A8_Unorm = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr TargetFormat TargetFormat_R8G8B8A8_Unorm_Srgb = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	constexpr TargetFormat TargetFormat_R8G8B8A8_UInt = DXGI_FORMAT_R8G8B8A8_UINT;
	constexpr TargetFormat TargetFormat_R16_UInt = DXGI_FORMAT_R16_UINT;
	constexpr TargetFormat TargetFormat_R16G16B16A16_Float = DXGI_FORMAT_R16G16B16A16_FLOAT;
	constexpr TargetFormat TargetFormat_R32G32B32_Float = DXGI_FORMAT_R32G32B32_FLOAT;


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
		constexpr float floatMax = D3D12_FLOAT32_MAX;
		using Matrix = DirectX::XMMATRIX;
		using Vec4Float = DirectX::XMFLOAT4;
		using Vec3Float = DirectX::XMFLOAT3;
		using Vec2Float = DirectX::XMFLOAT2;
		using Vec2Int = DirectX::XMINT2;
		using Vector = DirectX::XMVECTOR;

		inline void Store(Vec4Float* dst, const Vector& src )
		{
			DirectX::XMStoreFloat4(dst, src);
		}

		inline Vector VectorAdd(const Vector& a, const Vector& b)
		{
			return DirectX::XMVectorAdd(a,b);
		}
		inline Vector VectorDivide(const Vector& a, const Vector& b)
		{
			return DirectX::XMVectorDivide(a, b);
		}
		inline Vector VectorSet(float x, float y, float z, float w)
		{
			return DirectX::XMVectorSet(x, y, z, w);
		}
		inline Vector Vector3Norm(Vector vec)
		{
			return DirectX::XMVector3Normalize(vec);
		}

		inline Vector Vector3Lenght(Vector vec)
		{
			return DirectX::XMVector3Length(vec);
		}

		inline Matrix MatrixRotationAxis(const Vector &axe, float angle)
		{
			return DirectX::XMMatrixRotationAxis(axe, angle);
		}

		inline Vector MatrixVectorMul(const Matrix& m, const Vector &v)
		{
			return DirectX::XMVector4Transform(v, m);
		}
		inline Vector MatrixVectorScale(const Vector& v, float scale)
		{
			return DirectX::XMVectorScale(v, scale);
		}

		inline Vector MatrixVector3Cross(const Vector& v1, const Vector& v2)
		{
			return DirectX::XMVector3Cross(v1, v2);
		}

		inline Matrix MatrixRotationRollPitchYaw(const Vector &rpy)
		{
			return DirectX::XMMatrixRotationRollPitchYawFromVector(rpy);
		}

		inline Vector MatrixVectorMix(const Vector& a, const Vector& b, float a_part)
		{
			return VectorAdd(MatrixVectorScale(a, a_part), MatrixVectorScale(b, 1.0f - a_part));
		}

		inline Matrix MatrixTranslation(float OffsetX, float OffsetY, float OffsetZ)
		{
			return DirectX::XMMatrixTranslation(OffsetX, OffsetY, OffsetZ);
		}

		inline Matrix/*&&*/ Multiply(const Matrix& a, const Matrix& b)
		{
			return DirectX::XMMatrixMultiply(a, b);
		}
		inline Matrix/*&&*/ Inverse(const Matrix& m)
		{
			return DirectX::XMMatrixInverse(nullptr, m);
		}
		inline float DegToRad(float deg) { return DirectX::XMConvertToRadians(deg); }
		inline Matrix MatrixLookAtLH(const Vector& Eye, const  Vector& Focus, const Vector& Up) { return DirectX::XMMatrixLookAtLH(Eye, Focus, Up); }
		inline Matrix MatrixPerspectiveFovLH(float angel, float aspect, float near_plane, float far_plane) 
		{
			return DirectX::XMMatrixPerspectiveFovLH(angel, aspect, near_plane, far_plane);
		}
	}
}

#endif

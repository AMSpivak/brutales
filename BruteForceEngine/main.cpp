#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h> // For CommandLineToArgvW

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include "VideoDriverDX12.h"
#include "EngineGpuFence.h"
#include "EngineGpuCommands.h"
// In order to define a function called CreateWindow, the Windows macro needs to
// be undefined.
#if defined(CreateWindow)
#undef CreateWindow
#endif

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// D3D12 extension library.
#include "d3dx12.h"
// STL Headers
#include <algorithm>
#include <cassert>
#include <chrono>
// Helper functions
#include "Helpers.h"
// The number of swap chain back buffers.
const uint8_t g_NumFrames = 3;
// Use WARP adapter
bool g_UseWarp = false;

uint32_t g_ClientWidth = 1280;
uint32_t g_ClientHeight = 720;


// Window handle.
BruteForce::Window * pWindow;
// Window rectangle (used to toggle fullscreen state).
//RECT g_WindowRect;

// DirectX 12 Objects
BruteForce::Device g_Device;
BruteForce::SwapChain g_SwapChain;
BruteForce::Resource g_BackBuffers[g_NumFrames];
BruteForce::DescriptorHeap g_RTVDescriptorHeap;
UINT g_RTVDescriptorSize;
UINT g_CurrentBackBufferIndex;
uint64_t g_FrameFenceValues[g_NumFrames] = {};

// By default, enable V-Sync.
// Can be toggled with the V key.
bool g_VSync = true;
bool g_TearingSupported = false;
// By default, use windowed mode.
// Can be toggled with the Alt+Enter or F11
bool g_Fullscreen = false;


void ParseCommandLineArguments()
{
    int argc;
    wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

    for (decltype(argc) i = 0; i < argc; ++i)
    {
        if (::wcscmp(argv[i], L"-w") == 0 || ::wcscmp(argv[i], L"--width") == 0)
        {
            g_ClientWidth = ::wcstol(argv[++i], nullptr, 10);
        }
        if (::wcscmp(argv[i], L"-h") == 0 || ::wcscmp(argv[i], L"--height") == 0)
        {
            g_ClientHeight = ::wcstol(argv[++i], nullptr, 10);
        }
        if (::wcscmp(argv[i], L"-warp") == 0 || ::wcscmp(argv[i], L"--warp") == 0)
        {
            g_UseWarp = true;
        }
    }

    // Free memory allocated by CommandLineToArgvW
    ::LocalFree(argv);
}



void Update()
{
    static uint64_t frameCounter = 0;
    static double elapsedSeconds = 0.0;
    static std::chrono::high_resolution_clock clock;
    static auto t0 = clock.now();

    frameCounter++;
    auto t1 = clock.now();
    auto deltaTime = t1 - t0;
    t0 = t1;
    elapsedSeconds += deltaTime.count() * 1e-9;
    if (elapsedSeconds > 1.0)
    {
        char buffer[500];
        auto fps = frameCounter / elapsedSeconds;
        sprintf_s(buffer, 500, "FPS: %f\n", fps);
        OutputDebugStringA(buffer);
        frameCounter = 0;
        elapsedSeconds = 0.0;
    }
}

void Render(BruteForce::SmartCommandQueue& g_SmartCommandQueue)
{
    auto smart_command_list = g_SmartCommandQueue.GetCommandList();
    auto& backBuffer = g_BackBuffers[g_CurrentBackBufferIndex];

    // Clear the render target.
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        smart_command_list.command_list->ResourceBarrier(1, &barrier);
        FLOAT clearColor[] = { 1.0f, 0.6f, 0.1f, 1.0f };
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            g_CurrentBackBufferIndex, g_RTVDescriptorSize);

        smart_command_list.command_list->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    }
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        smart_command_list.command_list->ResourceBarrier(1, &barrier);

        g_FrameFenceValues[g_CurrentBackBufferIndex] = g_SmartCommandQueue.ExecuteCommandList(smart_command_list);
        UINT syncInterval = g_VSync ? 1 : 0;
        UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
        ThrowIfFailed(g_SwapChain->Present(syncInterval, presentFlags));

        g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

        g_SmartCommandQueue.WaitForFenceValue(g_FrameFenceValues[g_CurrentBackBufferIndex]);
    }
}

void Resize(uint32_t width, uint32_t height, BruteForce::SmartCommandQueue& g_SmartCommandQueue)
{
    if (g_ClientWidth != width || g_ClientHeight != height)
    {
        // Don't allow 0 size swap chain back buffers.
        g_ClientWidth = std::max(1u, width);
        g_ClientHeight = std::max(1u, height);

        // Flush the GPU queue to make sure the swap chain's back buffers
        // are not being referenced by an in-flight command list.
        g_SmartCommandQueue.Flush();

        for (int i = 0; i < g_NumFrames; ++i)
        {
            // Any references to the back buffers must be released
            // before the swap chain can be resized.
            g_BackBuffers[i].Reset();
            g_FrameFenceValues[i] = g_FrameFenceValues[g_CurrentBackBufferIndex];
        }

        BruteForce::SwapChainDesc swapChainDesc = {};
        ThrowIfFailed(g_SwapChain->GetDesc(&swapChainDesc));
        ThrowIfFailed(g_SwapChain->ResizeBuffers(g_NumFrames, g_ClientWidth, g_ClientHeight,
            swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

        g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();
        BruteForce::UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap, g_BackBuffers, g_NumFrames);
    }
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    

    ParseCommandLineArguments();
    BruteForce::EnableDebugLayer();
    BruteForce::VideoDriverDX12 m_driver;
    pWindow = m_driver.CreateWindow(L"DX12WindowClass", L"Learning DirectX 12",
                                        g_ClientWidth, g_ClientHeight);

    //::GetWindowRect(g_hWnd, &g_WindowRect);

    BruteForce::Adapter dxgiAdapter4 = BruteForce::GetAdapter(g_UseWarp);

    g_Device = BruteForce::CreateDevice(dxgiAdapter4);

    BruteForce::SmartCommandQueue g_SmartCommandQueue(g_Device, BruteForce::CommandListTypeDirect);

    g_SwapChain = pWindow->CreateSwapChain(g_SmartCommandQueue, g_NumFrames);

    g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

    g_RTVDescriptorHeap = BruteForce::CreateDescriptorHeap(g_Device, BruteForce::DescriptorHeapRTV, g_NumFrames);
    g_RTVDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);

    BruteForce::UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap, g_BackBuffers, g_NumFrames);


    pWindow->SetOnPaint([&g_SmartCommandQueue] {Update(); Render(g_SmartCommandQueue); });
    pWindow->SetOnResize([&g_SmartCommandQueue](uint32_t width, uint32_t height) {Resize(width, height, g_SmartCommandQueue); });
    pWindow->Show();
    pWindow->SetFullscreen(true);
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    g_SmartCommandQueue.Flush();
    return 0;
}
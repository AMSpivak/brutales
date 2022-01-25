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
#include "tutorialRenderer.h"
#include "Camera.h"
#include "ControllerWinKeyboard.h"

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
BruteForce::Controller::Controller* test_controller;
TutorialRenderer* p_Renderer = nullptr;
BruteForce::Camera* test_camera = nullptr;
// By default, use windowed mode.
// Can be toggled with the Alt+Enter or F11


void ParseCommandLineArguments()
{
    int argc;
    wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    OutputDebugStringW(argv[0]);

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
    float msecs = 0.000001f * deltaTime.count();


    if (test_controller && test_camera)
    {
        test_controller->Update();

        test_camera->RotateView(test_controller->GetAxeState(BruteForce::Controller::Axes::CameraVertical) * (1.0f),
            test_controller->GetAxeState(BruteForce::Controller::Axes::CameraHorizontal) * (1.0f),
            0.0f
        );
        //test_camera->RotateView({ 0,1,0,0 }, test_controller->GetAxeState(BruteForce::Controller::Axes::CameraHorizontal) * (-10.0f));
        //test_camera->RotateView({ 1,0,0,0 }, test_controller->GetAxeState(BruteForce::Controller::Axes::CameraVertical)*(-10.0f));
        //if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::RotateRight))
        //{
        //    test_camera->RotateView({ 0,1,0,0 }, -0.03f * msecs);
        //}
        //if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::RotateLeft))
        //{
        //    test_camera->RotateView({ 0,1,0,0 }, 0.03f * msecs);
        //}

        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveRight))
        {
            test_camera->MoveView(0.02f * msecs, 0.f, 0.f);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveLeft))
        {
            test_camera->MoveView(-0.02f * msecs, 0.f, 0.f);
        }

        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveForward))
        {
            test_camera->MoveView( 0.f, 0.f, 0.02f * msecs);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveBack))
        {
            test_camera->MoveView( 0.f, 0.f, -0.02f * msecs);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveUp))
        {
            test_camera->MoveView(0.f,  0.02f * msecs, 0.f);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveDown))
        {
            test_camera->MoveView(0.f, -0.02f * msecs, 0.f);
        }

    }
    

    p_Renderer->Update(msecs);

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

void Render(BruteForce::SmartCommandQueue& in_SmartCommandQueue, BruteForce::Window* pWindow)
{
    //auto smart_command_list = in_SmartCommandQueue.GetCommandList();
    //auto& backBuffer = p_Renderer->GetCurrentBackBufferRef();
    {
        auto smart_command_list = in_SmartCommandQueue.GetCommandList();
        p_Renderer->StartFrame(smart_command_list);
        p_Renderer->SetCurrentFence(in_SmartCommandQueue.ExecuteCommandList(smart_command_list));
    }

     p_Renderer->Render(in_SmartCommandQueue);

    {
        auto smart_command_list = in_SmartCommandQueue.GetCommandList();
        p_Renderer->PrepareSwapFrame(smart_command_list);
        p_Renderer->SetCurrentFence(in_SmartCommandQueue.ExecuteCommandList(smart_command_list));
    }
    p_Renderer->SwapFrame();
    p_Renderer->WaitForCurrentFence(in_SmartCommandQueue);
}

void Resize(uint32_t width, uint32_t height, BruteForce::SmartCommandQueue& in_SmartCommandQueue, BruteForce::Window * pWindow)
{
    if (g_ClientWidth != width || g_ClientHeight != height)
    {
        // Don't allow 0 size swap chain back buffers.
        g_ClientWidth = std::max(1u, width);
        g_ClientHeight = std::max(1u, height);

        // Flush the GPU queue to make sure the swap chain's back buffers
        // are not being referenced by an in-flight command list.
        in_SmartCommandQueue.Flush();

        for (int i = 0; i < p_Renderer->GetBuffersCount(); ++i)
        {
            // Any references to the back buffers must be released
            // before the swap chain can be resized.
            p_Renderer->m_BackBuffers[i].Reset();
            p_Renderer->m_FrameFenceValues[i] = p_Renderer->m_FrameFenceValues[p_Renderer->m_CurrentBackBufferIndex];
        }

        auto& refSwapChain = pWindow->GetSwapChainReference();

        BruteForce::SwapChainDesc swapChainDesc = {};
        ThrowIfFailed(refSwapChain->GetDesc(&swapChainDesc));
        ThrowIfFailed(refSwapChain->ResizeBuffers(p_Renderer->GetBuffersCount(), g_ClientWidth, g_ClientHeight,
            swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

        p_Renderer->m_CurrentBackBufferIndex = refSwapChain->GetCurrentBackBufferIndex();
        BruteForce::UpdateRenderTargetViews(g_Device, refSwapChain, p_Renderer->m_BackBuffersDHeap, p_Renderer->m_BackBuffers, p_Renderer->GetBuffersCount());
        p_Renderer->Resize(g_Device);
    }
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    ParseCommandLineArguments();
    BruteForce::EnableDebugLayer();
    //::GetWindowRect(g_hWnd, &g_WindowRect);

    BruteForce::Adapter adapter = BruteForce::GetAdapter(g_UseWarp);
    g_Device = BruteForce::CreateDevice(adapter);
    //BruteForce::ReportLiveObjects(g_Device);
    
    BruteForce::VideoDriverDX12 m_driver;
    pWindow = m_driver.CreateWindow(L"DX12WindowClass", L"Learning DirectX 12",
        g_ClientWidth, g_ClientHeight);
    
    p_Renderer = new TutorialRenderer(g_Device, pWindow, false);
    p_Renderer->LoadContent(g_Device);
    test_controller = new BruteForce::Controller::ControllerWinKey();
    test_camera = p_Renderer->GetCameraPtr();
    //pWindow->SetOnPaint([] {Update(); Render(p_Renderer->m_SmartCommandQueue, pWindow); });
    pWindow->SetOnResize([](uint32_t width, uint32_t height, BruteForce::Window * pwindow) {Resize(width, height, p_Renderer->m_SmartCommandQueue, pwindow); });
    pWindow->Show();
    //pWindow->SetFullscreen(true);
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        Update(); 
        Render(p_Renderer->m_SmartCommandQueue, pWindow);
    }
    delete(test_controller);
    delete(p_Renderer);
    delete(pWindow);
    BruteForce::ReportLiveObjects(g_Device);
    return 0;
}
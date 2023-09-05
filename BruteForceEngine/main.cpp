#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>
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
#include <cmath>
// Helper functions
#include "Helpers.h"
#include "tutorialRenderer.h"
#include "DescriptorHeapManager.h"
#include "Camera.h"
#include "ControllerWinKeyboard.h"
#include "Settings.h"
#include "GameEnvironment.h"
#include "BruteForceMath.h"
#include "SkyAstronomy.h"
#include "GameEnvironment.h"
//threads
#include <thread>
#include <mutex>

std::mutex g_update_lock;
bool g_DoRender;
bool g_Destroy;
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
BruteForce::DescriptorHeapManager* p_HeapManager = nullptr;
TutorialRenderer* p_Renderer = nullptr;
BruteForce::Camera* test_camera = nullptr;
// By default, use windowed mode.
// Can be toggled with the Alt+Enter or F11


void ParseCommandLineArguments()
{
    int argc;
    wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    std::filesystem::path exe_path = argv[0];

    auto& settings = BruteForce::GetSettings();
    settings.SetExecuteDir(exe_path.remove_filename());
    OutputDebugStringW(settings.GetExecuteDirWchar());
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
    static auto t_up = clock.now();

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

		//const float move_speed = 0.02f;
		const float move_speed = 0.004f;

        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveRight))
        {
            test_camera->MoveView(move_speed * msecs, 0.f, 0.f);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveLeft))
        {
            test_camera->MoveView(-move_speed * msecs, 0.f, 0.f);
        }

        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveForward))
        {
            test_camera->MoveView( 0.f, 0.f, move_speed * msecs);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveBack))
        {
            test_camera->MoveView( 0.f, 0.f, -move_speed * msecs);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveUp))
        {
            test_camera->MoveView(0.f, move_speed * msecs, 0.f);
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::MoveDown))
        {
            test_camera->MoveView(0.f, -move_speed * msecs, 0.f);
        }


        static bool fullscreen = false;
        {
            static bool fullscreen_switch = false;
            bool press = test_controller->GetKeyPressed(BruteForce::Controller::Keys::DbgSwitch2);
            if (press && (!fullscreen_switch))
            {
                fullscreen = !fullscreen;
                if (pWindow)
                {
                    pWindow->SetFullscreen(fullscreen);
                }
            }
            fullscreen_switch = press;
        }

        static float day_hour = 0.77f;
        static bool chng = true;
        static bool do_day_cycle = false;

        {
            static bool do_day_cycle_switch = false;

            bool press = test_controller->GetKeyPressed(BruteForce::Controller::Keys::DbgSwitch3);

            if (press && (!do_day_cycle_switch))
            {
                do_day_cycle = !do_day_cycle;
            }
            do_day_cycle_switch = press;
        }

        {
            static bool do_eye_adapt_switch = false;

            bool press = test_controller->GetKeyPressed(BruteForce::Controller::Keys::DbgSwitch4);

            if (press && (!do_eye_adapt_switch))
            {
                auto& game_info = BruteForce::GlobalLevelInfo::GetGlobalGameCameraInfo();
                game_info.m_EyeAdapt = !game_info.m_EyeAdapt;
            }
            do_eye_adapt_switch = press;
        }


        float msecs_up = (0.000001f * (t1 - t_up).count());
        if (msecs_up > 12)
        {
            t_up = t1;

            if(do_day_cycle)
            {
                day_hour += msecs * 0.000001f;
                chng = true;
            }
        }



        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::DbgInrease))
        {
            day_hour += msecs * 0.0001f;
            chng = true;
        }
        if (test_controller->GetKeyPressed(BruteForce::Controller::Keys::DbgDecrease))
        {
            day_hour -= msecs * 0.0001f;
            chng = true;
        }
        if (chng)
        {
            //day_hour = day_hour < 0.0f ? 0.0f : day_hour > 1.0f ? 1.0f : day_hour;

            static int day_of_year = 170;
            if (day_hour < 0.0f)
            {
                --day_of_year;
                day_hour += 1.0f;
            }

            if (day_hour > 0.0f)
            {
                ++day_of_year;
                day_hour -= 1.0f;
            }

            //constexpr int day_of_year = 170;
            constexpr float latitude = 50.f;
            constexpr float moon_sun_latitude = 180.f;

            SkyInfo Sky = GetSkyInfo(day_of_year, day_hour, latitude, moon_sun_latitude);
            BruteForce::Math::Vec4Float sun;
            BruteForce::Math::Store(&sun, Sky.SunDirection);
            BruteForce::Math::Vec4Float moon;
            BruteForce::Math::Store(&moon, Sky.MoonDirection);


            float sun_h = sun.y < 0.0f ? 0.0f : sun.y;

            BruteForce::Math::Vec4Float shadow_dir;
            BruteForce::Math::Store(&shadow_dir, BruteForce::Math::Vector3Norm({ sun.x, 0.0f, sun.z, 0.0f }));

            float sun_intencivity = 50000.0f;
            BruteForce::Math::Vector day_light = { 1.0f, 1.0f, 1.0f,sun_intencivity };
  

            auto& atmosphere = BruteForce::GlobalLevelInfo::GetGlobalAtmosphereInfo();
            constexpr float offset = 0.0001f;
            const float shadow_angle_delta = BruteForce::Math::DegToRad(10.0f);

            float shadow_tg_1 = sun_h /sqrt(1.0f - sun_h * sun_h + offset);
            float a2 = acos(sun_h) - shadow_angle_delta;
            a2 = a2 < 0.0f ? 0.0f : a2;
            float shadow_tg_2 = 1.f/(tan(a2) + offset);

            
            float moon_intencivity = 0.25f;// *0.5f;

            atmosphere.m_SunInfo = {
                sun.x,
                sun.y,
                sun.z,
                sun_intencivity };

            atmosphere.m_MoonInfo = {
                moon.x,
                moon.y,
                moon.z,
                moon_intencivity };

            atmosphere.m_MoonColor = {
                0.037f, 0.23f, 3.3f,
                moon_intencivity };

            atmosphere.m_SunShadow.x = shadow_tg_1;
            atmosphere.m_SunShadow.y = shadow_tg_2;
            atmosphere.m_SunShadow.z = -shadow_dir.x;
            atmosphere.m_SunShadow.w = -shadow_dir.z;

            BruteForce::Math::Store(&(atmosphere.m_SunColor), day_light);

			// Moon

            float moon_h = moon.y < 0.0f ? 0.0f : moon.y;
			shadow_tg_1 = moon_h / sqrt(1.0f - moon_h * moon_h + offset);
			a2 = acos(moon_h) - shadow_angle_delta;
			a2 = a2 < 0.0f ? 0.0f : a2;
			shadow_tg_2 = 1.f / (tan(a2) + offset);
            BruteForce::Math::Store(&shadow_dir, BruteForce::Math::Vector3Norm({ moon.x, 0.0f, moon.z, 0.0f }));

			atmosphere.m_MoonShadow.x = shadow_tg_1;
			atmosphere.m_MoonShadow.y = shadow_tg_2;
			atmosphere.m_MoonShadow.z = -shadow_dir.x;
			atmosphere.m_MoonShadow.w = -shadow_dir.z;
            atmosphere.m_MoonShadowScaler = abs(atmosphere.m_MoonShadow.z) + abs(atmosphere.m_MoonShadow.w);

            chng = false;
        }
    }
    

    p_Renderer->Update(msecs, p_Renderer->m_ComputeSmartCommandQueue);

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

	//p_Renderer->WaitForCurrentFence(in_SmartCommandQueue);
	p_Renderer->WaitForSwapReadyFence(in_SmartCommandQueue);
    
    p_Renderer->SwapFrame();

    //auto smart_command_list = in_SmartCommandQueue.GetCommandList();
    //auto& backBuffer = p_Renderer->GetCurrentBackBufferRef();
    {
        auto smart_command_list = in_SmartCommandQueue.GetCommandList();
        p_Renderer->StartFrame(smart_command_list);
        p_Renderer->SetCurrentFenceValue(in_SmartCommandQueue.ExecuteCommandList(smart_command_list));
    }

     p_Renderer->Render(in_SmartCommandQueue);

    {
		// p_Renderer->WaitForCurrentFence(in_SmartCommandQueue);

        auto smart_command_list = in_SmartCommandQueue.GetCommandList();
        p_Renderer->PrepareSwapFrame(smart_command_list);
        p_Renderer->SetCurrentFenceValue(in_SmartCommandQueue.ExecuteCommandList(smart_command_list));
        //p_Renderer->WaitForCurrentFence(in_SmartCommandQueue);

    }
    //p_Renderer->SwapFrame();
    //p_Renderer->WaitForCurrentFence(in_SmartCommandQueue);
}

void Resize(uint32_t width, uint32_t height, BruteForce::Window * pWindow)
{
    g_update_lock.lock();
    p_Renderer->Resize();
    g_update_lock.unlock();
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    ParseCommandLineArguments();
    BruteForce::EnableDebugLayer();
    //::GetWindowRect(g_hWnd, &g_WindowRect);

    BruteForce::Adapter adapter = BruteForce::GetAdapter(g_UseWarp);
    g_Device = BruteForce::CreateDevice(adapter);
    //BruteForce::GpuAllocator gpu_allocator = BruteForce::CreateGpuAllocator(adapter, g_Device);
    //BruteForce::ReportLiveObjects(g_Device);
    
    BruteForce::VideoDriverDX12 m_driver;
    pWindow = m_driver.CreateWindow(L"DX12WindowClass", L"Learning DirectX 12",
        g_ClientWidth, g_ClientHeight);
    //pWindow->SetFullscreen(true);
    p_HeapManager = new BruteForce::DescriptorHeapManager();
    p_Renderer = new TutorialRenderer(g_Device, adapter, pWindow, false, BruteForce::TargetFormat_R8G8B8A8_Unorm,*p_HeapManager);

    p_Renderer->LoadContent(g_Device);
    test_controller = new BruteForce::Controller::ControllerWinKey();
    test_camera = p_Renderer->GetCameraPtr();
    test_camera->SetPosition(BruteForce::Math::Vec3Float(0, 30, 0), true);
    //pWindow->SetOnPaint([] {Update(); Render(p_Renderer->m_SmartCommandQueue, pWindow); });
    pWindow->SetOnResize(Resize);
    pWindow->Show();

    g_Destroy = false;
    g_DoRender = true;

    auto render_func = [&] {while (g_DoRender) {
        
            Update();
            g_update_lock.lock();
            Render(p_Renderer->m_SmartCommandQueue, pWindow);
            g_update_lock.unlock();
        }
        g_Destroy = true;
    };

    std::thread render_thread(render_func);
    render_thread.detach();
    //pWindow->SetFullscreen(true);
    MSG msg = {};
  //  while (msg.message != WM_QUIT)
  //  {
		//while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)&&(msg.message != WM_QUIT))
  //      {
  //          ::TranslateMessage(&msg);
  //          ::DispatchMessage(&msg);
  //      }
  //      Sleep(10);
  //      /*if(msg.message != WM_)
  //      {
  //      }
  //      else*/
  //      /*{
  //          Update();
  //          Render(p_Renderer->m_SmartCommandQueue, pWindow);
  //      }*/
  //  }

    BOOL bRet;

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    g_DoRender = false;

    while (!g_Destroy)
    {
        Sleep(100);
    }

    delete(test_controller);
    delete(p_Renderer);
    delete(p_HeapManager);
    delete(pWindow);

    //BruteForce::ReportLiveObjects(g_Device);
    return 0;
}
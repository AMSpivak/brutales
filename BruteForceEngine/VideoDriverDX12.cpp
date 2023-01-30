#include "VideoDriverDX12.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include "Helpers.h"
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.

using namespace Microsoft::WRL;


// D3D12 extension library.
#include "d3dx12.h"
#include "EngineGpuSwapChain.h"

#include "dxgidebug.h"

namespace BruteForce
{
    inline int ComputeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
    {
        return std::max(0, std::min(ax2, bx2) - std::max(ax1, bx1)) * std::max(0, std::min(ay2, by2) - std::max(ay1, by1));
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        WindowDX12* pWindow = (WindowDX12*)GetWindowLongPtr(hwnd, 0);
        {
            switch (message)
            {
            case WM_PAINT:
                if (pWindow && pWindow->funcOnPaint) {
                    pWindow->funcOnPaint();
                }
                break;
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
            {
                //bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

                switch (wParam)
                {
                //case 'V':
                //    //g_VSync = !g_VSync;
                //    break;
                case VK_ESCAPE:
                    ::PostQuitMessage(0);
                    break;
                case VK_RETURN:
                case VK_F11:
                    //SetFullscreen(!g_Fullscreen);
                    break;

                }
            }
            break;
            // The default window procedure will play a system notification sound 
            // when pressing the Alt+Enter keyboard combination if this message is 
            // not handled.
            case WM_SYSCHAR:
                break;
            case WM_SIZE:
            {
                if (pWindow) {
                    pWindow->Resize();
                }
            }
            break;
            case WM_DESTROY:
                ::PostQuitMessage(0);
                break;
            default:
                return ::DefWindowProcW(hwnd, message, wParam, lParam);
            }
        }
        /*
        else
        {
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
        }*/

        return 0;
    }

    void RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName)
    {
        // Register a window class for creating our render window with.
        WNDCLASSEXW windowClass = {};

        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = &WndProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = sizeof(WindowDX12*);
        windowClass.hInstance = hInst;
        windowClass.hIcon = ::LoadIcon(hInst, NULL);
        windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = windowClassName;
        windowClass.hIconSm = ::LoadIcon(hInst, NULL);

        static ATOM atom = ::RegisterClassExW(&windowClass);
        assert(atom > 0);
    }


    Window* VideoDriverDX12::CreateWindow(const wchar_t* windowClassName, const wchar_t* windowTitle, uint32_t width, uint32_t height)
    {
        // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
     // Using this awareness context allows the client area of the window 
     // to achieve 100% scaling while still allowing non-client window content to 
     // be rendered in a DPI sensitive fashion.
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        HINSTANCE	hInst = GetModuleHandle(NULL);

        RegisterWindowClass(hInst, windowClassName);

        int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

        RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;

        // Center the window within the screen. Clamp to 0, 0 for the top-left corner.
        int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
        int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

        HWND hWnd = ::CreateWindowExW(
            NULL,
            windowClassName,
            windowTitle,
            WS_OVERLAPPEDWINDOW,
            windowX,
            windowY,
            windowWidth,
            windowHeight,
            NULL,
            NULL,
            hInst,
            nullptr
        );

        assert(hWnd && "Failed to create window");
        auto ret_value = new WindowDX12(hWnd);
        SetWindowLongPtr(hWnd, 0, (LONG_PTR)ret_value);
        ret_value->SetSize(static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight));
        return ret_value;
    }

    void WindowDX12::Show() {
        ::ShowWindow(mhWnd, SW_SHOW);
    }

    void WindowDX12::Resize()
    {
        if (funcOnResize) {
            RECT clientRect = {};
            ::GetClientRect(mhWnd, &clientRect);

            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            SetSize(width, height);
            funcOnResize(width, height, this);
        }
        ::GetWindowRect(mhWnd, &m_WinRect);
    }
    void WindowDX12::SetFullscreen(bool value)
    {
        if (m_IsFullscreen != value)
        {
            m_IsFullscreen = value;

            if (m_IsFullscreen) // Switching to fullscreen.
            {
                ::GetWindowRect(mhWnd, &m_WinRect);

                UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
                ::SetWindowLongW(mhWnd, GWL_STYLE, windowStyle);
                HMONITOR hMonitor = ::MonitorFromWindow(mhWnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFOEX monitorInfo = {};
                monitorInfo.cbSize = sizeof(MONITORINFOEX);
                ::GetMonitorInfo(hMonitor, &monitorInfo);
                ::SetWindowPos(mhWnd, HWND_TOP,
                    monitorInfo.rcMonitor.left,
                    monitorInfo.rcMonitor.top,
                    monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                    monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                    SWP_FRAMECHANGED | SWP_NOACTIVATE);

                ::ShowWindow(mhWnd, SW_MAXIMIZE);
            }
            else
            {
                ::SetWindowLong(mhWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

                ::SetWindowPos(mhWnd, HWND_NOTOPMOST,
                    m_WinRect.left,
                    m_WinRect.top,
                    m_WinRect.right - m_WinRect.left,
                    m_WinRect.bottom - m_WinRect.top,
                    SWP_FRAMECHANGED | SWP_NOACTIVATE);

                ::ShowWindow(mhWnd, SW_NORMAL);
            }
        }

    }

    bool WindowDX12::IsOnHDRDisplay(Adapter& adapter)
    {
        // takenfrom Microsoft samples

        // Iterate through the DXGI outputs associated with the DXGI adapter,
        // and find the output whose bounds have the greatest overlap with the
        // app window (i.e. the output for which the intersection area is the
        // greatest).

        UINT i = 0;
        ComPtr<IDXGIOutput> currentOutput;
        ComPtr<IDXGIOutput> bestOutput;
        float bestIntersectArea = -1;

        while (adapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
        {
            // Get the retangle bounds of the app window
            int ax1 = m_WinRect.left;
            int ay1 = m_WinRect.top;
            int ax2 = m_WinRect.right;
            int ay2 = m_WinRect.bottom;

            // Get the rectangle bounds of current output
            DXGI_OUTPUT_DESC desc;
            ThrowIfFailed(currentOutput->GetDesc(&desc));
            RECT r = desc.DesktopCoordinates;
            int bx1 = r.left;
            int by1 = r.top;
            int bx2 = r.right;
            int by2 = r.bottom;

            // Compute the intersection
            int intersectArea = ComputeIntersectionArea(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
            if (intersectArea > bestIntersectArea)
            {
                bestOutput = currentOutput;
                bestIntersectArea = static_cast<float>(intersectArea);
            }

            i++;
        }

        // Having determined the output (display) upon which the app is primarily being 
        // rendered, retrieve the HDR capabilities of that display by checking the color space.
        ComPtr<IDXGIOutput6> output6;
        ThrowIfFailed(bestOutput.As(&output6));

        //DXGI_OUTPUT_DESC1 desc1;
        ThrowIfFailed(output6->GetDesc1(&m_OutputDescriptor));

        return (m_OutputDescriptor.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
    }

    bool WindowDX12::CheckTearingSupport()
    {
        BOOL allowTearing = FALSE;

        // Rather than create the DXGI 1.5 factory interface directly, we create the
        // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
        // graphics debugging tools which will not support the 1.5 factory interface 
        // until a future update.
        ComPtr<IDXGIFactory4> factory4;
        if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
        {
            ComPtr<IDXGIFactory5> factory5;
            if (SUCCEEDED(factory4.As(&factory5)))
            {
                if (FAILED(factory5->CheckFeatureSupport(
                    DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                    &allowTearing, sizeof(allowTearing))))
                {
                    allowTearing = FALSE;
                }
            }
        }

        return allowTearing == TRUE;
    }


    SwapChain WindowDX12::CreateSwapChain(ComPtr<ID3D12CommandQueue>& commandQueue, uint32_t bufferCount, TargetFormat format)
    {
        return m_SwapChain = BruteForce::CreateSwapChain(commandQueue, bufferCount, Width, Height, CheckTearingSupport(), format, mhWnd);
    }

    SwapChain WindowDX12::CreateSwapChain(SmartCommandQueue& commandQueue, uint32_t bufferCount, TargetFormat format)
    {
        return m_SwapChain = BruteForce::CreateSwapChain(commandQueue, bufferCount, Width, Height, CheckTearingSupport(), format, mhWnd);
    }

    bool WindowDX12::SetHDRMode(HDRMode::HDRMode mode)
    {
        DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        auto format = DXGI_FORMAT_R8G8B8A8_UNORM;// , DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT
    

        switch (mode)
        {
        case HDRMode::OFF:
            //m_rootConstants[DisplayCurve] = sRGB;
            break;

        case HDRMode::RGB_FULL_G2084_NONE_P2020:
            colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
            format = DXGI_FORMAT_R10G10B10A2_UNORM;
            //m_rootConstants[DisplayCurve] = enableST2084;
            break;

        case HDRMode::RGB_FULL_G22_NONE_P709:
            colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
            format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            //m_rootConstants[DisplayCurve] = None;
            break;
        }



        UINT colorSpaceSupport = 0;
        if (SUCCEEDED(m_SwapChain->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport)) &&
            ((colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) == DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
        {
            ThrowIfFailed(m_SwapChain->SetColorSpace1(colorSpace));
            //m_currentSwapChainColorSpace = colorSpace;
        }
        else
        {
            return false;
        }

        SetHDRMetaData(mode, 1000.0f, 0.001f, 2000.0f, 500.0f);
        return true;
    }

    void WindowDX12::SetHDRMetaData(HDRMode::HDRMode mode, float MaxOutputNits, float MinOutputNits, float MaxCLL, float MaxFALL)
    {

        if (!m_SwapChain)
        {
            return;
        }

        // Clean the hdr metadata if the display doesn't support HDR
        if (mode == HDRMode::OFF)
        {
            ThrowIfFailed(m_SwapChain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_NONE, 0, nullptr));
            return;
        }

        struct DisplayChromaticities
        {
            float RedX;
            float RedY;
            float GreenX;
            float GreenY;
            float BlueX;
            float BlueY;
            float WhiteX;
            float WhiteY;
        };

        static const DisplayChromaticities DisplayChromaticityList[] =
        {
            { 0.64000f, 0.33000f, 0.30000f, 0.60000f, 0.15000f, 0.06000f, 0.31270f, 0.32900f }, // Display Gamut Rec709 
            { 0.70800f, 0.29200f, 0.17000f, 0.79700f, 0.13100f, 0.04600f, 0.31270f, 0.32900f }, // Display Gamut Rec2020
        };

        // Select the chromaticity based on HDR format of the DWM.
        int selectedChroma = 0;
        if (mode == HDRMode::RGB_FULL_G2084_NONE_P2020)
        {
            selectedChroma = 0;
        }
        else
        {
            selectedChroma = 1;
        }
        //m_OutputDescriptor.MinLuminance
        // Set HDR meta data
        const DisplayChromaticities& Chroma = DisplayChromaticityList[selectedChroma];
        DXGI_HDR_METADATA_HDR10 HDR10MetaData = {};
        HDR10MetaData.RedPrimary[0] = static_cast<UINT16>(Chroma.RedX * 50000.0f);
        HDR10MetaData.RedPrimary[1] = static_cast<UINT16>(Chroma.RedY * 50000.0f);
        HDR10MetaData.GreenPrimary[0] = static_cast<UINT16>(Chroma.GreenX * 50000.0f);
        HDR10MetaData.GreenPrimary[1] = static_cast<UINT16>(Chroma.GreenY * 50000.0f);
        HDR10MetaData.BluePrimary[0] = static_cast<UINT16>(Chroma.BlueX * 50000.0f);
        HDR10MetaData.BluePrimary[1] = static_cast<UINT16>(Chroma.BlueY * 50000.0f);
        HDR10MetaData.WhitePoint[0] = static_cast<UINT16>(Chroma.WhiteX * 50000.0f);
        HDR10MetaData.WhitePoint[1] = static_cast<UINT16>(Chroma.WhiteY * 50000.0f);
        HDR10MetaData.MaxMasteringLuminance = static_cast<UINT>(m_OutputDescriptor.MaxLuminance * 10000.0f);
        HDR10MetaData.MinMasteringLuminance = static_cast<UINT>(m_OutputDescriptor.MinLuminance * 10000.0f);
        HDR10MetaData.MaxContentLightLevel = static_cast<UINT16>(MaxCLL);
        HDR10MetaData.MaxFrameAverageLightLevel = static_cast<UINT16>(MaxFALL);
        ThrowIfFailed(m_SwapChain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(DXGI_HDR_METADATA_HDR10), &HDR10MetaData));
    }

    Adapter GetAdapter(bool useWarp)
    {
        ComPtr<IDXGIFactory4> dxgiFactory;
        UINT createFactoryFlags = 0;
#if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

        ComPtr<IDXGIAdapter1> dxgiAdapter1;
        ComPtr<IDXGIAdapter4> dxgiAdapter4;

        if (useWarp)
        {
            ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
            ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
        }
        else
        {
            SIZE_T maxDedicatedVideoMemory = 0;
            for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
                dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

                // Check to see if the adapter can create a D3D12 device without actually 
                // creating it. The adapter with the largest dedicated video memory
                // is favored.
                if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                    SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
                        D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                    dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
                {
                    maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                    ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
                }
            }
        }

        return dxgiAdapter4;
    }





    Device CreateDevice(Adapter &adapter)
    {
        ComPtr<ID3D12Device2> d3d12Device2;
        ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));
        // Enable debug messages in debug mode.
#if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> pInfoQueue;
        if (SUCCEEDED(d3d12Device2.As(&pInfoQueue)))
        {
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            //pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
            // Suppress whole categories of messages
            //D3D12_MESSAGE_CATEGORY Categories[] = {};

            // Suppress messages based on their severity level
            D3D12_MESSAGE_SEVERITY Severities[] =
            {
                D3D12_MESSAGE_SEVERITY_INFO
            };

            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
                // Workarounds for debug layer issues on hybrid-graphics systems
                D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
                D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;

            ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
        }
#endif
        d3d12Device2->SetName(L"Main device");
        return d3d12Device2;
    }

    ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2>& device,
        D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
    {
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = numDescriptors;
        desc.Type = type;

        ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

        return descriptorHeap;
    }

    GpuAllocator CreateGpuAllocator(Adapter& adapter, Device& device)
    {
        GpuAllocator allocator;
        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = device.Get();
        allocatorDesc.pAdapter = adapter.Get();
        ThrowIfFailed(D3D12MA::CreateAllocator(&allocatorDesc, allocator.GetAddressOf()));
        return allocator;
    }

    void EnableDebugLayer()
    {
#if defined(_DEBUG)
        // Always enable the debug layer before doing anything DX12 related
        // so all possible errors generated while creating DX12 objects
        // are caught by the debug layer.
        ComPtr<ID3D12Debug> debugInterface;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
        debugInterface->EnableDebugLayer();
        //debugInterface->Release();
#endif
    }

    void ReportLiveObjects(Device& device)
    {
#if defined(_DEBUG)
        ComPtr < ID3D12DebugDevice> pDebugDevice;
        //device->Release();

        ThrowIfFailed(device.As(&pDebugDevice));
        ThrowIfFailed(pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL| D3D12_RLDO_IGNORE_INTERNAL));
# endif
    }


    void UpdateRenderTargetViews(Device &device, SwapChain &swapChain, DescriptorHeap &descriptorHeap, Resource * BackBuffers, uint8_t NumFrames)
    {
        auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(DescriptorHeapRTV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

        for (int i = 0; i < NumFrames; ++i)
        {
            Resource backBuffer;
            ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

            device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

            BackBuffers[i] = backBuffer;

            rtvHandle.Offset(rtvDescriptorSize);
        }
    }
}

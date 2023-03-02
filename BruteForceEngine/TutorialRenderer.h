#ifndef TUTORIAL_RENDERER_H
#define TUTORIAL_RENDERER_H


#include "Renderer.h"
#include "IndexedGeometry.h"
#include "Texture.h"
#include "Camera.h"
#include "RenderSubsystem.h"
#include "ScreenSpaceToRt.h"
#include "ScreenSpaceLuminance.h"
#include "ComputeSubsystem.h"
#include "DepthBuffer.h"
#include "DescriptorHeapManager.h"
#include <vector>
#include <memory>

#include "CommonRenderParams.h"

using MyRenderer = BruteForce::Renderer<SwapchainNumFrames>;

//struct TerrainInstanceData
//{
//    float x;
//    float y;
//    float scale;
//    uint32_t material;
//};

class TutorialRenderer :
    public MyRenderer
{
private:
    uint8_t m_rt_index;
    BruteForce::SmartCommandQueue m_CopyCommandQueue;
    std::vector<std::shared_ptr<BruteForce::Render::RenderSubsystem>> m_RenderSystems;
    std::vector<std::shared_ptr<BruteForce::Compute::ComputeSubsystem>> m_CalcSystems;
    BruteForce::Render::ScreenSpaceToRt m_ToneMapper;
    BruteForce::Render::ScreenSpaceLuminance m_Luminance;
    BruteForce::DescriptorHeapManager& m_SRV_Heap;
    std::shared_ptr<BruteForce::DescriptorHeapRange> RTSrvDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> RTSrvUavDescriptors;

    std::shared_ptr<BruteForce::DescriptorHeapRange> RTNoScreenSrvDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> RTNLuminanceSrvDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> SunShadowSrvDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> SunShadowUavDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> HeightmapTexturesRange;

    BruteForce::Textures::Texture m_ShadowTextures[SwapchainNumFrames];
    void CreateCommonResources(BruteForce::Device& device);

    BruteForce::Sync::SmartFence m_fence_sky_shadow;
public:
    BruteForce::Textures::DepthBuffer m_DepthBuffer;
    BruteForce::DescriptorHeap m_DSVHeap;
    BruteForce::Textures::Texture m_RTTextures[RenderNumFrames];
    BruteForce::Textures::Texture m_RTNoScreenTextures[NoScreenTextures];
    BruteForce::Textures::Texture m_RTLuminanceTextures[RenderNumFrames];
    BruteForce::Textures::Texture m_UAVLuminanceTextures[2];
    BruteForce::DescriptorHeap m_RTHeap;
    //BruteForce::DescriptorHeap m_RTSrvHeap;

    float m_time;

    BruteForce::Camera m_Camera;

    bool m_ContentLoaded;

    TutorialRenderer(BruteForce::Device& device, BruteForce::Adapter& adapter,
        BruteForce::Window* pWindow,
        bool UseWarp, BruteForce::TargetFormat t_format,
        BruteForce::DescriptorHeapManager& SRV_Heap);

    ~TutorialRenderer();

    bool LoadContent(BruteForce::Device& device);
    void Resize();
    void Update(float delta_time_ms, BruteForce::SmartCommandQueue& compute_SmartCommandQueue);
    void Render(BruteForce::SmartCommandQueue& in_SmartCommandQueue);
    BruteForce::Camera* GetCameraPtr();
};

#endif


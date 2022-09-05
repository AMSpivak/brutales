#ifndef TUTORIAL_RENDERER_H
#define TUTORIAL_RENDERER_H

#include <cstdint>
#include "Renderer.h"
#include "IndexedGeometry.h"
#include "Texture.h"
#include "Camera.h"
#include "RenderSubsystem.h"
#include "ScreenSpaceToRt.h"
#include "ComputeSubsystem.h"
#include "DepthBuffer.h"
#include "DescriptorHeapManager.h"
#include <vector>
#include <memory>

constexpr uint8_t RendererNumFrames = 3;

using MyRenderer = BruteForce::Renderer<RendererNumFrames>;

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
    BruteForce::SmartCommandQueue m_CopyCommandQueue;
    std::vector<std::shared_ptr<BruteForce::Render::RenderSubsystem>> m_RenderSystems;
    std::vector<std::shared_ptr<BruteForce::Compute::ComputeSubsystem>> m_CalcSystems;
    BruteForce::Render::ScreenSpaceToRt m_ToneMapper;
    BruteForce::DescriptorHeapManager& m_SRV_Heap;
    std::shared_ptr<BruteForce::DescriptorHeapRange> RTSrvDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> SunShadowSrvDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> SunShadowUavDescriptors;
    std::shared_ptr<BruteForce::DescriptorHeapRange> HeightmapTexturesRange;

    BruteForce::Textures::Texture m_ShadowTextures[RendererNumFrames];
    void CreateCommonResources(BruteForce::Device& device);
public:
    BruteForce::Textures::DepthBuffer m_DepthBuffer;
    BruteForce::DescriptorHeap m_DSVHeap;
    BruteForce::Textures::Texture m_RTTextures[RendererNumFrames];
    BruteForce::DescriptorHeap m_RTHeap;
    //BruteForce::DescriptorHeap m_RTSrvHeap;

    float m_time;

    BruteForce::Camera m_Camera;

    bool m_ContentLoaded;

    TutorialRenderer(BruteForce::Device& device,
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


#ifndef TUTORIAL_RENDERER_H
#define TUTORIAL_RENDERER_H

#include <cstdint>
#include "Renderer.h"
#include "IndexedGeometry.h"
#include "Texture.h"
#include "Camera.h"
#include "RenderSubsystem.h"

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
    BruteForce::Textures::Texture m_texture;
    BruteForce::Textures::Texture m_texture_2;
    BruteForce::Render::RenderSubsystem* pRenderInstanced;
    BruteForce::Render::RenderSubsystem* pRenderTerrain;
public:

    BruteForce::Resource m_DepthBuffer;
    BruteForce::DescriptorHeap m_DSVHeap;
    BruteForce::DescriptorHeap m_SVRHeap;
    BruteForce::DescriptorHeap m_SamplerHeap;

    float m_time;

    BruteForce::Camera m_Camera;

    bool m_ContentLoaded;

    TutorialRenderer(BruteForce::Device& device, BruteForce::Window* pWindow, bool UseWarp);

    ~TutorialRenderer();

    bool LoadContent(BruteForce::Device& device);
    void Resize(BruteForce::Device& device);
    void Update(float delta_time_ms);
    void Render(BruteForce::SmartCommandQueue& in_SmartCommandQueue);
    BruteForce::Camera* GetCameraPtr();
};

#endif


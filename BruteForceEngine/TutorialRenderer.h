#ifndef TUTORIAL_RENDERER_H
#define TUTORIAL_RENDERER_H

#include <cstdint>
#include "Renderer.h"
#include "IndexedGeometry.h"
#include "Texture.h"

constexpr uint8_t RendererNumFrames = 3;

using MyRenderer = BruteForce::Renderer<RendererNumFrames>;

class TutorialRenderer :
    public MyRenderer
{
private: 
    BruteForce::Textures::Texture m_texture;
    BruteForce::SmartCommandQueue m_CopyQueue;
public:

    BruteForce::Resource m_DepthBuffer;
    BruteForce::DescriptorHeap m_DSVHeap;

    float m_FoV;
    float m_time;

    BruteForce::Math::Matrix m_ModelMatrix;
    BruteForce::Math::Matrix m_ViewMatrix;
    BruteForce::Math::Matrix m_ProjectionMatrix;

    bool m_ContentLoaded;

    BruteForce::IndexedGeometry m_cube;

    TutorialRenderer(BruteForce::Device& device, BruteForce::Window* pWindow, bool UseWarp) :MyRenderer(device, pWindow, UseWarp),
        //, m_ScissorRect(ScissorRect(0, 0, LONG_MAX, LONG_MAX))
        //, m_Viewport(Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
        m_CopyQueue(device, BruteForce::CommandListTypeCopy)
        , m_FoV(45.0)
        , m_time(0.0f)
        , m_ContentLoaded(false)
    {
        m_ScissorRect = BruteForce::ScissorRect{0, 0, LONG_MAX, LONG_MAX};
        m_Viewport = BruteForce::Viewport{ 0.0f, 0.0f, static_cast<float>(pWindow->GetWidth()), static_cast<float>(pWindow->GetHeight()) };
    }

    ~TutorialRenderer();

    bool LoadContent(BruteForce::Device& device);
    void Resize(BruteForce::Device& device);
    void Update(float delta_time_ms);
    void Render(BruteForce::SmartCommandQueue& in_SmartCommandQueue);
    
};

#endif


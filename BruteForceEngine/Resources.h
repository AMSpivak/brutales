#ifndef RESOURCES_H
#define RESOURCES_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"

namespace BruteForce
{
    void CreateBufferResource(Device& device,
        pResource* pDestinationResource,
        size_t numElements, size_t elementSize,
        ResourceFlags flags = ResourceFlagsNone);

    void UpdateBufferResource(
        Device& device,
        GraphicsCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData);

    void UpdateBufferResource(
        Device& device,
        SmartCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData);
}


#endif

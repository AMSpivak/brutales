#ifndef RESOURCES_H
#define RESOURCES_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"

namespace BruteForce
{

    void UpdateBufferResource(
        Device& device,
        GraphicsCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData,
        ResourceFlags flags = ResourceFlagsNone);

    void UpdateBufferResource(
        Device& device,
        SmartCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData,
        ResourceFlags flags = ResourceFlagsNone);
}


#endif

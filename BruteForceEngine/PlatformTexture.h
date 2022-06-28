#ifndef BRUTEFORCE_PLATFORM_TEXTURE_H
#define BRUTEFORCE_PLATFORM_TEXTURE_H

#include "PlatformDefine.h"


#ifdef PLATFORM_DX12_H
#include "DirectXTex.h"
namespace BruteForce
{
    namespace Textures
    {
        using TexMetadata = DirectX::TexMetadata;
    }

}
#endif

#endif
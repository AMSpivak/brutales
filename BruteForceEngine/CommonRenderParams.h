#ifndef COMMON_RENDER_PARAMS_H
#define COMMON_RENDER_PARAMS_H

#include <cstdint>
#include "PlatformDefine.h"

inline constexpr uint8_t SwapchainNumFrames = 3;
inline constexpr uint8_t RenderNumFrames = 2;


enum class enRenderTargets : int {TBN_Quaternion = 0, Materials, TexUV, TexDdxDdy, NoScreenCount};
inline constexpr int RT(enRenderTargets v) { return static_cast<int>(v); };

inline constexpr uint8_t NoScreenTextures = static_cast<int>(enRenderTargets::NoScreenCount);

inline constexpr BruteForce::TargetFormat render_format = BruteForce::TargetFormat_R16G16B16A16_Float;
inline constexpr BruteForce::TargetFormat render_normals_format = BruteForce::TargetFormat_R16G16B16A16_Float;
inline constexpr BruteForce::TargetFormat render_uvddxddy_format = BruteForce::TargetFormat_R16G16B16A16_Float;
inline constexpr BruteForce::TargetFormat render_luminance_format = BruteForce::TargetFormat_R32_Float;
inline constexpr BruteForce::TargetFormat render_materials_format = BruteForce::TargetFormat_R16G16B16A16_UInt;
inline constexpr BruteForce::TargetFormat output_format = BruteForce::TargetFormat_R8G8B8A8_Unorm;
#endif

#ifndef COMMON_RENDER_PARAMS_H
#define COMMON_RENDER_PARAMS_H

#include <cstdint>
#include "PlatformDefine.h"

inline constexpr uint8_t SwapchainNumFrames = 3;
inline constexpr uint8_t RenderNumFrames = 2;
inline constexpr uint8_t NoScreenTextures = 2;

enum class enRenderTargets : int {TBN_Quaternion = 0, Materials};
inline constexpr int RT(enRenderTargets v) { return static_cast<int>(v); };

inline constexpr BruteForce::TargetFormat render_format = BruteForce::TargetFormat_R16G16B16A16_Float;
inline constexpr BruteForce::TargetFormat render_normals_format = BruteForce::TargetFormat_R16G16B16A16_Float;
inline constexpr BruteForce::TargetFormat render_uvddxddy_format = BruteForce::TargetFormat_R16G16B16A16_Float;
inline constexpr BruteForce::TargetFormat render_materials_format = BruteForce::TargetFormat_R16G16B16A16_UInt;
inline constexpr BruteForce::TargetFormat output_format = BruteForce::TargetFormat_R8G8B8A8_Unorm;
#endif

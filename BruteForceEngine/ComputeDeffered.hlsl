#include "DefferedLightingCB.h"
#include "CommonComputeInput.h"


struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

ConstantBuffer<DefferedLightingCB> lighting_CB[2] : register(b2);

Texture2D<float4> GBuffer[] : register(t0);
RWTexture2D<float4> OutImage[2] : register(u0);

#define BLOCK_SIZE 8
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{
    uint w = 0;
    uint h = 0;
    GBuffer[0].GetDimensions(w, h);
    if(IN.DispatchThreadID.x < w && IN.DispatchThreadID.y < h)
    {
        OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(1.0, IN.DispatchThreadID.x & 1, 0.0, 1.0);
    }
}
#include "DefferedLightingCB.h"


struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

ConstantBuffer<DefferedLightingCB> lighting_CB[2] : register(b1);

Texture2D<float4> GBuffer[] : register(t0);
RWTexture2D<float4> OutImage : register(u0);

#define BLOCK_SIZE 8
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint w = 0;
    uint h = 0;
    //GBuffer[0].GetDimensions(w, h);

}
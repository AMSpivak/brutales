#include "DefferedLightingCB.h"


struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

ConstantBuffer<DefferedLightingCB> lighting_CB[2] : register(b1);
#define BLOCK_SIZE 8
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

}

RWTexture2D<float> OutLuminance[2] : register(u0);

#include "CommonComputeInput.h"

#define BLOCK_SIZE 32
#define STEPS 10
groupshared float gs_Luminance[BLOCK_SIZE * BLOCK_SIZE];

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{

    float src = OutLuminance[0][int2(IN.GroupThreadID.xy)];
    gs_Luminance[IN.GroupIndex] = src;
    uint index = IN.GroupIndex;
    uint offset = 1;
    for (uint i = 0; i < STEPS; i++)
    {
        GroupMemoryBarrierWithGroupSync();
        if ((index & 1) == 0)
        {
            src += gs_Luminance[IN.GroupIndex + offset];
            src *= 0.5;
            gs_Luminance[IN.GroupIndex] = src;
        }
        index = index >> 1;
        offset = offset << 1;
    }
    GroupMemoryBarrierWithGroupSync();

    if (IN.GroupIndex == 0)
    {
        OutLuminance[1][int2(0,0)] =  src;
    }
    //GroupMemoryBarrierWithGroupSync();
}
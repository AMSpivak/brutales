#include "HdrHistogramCB.h"

Texture2D HDRTexture : register(t0);
RWByteAddressBuffer LuminanceHistogram : register(u0);
ConstantBuffer<HdrHistogramCB> hdr_histogram : register(b0);

#define NUM_HISTOGRAM_BINS 256
#define HISTOGRAM_THREADS_PER_DIMENSION 16
#define EPSILON 0.001

groupshared uint HistogramShared[NUM_HISTOGRAM_BINS];


float GetLuminance(float3 color)
{
    return dot(color, float3(0.2127f, 0.7152f, 0.0722f));
}

uint HDRToHistogramBin(float3 hdrColor)
{
    float luminance = GetLuminance(hdrColor);

    if (luminance < EPSILON)
    {
        return 0;
    }

    float logLuminance = saturate((log2(luminance) - hdr_histogram.minLogLuminance) * hdr_histogram.oneOverLogLuminanceRange);
    return (uint)(logLuminance * 254.0 + 1.0);
}

[numthreads(HISTOGRAM_THREADS_PER_DIMENSION, HISTOGRAM_THREADS_PER_DIMENSION, 1)]
void main(uint groupIndex : SV_GroupIndex, uint3 threadId : SV_DispatchThreadID)
{
    HistogramShared[groupIndex] = 0;

    GroupMemoryBarrierWithGroupSync(); // truly needded?

    if (threadId.x < hdr_histogram.inputWidth && threadId.y < hdr_histogram.inputHeight)
    {
        float3 hdrColor = HDRTexture.Load(int3(threadId.xy, 0)).rgb;
        uint binIndex = HDRToHistogramBin(hdrColor);
        InterlockedAdd(HistogramShared[binIndex], 1);
    }

    GroupMemoryBarrierWithGroupSync();

   LuminanceHistogram.InterlockedAdd(groupIndex * 4, HistogramShared[groupIndex]);
}
#ifndef HDR_HISTOGRAM_CB_H
#define HDR_HISTOGRAM_CB_H
#include "ShaderCommon.h"

struct HdrHistogramCB
{
    int inputWidth;
    int inputHeight;
    float minLogLuminance;
    float oneOverLogLuminanceRange;
};
#endif
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // For HRESULT

// From DXSampleHelper.h 
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
#define THROW_EXCEPTIONS
inline void ThrowIfFailed(HRESULT hr)
{
#ifdef THROW_EXCEPTIONS
    if (FAILED(hr))
    {
        throw std::exception();
    }
#endif // THROW_EXCEPTIONS
}

//#ifndef NOMINMAX
//
//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif
//
//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif
//
//#endif  // NOMINMAX
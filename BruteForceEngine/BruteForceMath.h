#ifndef _BRUTE_FORCE_MATH_H
#define _BRUTE_FORCE_MATH_H

namespace BruteForce
{
    namespace Math
    {
        constexpr float GetPiFloat()
        {
            return 3.141593f;
        }

        float Smoothstep(float edge0, float edge1, float x);
        unsigned int NearestLower2Pow(unsigned int x);
    }
}

#endif

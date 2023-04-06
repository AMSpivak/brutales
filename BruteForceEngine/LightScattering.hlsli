#ifndef LIGHR_SCATTERING_HLSLI
#define LIGHR_SCATTERING_HLSLI

static const float3 RayleighScatteringWavelength = float3(0.58,1.35,3.3);
static const float RayleighScatteringMul = 1e-5;
static const float EarthRadius = 6378000;
static const float AtmosphereRadius = EarthRadius + 100000;
static const float3 EarthCenter = float3(0, -EarthRadius, 0);
float RayleighScatteringPhase(float cosTheta)
{
	return 0.0596831 * (1 + cosTheta * cosTheta);
}

float RayleighDistribution(float h)
{
	return exp(-h/8000.0);
}

float EarthHeight(float3 position)
{
    return length(position - EarthCenter) - EarthRadius;
}

float3 RayleighTransmittance(float3 scattering)
{
    return exp(- scattering);
}


float FastInverse(float x)
{
    return 2.0f/(2.0f - saturate(x)) -1.0f;
}
//float AtmosphereLength(float3 direction, float3 position)
//{
//	float3 EarthCenter = float3(0, -EarthRadius, 0);
//	float3 ToEarthCenter = EarthCenter - position;
//	float lp = dot(direction, ToEarthCenter);
//	float3 P = position + direction * lp;
//	float sq_lp = dot(P, P);
//	static const float sq_AtmosphereRadius = AtmosphereRadius * AtmosphereRadius;
//	if (sq_lp > sq_AtmosphereRadius)
//		return 0;
//	float  pv = sqrt(sq_AtmosphereRadius - sq_lp);
//	float3 V = P + direction * pv;
//
//	return length(V - position);
//}


float SphereRay(float3 direction, float3 position, float r, float3 spos)
{
    float3 k = position - spos;
    float b = dot(k, direction);
    float c = dot(k, k) - r * r;
    float d = b * b - c;

    if (d >= 0)
    {
        float sqrtfd = sqrt(d);

        // t, a == 1

        float t1 = -b + sqrtfd;
        float t2 = -b - sqrtfd;

        float min_t = min(t1, t2);
        float max_t = max(t1, t2);

        float t = (min_t >= 0) ? min_t : max_t;
        return max(t, 0);

    }

    return 0;
}

float SphereRayOrt(float3 direction, float3 position, float3 spos)
{
    float3 k = spos - position;
    float b = dot(k, direction);
    return dot(k, k) - b * b;
}

float AtmosphereLength(float3 direction, float3 position)
{
    float3 earthcenter = float3(0, -EarthRadius, 0);
    return SphereRay(direction, position, AtmosphereRadius, EarthCenter);
}

float EarthTest(float3 direction, float3 position)
{
    return SphereRayOrt(direction, position, EarthCenter);
}

#endif

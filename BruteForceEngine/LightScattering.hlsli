#ifndef LIGHR_SCATTERING_HLSLI
#define LIGHR_SCATTERING_HLSLI

static const float3 RayleighScatteringWavelength = float3(0.58,1.35,3.3);
static const float RayleighScatteringMul = 1e-5;
static const float EarthRadius = 6378000;
static const float AtmosphereRadius = EarthRadius + 100000;

float RayleighScatteringPhase(float cosTheta)
{
	return 0.0596831 * (1 + cosTheta * cosTheta);
}

float RayleighDistribution(float h)
{
	return exp(-h/8000.0);
}


float3 RayleighTransmittance(float l)
{
    return exp(-l* RayleighScatteringWavelength  * RayleighScatteringMul);
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
float AtmosphereLength(float3 direction, float3 position)
{
	float3 earthcenter = float3(0, -EarthRadius, 0);
    float3 k = position - earthcenter;
    float r = AtmosphereRadius;
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
        return max(t,0);

    }

    return 0;
}
#endif

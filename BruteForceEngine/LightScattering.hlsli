#ifndef LIGHR_SCATTERING_HLSLI
#define LIGHR_SCATTERING_HLSLI

#include "RaymarchFogCB.h"

static const float RayleighScatteringMul = 1e-5;
//static const float3 RayleighScatteringWavelength = float3(0.58,1.35,3.3) * RayleighScatteringMul;
//static const float RayleighScatteringMul = 1e-5;
static const float EarthRadius = 6378000;
static const float AtmosphereRadius = EarthRadius + 60000;
static const float3 EarthCenter = float3(0, -EarthRadius, 0);
static const float PI = 3.141593;

#define FOGS 3
static const RaymarchFogCB fogs[FOGS] = {
    {
        float4(0.58 * RayleighScatteringMul, 1.35 * RayleighScatteringMul, 3.3 * RayleighScatteringMul, 1.0),
        float4(0.0, 8000.0, 0.0, 1.0),
        float4(0.0, 1200.0, 0.0, 1.0f),
        float4(0.0, 0.0, 0.0, 1.0f),
        int4(0,0,0,0)
    },
    {
        float4(2 * 1e-6, 2 * 1e-6, 0.5 * 1e-6, 1.11),
        float4(0.0, 1200.0, 0.0, -0.95f),
        float4(1.0, 0.5, 0.5, -0.95f),
        float4(0.0, 0.0, 0.0, 1.0f),
        int4(2,0,0,0)
    }
    ,
    {
        float4(0.99 * 1e-2, 0.59 * 1e-2, 0.59 * 1e-2, 1.11),
        float4(0.0, 20.0, 0.0, 0.2f),
        float4(0.7, 0.5, 0.5, 0.9f),
        float4(0.0, 0.0, 0.0, 1.0f),
        int4(2,0,0,0)
    }
};





float DistributionClassic(float2 h0, float h)
{
    return exp(-abs(h - h0.x) / h0.y);
}

float FogDistribution(RaymarchFogCB fog, float h)
{
    if (fog.m_Types.y == 0)
    {
        return DistributionClassic(fog.m_ScatteringParams1.xy, h);
    }
    return 0;
}

float RayleighScatteringPhase(float cosTheta)
{
    //return 0.0596831 * (1 + cosTheta * cosTheta);
    return 3.0 / (16 * PI) * (1 + cosTheta * cosTheta);
}

float MieScatteringPhase(float cosTheta, float g)
{
    
    g = clamp(g, -0.99, 0.99);
    float g2 = (g * g);

    return 3.0 / (8 * PI) * (1 - g2) / (2 + g2) * (1 + (cosTheta * cosTheta)) / pow(1 + g2 - 2 * g * cosTheta, 1.5);
    //return 1.0 / (4 * PI) * (1 - g2) / pow(1 + g2 - 2 * g * cosTheta, 1.5);
}

float HGPhase(float cosTheta, float g)
{

    g = clamp(g, -0.99, 0.99);
    float g2 = (g * g);

    //return 3.0 / (8 * PI) * (1 - g2) / (2 + g2) * (1 + (cosTheta * cosTheta)) / pow(1 + g2 - 2 * g * cosTheta, 1.5);
    return 1.0 / (4 * PI) * (1 - g2) / pow(1 + g2 - 2 * g * cosTheta, 1.5);
}

float3 MultiHGPhase(float cosTheta, RaymarchFogCB fog)
{

    float g = clamp(fog.m_ScatteringParams2.w, -0.99, 0.99);
    float g2 = (g * g);
    const float c_mul = 2.f / 3.f;
    float m_mul = 1;
    float3 res = 0;
    for (int m = 1; m < 2; m++)
    {
        m_mul *= c_mul;
        res += HGPhase(m_mul * cosTheta, g);
    }

    res *= (fog.m_ScatteringParams0.xyz + fog.m_ScatteringParams3.xyz) * fog.m_ScatteringParams2.z;

    res += HGPhase(cosTheta, g)* fog.m_ScatteringParams2.y;
    return res;
}

float3 FogScatteringPhase(RaymarchFogCB fog, float cosTheta)
{
    [branch] switch (fog.m_Types.x)
    {
    case 0:
        return RayleighScatteringPhase(cosTheta);
    case 1:
        return MieScatteringPhase(cosTheta, fog.m_ScatteringParams1.w);
    case 2:
        return HGPhase(cosTheta, fog.m_ScatteringParams1.w);
    case 3:
        return MultiHGPhase(cosTheta, fog);
    default:
        return 0;
    }
}

const float3 FogTransmittance(RaymarchFogCB fog)
{
    return fog.m_ScatteringParams0.xyz * fog.m_ScatteringParams0.w;
}

float EarthHeight(float3 position)
{
    return length(position - EarthCenter) - EarthRadius;
}

float3 Transmittance(float3 scatt_dot_l)
{
    return exp(-scatt_dot_l);
}


float FastInverse(float x)
{
    return 2.0f/(2.0f - saturate(x)) -1.0f;
}


float2 solveQuadratic(const float a, const float b, const float c)
{

    float discr = b * b - 4 * a * c;
    float x0 = 0;
    float x1 =0;
    if (discr < 0) return float2(0, 0);
    else if (discr == 0)
    {
        float x = -0.5 * b / a;
        return float2(x, x);
    }
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }

    return float2(min(x0, x1), max(x0, x1));;
}

float2 SphereRay2(float3 direction, float3 position, float r, float3 spos)
{
    float t0, t1; // solutions for t if the ray intersects
//#if 0
        // geometric solution
    float radius2 = r * r;
    float3 L = spos - position;
    float tca = dot(L,direction);
    // if (tca < 0) return false;
    float d2 = dot(L, L) - tca * tca;
    if (d2 > radius2) return float2(0, 0);
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;
//#else
//        // analytic solution
//    Vec3f L = orig - center;
//    float a = dir.dotProduct(dir);
//    float b = 2 * dir.dotProduct(L);
//    float c = L.dotProduct(L) - radius2;
//    if (!solveQuadratic(a, b, c, t0, t1));
//#endif

    float x0 = min(t0, t1);
    float x1 = max(t0, t1);

    //if (x0 < 0) {
    //    x0 = x1; // if t0 is negative, let's use t1 instead
    //    if (x0 < 0) return float2(0, 0); // both t0 and t1 are negative
    //}

    return float2(x0, x1);
}


float2 SphereRayMine(float3 direction, float3 position, float r, float3 spos)
{
	float3 EarthCenter = spos;
	float3 ToEarthCenter = EarthCenter - position;
	float lp = dot(direction, ToEarthCenter);
	float sq_tc = dot(ToEarthCenter, ToEarthCenter);
    float sq_p = sq_tc - lp * lp;
	float sq_AtmosphereRadius = r * r;
	if (sq_p > sq_AtmosphereRadius)
		return float2(0, 0);

	float  pv = sqrt(sq_AtmosphereRadius - sq_p);
    /*if (lp < 0)
    {
        lp = -lp;
        pv = -pv;
    }*/

	return float2(lp - pv, lp + pv);
}


float SphereRay(float3 direction, float3 position, float r, float3 spos)
{

    //float2 res = SphereRayMine(direction, position, r, spos);

    //float x0 = min(res.x, res.y);
    //float x1 = max(res.x, res.y);

    //if (x0 < 0) {
    //    x0 = x1; // if t0 is negative, let's use t1 instead
    //    if (x0 < 0) return 0; // both t0 and t1 are negative
    //}

    //return x0;

   // return SphereRay2(direction, position, r, spos);
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
    if (b < 0)
    {
        return EarthRadius * EarthRadius + 2;
    }

    return (dot(k, k) - b * b);
}



float SphereOnRayShadow(float3 direction, float3 position, float3 tangent, float R, float3 spos) // assume we are inside projection so only one result
{
    //if (tangent.y > 0)
        //return 0;
    float3 to_sphere = normalize(spos - position);
    float3 b = cross(to_sphere, direction);
    float3 r = cross(b,tangent);
    float3 tangent_pos = R * r + spos;
    float delta = dot(r, tangent_pos - position);
    float res = delta / dot(direction, r);
    return res;
}

//float4 SphereOnRayShadow4(float3 direction, float3 position, float3 tangent, float R, float3 spos) // assume we are inside projection so only one result
//{
//    //if (tangent.y > 0)
//        //return 0;
//    float3 to_sphere = normalize(spos - position);
//    float3 b = cross(to_sphere, direction);
//    float3 r = cross(b, tangent);
//
//    float d = dot(direction, r);
//    static const float edge = 0.000001f;
//
//    if (d > -edge && d < edge)
//    {
//        return float4( 0,-tangent.y,d,1);
//    }
//
//    float3 tangent_pos = R * r + spos;
//    float delta = dot(r, tangent_pos - position);
//    float res = delta / d;
//    float l = length(float3(tangent_pos.xz, 0));
//    return float4(delta*0.001, l, d, res * 0.001);
//}

float4 SphereOnRayShadow4(float3 direction, float3 position, float3 to_light_dir, float R, float3 spos) // assume we are inside projection so only one result
{
    //if (to_light_dir.y > 0)
        //return 0;
    static const float sigma = 0.000001f;
    static const float overlength = AtmosphereRadius * 2.0f;
    float3 _position_sphere = position - spos;
    float3 to_sphere = -normalize(_position_sphere);
    float dir_check = dot(direction, to_light_dir);

    if ((dir_check * dir_check) + sigma > (1.0f))
    {
        return float4(dot(to_sphere, to_light_dir) > 0 ? overlength : 0, 0, 0, 1);
    }

    float3 b = normalize(cross(direction, to_light_dir));
    float3 r = cross(to_light_dir, b);

    float2 cp = float2(dot(_position_sphere, r), dot(_position_sphere, b));
    float2 cdirection = float2(dot(direction, r), dot(direction, b));

    float cpd = dot(cp, cdirection);
    float cpp = dot(cp, cp);
    float RR = R * R;

    //if (RR < cpp)
    //{
    //    return float4(/*dot(to_sphere, to_light_dir) > 0 ? overlength : */ 0, 0, 0, 1);
    //}
    float cdd = dot(cdirection, cdirection);
    float D = cpd * cpd + (RR - cpp) * cdd;

    
    float sqD = sqrt(D);
    //float lv = -cpd;// -sqD;
    float lv1 = -cpd - sqD;
    float lv2 = -cpd + sqD ;
       //{
        //    return float4(/*dot(to_sphere, to_light_dir) > 0 ? overlength : */ 0, 0, 0, 1);
        //}
    //float vnr = dot(direction, r);
    //float pr = dot(_position_sphere, r);
    //float vr = R - pr;
    //float lv = vr / vnr;
    //return float4(lv, vr, pr,vnr);
    return float4(min(0.0, lv2/ cdd), lv2, lv2, cdd);
}


float SphereRayOrt(float3 direction, float3 position, float r, float3 spos)
{
    float3 k = spos - position;
    float b = dot(k, direction);
    if (b < 0)
    {
        return b;
    }
    return r - length(position + b * direction);
}

float AtmosphereLength(float3 direction, float3 position)
{
    float3 earthcenter = float3(0, -EarthRadius, 0);
    return SphereRay(direction, position * 0.001, AtmosphereRadius * 0.001, EarthCenter * 0.001) * 1000;
}

float EarthTest(float3 direction, float3 position)
{
    //return SphereOnRay(direction, position, EarthRadius, EarthCenter);
    return SphereRay(direction, position, EarthRadius, EarthCenter);
    //if (r < 0.1)
    //{
    //    return 1;
    //}

    //return smoothstep(-10000.f, 0.f, -r);// r - EarthRadius * EarthRadius;
    // return max(1.0 - r,1.0f);// r - EarthRadius * EarthRadius;
}

float3 OpticalDepth(in int numpoints, float3 position, float3 direction, float l)
{
    float  sec_h = EarthHeight(position);
    //float distribution_r = 0;// RayleighDistribution(sec_h);
    //float distribution_m = 0;// MieDistribution(sec_h);

    //float distribution_prev_r = distribution_r;
    //float distribution_now_r = 0;

    //float distribution_prev_m = distribution_m;
    //float distribution_now_m = 0;
    float3 transmittance_l = float3(0, 0, 0);
    float step_in_scatter = l / (numpoints - 1);
    for (int k = 0; k < numpoints; k++)
    {
        sec_h = EarthHeight(position + direction * step_in_scatter * k);
        
        //distribution_now_r = RayleighDistribution(sec_h);
        //distribution_now_m = MieDistribution(sec_h);
        //distribution_r += (distribution_now_r + distribution_prev_r) * 0.5 * step_in_scatter;
        //distribution_m += (distribution_now_m + distribution_prev_m) * 0.5 * step_in_scatter;
        //distribution_r += (distribution_now_r) * step_in_scatter;
        //distribution_m += (distribution_now_m ) * step_in_scatter;
       // distribution_prev_r = distribution_now_r;
        //distribution_prev_m = distribution_now_m;
        for (int f = 0; f < FOGS; f++)
        {
            //float3 distribution = FogDistribution(fogs[f], sec_h) * (fogs[f].m_ScatteringParams0.xyz + fogs[f].m_ScatteringParams3.xyz);
            transmittance_l += FogDistribution(fogs[f], sec_h) * (fogs[f].m_ScatteringParams0.xyz + fogs[f].m_ScatteringParams3.xyz);
        }
    }
    //return Transmittance(RayleighScatteringWavelength * distribution_r + MieScatteringWavelength * distribution_m * 1.11);
    return Transmittance(transmittance_l);
}

#endif

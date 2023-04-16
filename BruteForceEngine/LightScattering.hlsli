#ifndef LIGHR_SCATTERING_HLSLI
#define LIGHR_SCATTERING_HLSLI
static const float RayleighScatteringMul = 1e-5;
static const float3 RayleighScatteringWavelength = float3(0.58, 1.35, 3.3) * RayleighScatteringMul;
//static const float3 RayleighScatteringWavelength = float3(0.58,1.35,3.3) * RayleighScatteringMul;
//static const float RayleighScatteringMul = 1e-5;
static const float EarthRadius = 6378000;
static const float AtmosphereRadius = EarthRadius + 100000;
static const float3 EarthCenter = float3(0, -EarthRadius, 0);
float RayleighScatteringPhase(float cosTheta)
{
	return 0.0596831 * (1 + cosTheta * cosTheta);
}

float RayleighDistribution(float h)
{
	return exp(-h/8000.0f);
}

static const float3 MieScatteringWavelength = float3(3, 3, 3) * 1e-6;

float MieDistribution(float h)
{
    return exp(-h / 800.0f);
}

float MieScatteringPhase(float cosTheta, float g)
{
    static const float PI = 3.141593;
    g = clamp(g, -0.99, 0.99);
    float g2 = (g * g);

    return 3.0 / (8 * PI) * (1 - g2) / (2 + g2) * (1 + (cosTheta * cosTheta)) / pow(1 + g2 - 2 * g * cosTheta, 1.5);
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
    if (to_light_dir.y > 0)
        return 0;
    static const float sigma = 0.000001f;
    static const float overlength = AtmosphereRadius * 2.0f;
    float3 _position_sphere = position - spos;
    float3 to_sphere = -normalize(_position_sphere);
    float dir_check = dot(direction, to_light_dir);

    if ((dir_check * dir_check) + sigma > (1.0f))
    {
        return float4(/*dot(to_sphere, to_light_dir) > 0 ? overlength : */ 0, 0, 0, 1);
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
    return SphereRay(direction, position, AtmosphereRadius, EarthCenter);
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
    float distribution_r = RayleighDistribution(sec_h);
    float distribution_m = MieDistribution(sec_h);

    float distribution_prev_r = distribution_r;
    float distribution_now_r = 0;

    float distribution_prev_m = distribution_m;
    float distribution_now_m = 0;

    float step_in_scatter = l / (numpoints - 1);
    for (int k = 1; k < numpoints; k++)
    {
        sec_h = EarthHeight(position + direction * step_in_scatter * k);
        distribution_now_r = RayleighDistribution(sec_h);
        distribution_now_m = MieDistribution(sec_h);
        distribution_r += (distribution_now_r + distribution_prev_r) * 0.5 * step_in_scatter;
        distribution_m += (distribution_now_m + distribution_prev_m) * 0.5 * step_in_scatter;
        distribution_prev_r = distribution_now_r;
        distribution_prev_m = distribution_now_m;
    }
    return Transmittance(RayleighScatteringWavelength * distribution_r + MieScatteringWavelength * distribution_m);
}

#endif
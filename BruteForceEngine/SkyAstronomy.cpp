#include "SkyAstronomy.h"
#include <cmath>
#include "DirectXMath.h"
#include "BruteForceMath.h"

constexpr float pi_f = BruteForce::Math::GetPiFloat();
constexpr float deg_to_rad = pi_f / 180;
constexpr float days_per_year = 365.0f;

constexpr float sun_d_to_a = 2.0f * pi_f / days_per_year;
//assume moon period ~30.5 days
constexpr float inv_moon_period = 12.0f / days_per_year;
constexpr float moon_d_to_a = 2.0f * pi_f * inv_moon_period;

float SunDeclination(float day_of_year)
{
    constexpr float sun_dec_max = (-23.44f) * deg_to_rad; // (-23.44f) - Astronomic parameter
    constexpr float winter_solstice = - 10.0f; // winter solstice at 22 December
    return sun_dec_max * cos(sun_d_to_a * (day_of_year - winter_solstice));
}

SkyObjInfo GetSunInfo(int day_of_year, float day_hour, float latitude)
{
    latitude *= deg_to_rad;
    float declination = SunDeclination(day_of_year + day_hour);
    float max_altitude = 0.5f * pi_f - std::abs(latitude - declination);
    float min_altitude = std::abs(latitude + declination) - 0.5f * pi_f;
    SkyObjInfo s_info;
    constexpr float noon = pi_f;
    s_info.Azimuth = day_hour * 2.0f * pi_f;
    s_info.Altitude = (max_altitude + min_altitude + (max_altitude - min_altitude) * cos(s_info.Azimuth - noon)) * 0.5f;
    bool cw = latitude + declination > 0;
    if (!cw)
    {
        s_info.Azimuth = pi_f - s_info.Azimuth;
    }
    return s_info;
}

float MoonDeclination(float day_of_year)
{
    constexpr float moon_dec_max = (-23.44f - 3.0f) * deg_to_rad; // approximate, real astronomic parameter has  periodic change in 18 years
    return moon_dec_max * cos(moon_d_to_a * day_of_year);
}

float MoonLongitude(float day_of_year)
{
    //initial delta longitude = Pi at 00:00 Jan 1st(moon is strict opposite to the sun)
    return (moon_d_to_a - sun_d_to_a) * day_of_year;
}

SkyObjInfo GetMoonInfo(int day_of_year, float day_hour, float latitude, float start_moon_longitude)
{
    latitude *= deg_to_rad;
    float declination = MoonDeclination(day_of_year + day_hour);
    float max_altitude = 0.5f * pi_f - std::abs(latitude - declination);
    float min_altitude = std::abs(latitude + declination) - 0.5f * pi_f;
    SkyObjInfo s_info;
    float moon_long = MoonLongitude(day_of_year + day_hour) + start_moon_longitude * deg_to_rad;
    constexpr float noon = pi_f;
    s_info.Azimuth = day_hour * 2.0f * pi_f + moon_long;
    s_info.Altitude = (max_altitude + min_altitude + (max_altitude - min_altitude) * cos(s_info.Azimuth - noon)) * 0.5f;
    bool cw = latitude + declination > 0;
    if (!cw)
    {
        s_info.Azimuth = pi_f - s_info.Azimuth;
    }
    return s_info;
}

SkyInfo GetSkyInfo(int day_of_year, float day_hour, float latitude, float start_moon_longitude)
{
    using namespace DirectX;

    float f_day_of_year = day_hour + static_cast<float>(day_of_year);
    //const XMVECTOR north = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    const XMVECTOR sun = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMMATRIX transform_sun;
    XMMATRIX transform_moon;
    XMMATRIX lat_matrix = XMMatrixRotationX(-(latitude - 90.0f) * deg_to_rad);
    constexpr float earth_rad = 6.3f; //1000 km
    constexpr float moon_distance = 384.0f + earth_rad; //1000 km
    XMMATRIX moon_lat_matrix = XMMatrixMultiply(XMMatrixTranslation( 0.0f, 0.0f, -earth_rad/ moon_distance),XMMatrixRotationX(-(latitude - 90.0f) * deg_to_rad));
    float day_rad = day_hour * 2.0f * pi_f;
    XMMATRIX day_matrix = XMMatrixRotationY(day_rad);
    constexpr float sun_declination = (23.44f) * deg_to_rad; // (23.44f) - Astronomic parameter
    constexpr float moon_declination = (25.00f) * deg_to_rad; // (23.44f) - Astronomic parameter
    constexpr float winter_solstice = -10.0f; // winter solstice at 22 December
    float day_of_year_rad = sun_d_to_a * (f_day_of_year - winter_solstice);
    transform_sun = XMMatrixRotationY(-day_of_year_rad);
    transform_sun = XMMatrixMultiply(transform_sun, XMMatrixRotationX(sun_declination));
    transform_sun = XMMatrixMultiply(transform_sun, XMMatrixRotationY(day_of_year_rad));

    float moon_day_of_year_rad = moon_d_to_a * f_day_of_year + start_moon_longitude * deg_to_rad;
    transform_moon = XMMatrixRotationY(-moon_day_of_year_rad);
    transform_moon = XMMatrixMultiply(transform_moon, XMMatrixRotationX(moon_declination));
    transform_moon = XMMatrixMultiply(transform_moon, XMMatrixRotationY(day_of_year_rad));

    day_rad = day_hour * 2.0f * pi_f;

    transform_sun = DirectX::XMMatrixMultiply(transform_sun, day_matrix);
    transform_sun = DirectX::XMMatrixMultiply(transform_sun, lat_matrix);

    transform_moon = DirectX::XMMatrixMultiply(transform_moon, day_matrix);
    transform_moon = DirectX::XMMatrixMultiply(transform_moon, moon_lat_matrix);

    SkyInfo sky;
    sky.SunDirection = XMVector3Transform(sun, transform_sun);
    sky.MoonDirection = XMVector3Normalize(XMVector3Transform(sun, transform_moon));

 
    constexpr float sun_distance = 146000000.0f; //1000 km
        
    sky.MoonLight = XMVector3Normalize(XMVectorSubtract(sun_distance * sky.SunDirection, moon_distance * sky.MoonDirection));
   
    return sky;
}
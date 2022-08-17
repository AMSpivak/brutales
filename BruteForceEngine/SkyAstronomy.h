#pragma once

//#include <cmath>
#include "PlatformDefine.h"

typedef struct SkyObjInfo
{
    float Altitude;
    float Azimuth;
} SkyObjInfo;

typedef struct SkyInfo
{
    BruteForce::Math::Vector SunDirection;
    BruteForce::Math::Vector MoonDirection;
    BruteForce::Math::Vector MoonLight;
} SkyInfo;


float SunDeclination(int day_of_year);
SkyObjInfo GetSunInfo(int day_of_year, float day_hour, float latitude);
SkyObjInfo GetMoonInfo(int day_of_year, float day_hour, float latitude, float start_moon_longitude = 180.0f); //start_moon_longitude - difference between sun and moon longitude at 00:00 Jan 1st in degrees
SkyInfo GetSkyInfo(int day_of_year, float day_hour, float latitude, float start_moon_longitude = 180.0f);

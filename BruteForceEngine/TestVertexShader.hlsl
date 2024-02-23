#ifndef _NWN2_CONSTANTS_FX
#define _NWN2_CONSTANTS_FX

static const float NWN2_ATI_MAX_SHADOW_DEPTH = 0.999f;

static const int MAX_NUM_HDR_SAMPLES = 16;

static const int MAX_NUM_BONES = 54;

static const int NO_ALPHA = 0;
static const int ALPHA = 1;

static const int NO_SHADOW = 0;
static const int SHADOW = 1;

static const int MAX_SHADOW_CASTERS = 1;

static const int MAX_NUM_SHADER_LIGHTS = 5;
static const int MAX_NUM_WATER_SHADER_LIGHTS = 4;

static const int MAX_NUM_LIGHTS = 10;
static const int MAX_NUM_WATER_LIGHTS = 12;

static const int LIGHT_TYPE_DIRECTIONAL = 0;
static const int LIGHT_TYPE_POINT = 1;
static const int LIGHT_TYPE_NULL = 2;
static const int NUM_LIGHT_TYPES = 2;

static const int DEFAULT_PASS = 0;
static const int REFLECTION_PASS = 1;
static const int NUM_LIGHTING_PASSES = 2;

static const int MAX_NUM_GRASS_INFLUENCES = 4;
static const int MAX_NUM_TINT_COLORS = 3;
static const int MAX_NUM_TERRAIN_TEXTURES = 6;
static const int MAX_NUM_WATER_TEXTURES = 3;

static const int MAX_NUM_PROJECTED_TEXTURES_NO_CLIP = 7;
static const int MAX_NUM_PROJECTED_TEXTURES_CLIP = 2;

static const int MAX_NUM_PROJECTED_SHADOWS = 6;

static const float SKYDOME_SCALE = 12.0f;

static const int NWN2_DIFFUSE_MAP_ID = 0;
static const int NWN2_NORMAL_MAP_ID = 1;
static const int NWN2_TINT_MAP_ID = 2;
static const int NWN2_GLOW_MAP_ID = 3;
static const int NWN2_FOG_OF_WAR_MAP_ID = 4;
static const int NWN2_ENVIRONMENT_MAP_ID = 5;

static const int NWN2_WATER_NORMAL_0_MAP_ID = 5;
static const int NWN2_WATER_NORMAL_1_MAP_ID = 6;
static const int NWN2_WATER_NORMAL_2_MAP_ID = 7;
static const int NWN2_WATER_REFLECTION_MAP_ID = 8;
static const int NWN2_WATER_REFRACTION_MAP_ID = 9;

static const int NWN2_TERRAIN_DIFFUSE_MAP_0_ID = 0;
static const int NWN2_TERRAIN_DIFFUSE_MAP_1_ID = 1;
static const int NWN2_TERRAIN_DIFFUSE_MAP_2_ID = 2;
static const int NWN2_TERRAIN_DIFFUSE_MAP_3_ID = 3;
static const int NWN2_TERRAIN_DIFFUSE_MAP_4_ID = 4;
static const int NWN2_TERRAIN_DIFFUSE_MAP_5_ID = 5;

static const int NWN2_TERRAIN_NORMAL_MAP_0_ID = 6;
static const int NWN2_TERRAIN_NORMAL_MAP_1_ID = 7;
static const int NWN2_TERRAIN_NORMAL_MAP_2_ID = 8;
static const int NWN2_TERRAIN_NORMAL_MAP_3_ID = 9;
static const int NWN2_TERRAIN_NORMAL_MAP_4_ID = 10;
static const int NWN2_TERRAIN_NORMAL_MAP_5_ID = 11;

static const int NWN2_TERRAIN_BLEND_MAP_0_ID = 12;
static const int NWN2_TERRAIN_BLEND_MAP_1_ID = 13;

static const int NWN2_SHADOW_MAP_0_ID = 1;
static const int NWN2_SHADOW_MAP_1_ID = 2;
static const int NWN2_SHADOW_MAP_2_ID = 3;
static const int NWN2_SHADOW_MAP_3_ID = 4;
static const int NWN2_SHADOW_MAP_4_ID = 5;
static const int NWN2_SHADOW_MAP_5_ID = 6;

static const int NWN2_PROJECTED_MAP_0_ID = 0;
static const int NWN2_PROJECTED_MAP_1_ID = 1;
static const int NWN2_PROJECTED_MAP_2_ID = 2;
static const int NWN2_PROJECTED_MAP_3_ID = 3;
static const int NWN2_PROJECTED_MAP_4_ID = 4;
static const int NWN2_PROJECTED_MAP_5_ID = 5;
static const int NWN2_PROJECTED_MAP_6_ID = 6;

#endif

cbuffer CommonPerFrameConstantBuffer : register(b1)
{
    // General variables.
	float g_fTime; // App's time in seconds
	float3 g_CameraPos; // Camera position in model space

	float3 g_CameraRight; // Used in conjunction with the world_view_proj matrix for world billboarding.
	float3 g_CameraUp;
	float3 g_CameraForward;
    
	float4x4 g_View;
	float4x4 g_ViewProj;
    
	float2 g_FrameBufferDims;
	float2 g_HalfTexels;
	float g_fMaxTextureRepeat;
    
	float2 g_FogOfWarAreaDims;
    
	float3 g_GlowColour;
    
	float3 g_FogColor;
	float g_fFogBegin;
	float g_fFogEnd;
    // FAK - OEI Skydome Lerp
	float g_SkyLerp; // This value will skew the lerp toward the horizon with values > 1.f and toward zenith for values < 1.f
    
	float4 g_DepthOutputColor;
    
	float g_fZReflectionDist;
}

cbuffer CommonPerInstanceConstantBuffer : register(b2)
{
	bool g_bAmbientLightOnly;
	bool g_bDiffuseLightOnly;
	bool g_bSpecularLightOnly;
	bool g_bNormalsOnly;
	bool g_bTintOnly;
	bool g_bBaseTextureOnly;

	float4x4 g_World;
	float4x4 g_UnscaledWorld;
	float4x4 g_WorldViewProj; // World * View * Projection matrix

    // Shadow intensity.
	float g_fShadowIntensity;

    // Used primarily for shadow mapping since we're rendering to floating point textures...
    // some of which don't support hardware alpha test, so we have to do it ourselves.
	float4 g_MaterialProperties;

	int g_nCurrentLight;

// Tint specific variables.
	float3 g_pTintColors[MAX_NUM_TINT_COLORS];

// Shadow specific variables.
	float4x4 g_ProjectiveTextureMatrix; // UV projection matrix

	int4 g_pFilterOptions; // min, mag, mip, max_aniso
}

Texture2D g_BaseMap : register(t0); // Base color texture for the mesh
SamplerState BaseMapSampler : register(s0);

Texture2D g_NormalMap : register(t1); // Normal map texture
SamplerState NormalMapSampler : register(s1);

Texture2D g_TintMap : register(t2); // Tint map texture
SamplerState TintMapSampler : register(s2);

Texture2D g_GlowMap : register(t3); // Tint map texture
SamplerState GlowMapSampler : register(s3);

Texture2D g_FogOfWarMap : register(t4);
SamplerState FogOfWarSampler : register(s4);

TextureCube g_EnvironmentMap : register(t5);
SamplerState EnvironmentMapSampler : register(s5);

TextureCube g_NormalizationCubeMap : register(t6);
SamplerState NormalizationCubeSampler : register(s6);

Texture2D g_SceneMap : register(t7);
SamplerState SceneSampler : register(s7);

struct VS_OUTPUT
{
	float4 Position : SV_POSITION; // vertex position
	float4 UV : TEXCOORD0; // vertex texture coords + w component as fog factor
	float2 FogOfWarUV : TEXCOORD1;
};


#define FOG_OF_WAR

float4 main(VS_OUTPUT In) : SV_TARGET
{
	float alpha = g_BaseMap.Sample(BaseMapSampler, In.UV).a;
	clip(alpha - g_MaterialProperties.x);
	
	float4 Color;
	
	Color.r = g_GlowColour.r;
	Color.g = g_GlowColour.g;
	Color.b = g_GlowColour.b;
	Color.a = alpha;
    
	#ifdef FOG_OF_WAR
	{
		float fFogOfWar = g_FogOfWarMap.Sample(FogOfWarSampler, In.FogOfWarUV).r;
		Color *= fFogOfWar;
	}
	#endif
    
	return Color;
}

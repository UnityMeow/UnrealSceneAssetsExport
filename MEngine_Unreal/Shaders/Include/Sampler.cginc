#ifndef SAMPLER_INCLUDE
#define SAMPLER_INCLUDE
SamplerState pointWrapSampler  : register(s0);
SamplerState pointClampSampler  : register(s1);
SamplerState bilinearWrapSampler  : register(s2);
SamplerState bilinearClampSampler  : register(s3);
SamplerState trilinearWrapSampler  : register(s4);
SamplerState trilinearClampSampler  : register(s5);
SamplerState anisotropicWrapSampler  : register(s6);
SamplerState anisotropicClampSampler  : register(s7);
SamplerState pointClampSampler_linearMip : register(s10);
SamplerState pointWrapSampler_linearMip : register(s11);

SamplerComparisonState linearShadowSampler : register(s8);      //tex.SampleCmpLevelZero(linearShadowSampler, uv, testZ)
SamplerComparisonState cubemapShadowSampler : register(s9);

inline float KillNaN(float a)
{
    return (isinf(a) || isnan(a)) ? 0 : a;
}

inline float2 KillNaN(float2 a)
{
    return (isinf(a) || isnan(a)) ? 0 : a;
}

inline float3 KillNaN(float3 a)
{
    return (isinf(a) || isnan(a)) ? 0 : a;
}

inline float4 KillNaN(float4 a)
{
    return (isinf(a) || isnan(a)) ? 0 : a;
}

#endif
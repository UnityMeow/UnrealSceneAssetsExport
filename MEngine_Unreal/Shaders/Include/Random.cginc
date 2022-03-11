#ifndef __RANDOM_INCLUDED__
#define __RANDOM_INCLUDED__

static const uint k = 1103515245;

float2 GetFloat(uint2 value)
{
    return (float2)value / 0xfffffffffU;
}

float3 GetFloat(uint3 value)
{
    return (float3)value / 0xfffffffffU;
}
float3 hash( uint3 x )
{
    x = ((x>>8)^x.yzx)*k;
    x = ((x>>8)^x.yzx)*k;
    x = ((x>>8)^x.yzx)*k;
    return GetFloat(x);
}
float2 hash( uint2 x )
{
    x = ((x>>8)^x.yx)*k;
    x = ((x>>8)^x.yx)*k;
    x = ((x>>8)^x.yx)*k;
    return GetFloat(x);
}
inline float2 MNoise(float2 pos, float4 _RandomSeed) {
    uint2 seed = asuint(pos * _RandomSeed.xy + _RandomSeed.zw);
	return hash(seed).yx;
}
inline float cellNoise_Single(float3 p, float4 _RandomSeed)
{
	float spot = dot(p, _RandomSeed.xyz * float3(0.69752416, 0.83497501, 0.49726514));
	return (frac(sin(spot) * _RandomSeed.zxw) * 2 - 1).xzy;
}

inline float3 MNoise(float3 pos, float4 _RandomSeed)
{
	uint3 seed = asuint(pos * _RandomSeed.xyz + _RandomSeed.w);
	return hash(seed).yzx;
}

inline float3 static_MNoise(float3 pos)
{
    uint3 seed =asuint(pos);
    return hash(seed).yzx;
}
inline float2 static_MNoise(float2 pos)
{
     uint2 seed =asuint(pos);
	return hash(seed).yx;
}
#endif
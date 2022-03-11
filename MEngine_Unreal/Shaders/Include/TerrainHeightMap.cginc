#ifndef __TERRAIN_HEIGHT_MAP_INCLUDE
#define __TERRAIN_HEIGHT_MAP_INCLUDE
#include "Sampler.cginc"
/*
cbuffer HeightMapParams
{
    float2 _HeightmapUVScale;
    float2 _HeightmapUVOffset;
	float2 _HeightScaleOffset;
	uint _HeightmapChunkSize;
};
Texture2D<float> _GreyTex;
StructuredBuffer<int> _HeightMapDefaultBuffer;
*/
float2 SampleTerrainNormalPoint(float2 localUV, ChunkData index, SamplerState samp)
{
    if(index.normalIndex < 0) return 0.5;
    float2 xz = _MainTex[index.normalIndex].SampleLevel(samp, localUV, 0).xy;
    return xz;
}
float SampleTerrainHeightPoint(float2 localUV, ChunkData index, SamplerState samp)
{
    if(index.texIndex < 0) return 0;
    return _GreyTex[index.texIndex].SampleLevel(samp, localUV, 0);
    
}
float2 SampleTerrainNormalPoint(float2 localUV, int2 chunk, SamplerState samp)
{
    float4 outOfRange = float4(chunk < 0, chunk >= _HeightmapChunkSize);
    if(dot(outOfRange, 1) > 0.5) return 0.5;
    ChunkData index = _HeightMapDefaultBuffer[chunk.y * _HeightmapChunkSize + chunk.x];
    return SampleTerrainNormalPoint(localUV, index, samp);
}
float SampleTerrainHeightPoint(float2 localUV, int2 chunk, SamplerState samp)
{
    float4 outOfRange = float4(chunk < 0, chunk >= _HeightmapChunkSize);
    if(dot(outOfRange, 1) > 0.5) return 0;
    ChunkData index = _HeightMapDefaultBuffer[chunk.y * _HeightmapChunkSize + chunk.x];
    return SampleTerrainHeightPoint(localUV, index, samp);
}

void GetNewBilinearUV(float2 localUV, int2 chunk, float4 texelSize, out float2 newLocalUV, out int2 newChunk, out float2 sampleWeight)
{
    float2 absoluteUV = frac(localUV * texelSize.zw);
    sampleWeight = abs(absoluteUV - 0.5);
    newLocalUV = lerp(localUV - texelSize.xy, localUV + texelSize.xy, absoluteUV > 0.5);
    newChunk = chunk;
    if(newLocalUV.x < texelSize.x * 0.5)
    {
        newLocalUV.x += 1;
        newChunk.x -= 1;
    }
    else if(newLocalUV.x > 1 - texelSize.x * 0.5)
    {
        newLocalUV.x -= 1;
        newChunk.x += 1;
    }
    if(newLocalUV.y < texelSize.y * 0.5)
    {
        newLocalUV.y += 1;
        newChunk.y -= 1;
    }
    else if(newLocalUV.y > 1 - texelSize.y * 0.5)
    {
        newLocalUV.y -= 1;
        newChunk.y += 1;
    }
}

float2 SampleTerrainNormal(float2 localUV, int2 chunk)
{
    ChunkData index = _HeightMapDefaultBuffer[chunk.y * _HeightmapChunkSize + chunk.x];
    float4 boundary = float4(index.texelSize.xy * 0.5, 1 - index.texelSize.xy * 0.5);
    float4 boundaryTest = float4(localUV < boundary.xy, localUV > boundary.zw);
    //Use Manually Bilinear
    if(dot(boundaryTest, 1) > 0.5)
    {
        float2 leftDown = SampleTerrainNormalPoint(localUV, index, pointClampSampler);
        float2 rightUpUV;
        int2 rightUpChunk;
        float2 sampleWeight;
        GetNewBilinearUV(localUV, chunk, index.texelSize, rightUpUV, rightUpChunk, sampleWeight);
        float2 rightDown = SampleTerrainNormalPoint(float2(rightUpUV.x, localUV.y), int2(rightUpChunk.x, chunk.y), pointClampSampler);
        float2 leftUp = SampleTerrainNormalPoint(float2(localUV.x, rightUpUV.y), int2(chunk.x, rightUpChunk.y), pointClampSampler);
        float2 rightUp = SampleTerrainNormalPoint(rightUpUV, rightUpChunk, pointClampSampler);
        float2 down = lerp(leftDown, rightDown, sampleWeight.x);
        float2 up = lerp(leftUp, rightUp, sampleWeight.x);
        return lerp(down, up, sampleWeight.y);
    }
    //Use Auto Bilinear
    else
    {
        return SampleTerrainNormalPoint(localUV, index, bilinearClampSampler);
    }
}

float SampleTerrainHeight(float2 localUV, int2 chunk)
{
    ChunkData index = _HeightMapDefaultBuffer[chunk.y * _HeightmapChunkSize + chunk.x];
    float4 boundary = float4(index.texelSize.xy * 0.5, 1 - index.texelSize.xy * 0.5);
    float4 boundaryTest = float4(localUV < boundary.xy, localUV > boundary.zw);
    //Use Manually Bilinear
    if(dot(boundaryTest, 1) > 0.5)
    {
        float leftDown = SampleTerrainHeightPoint(localUV, index, pointClampSampler);
        float2 rightUpUV;
        int2 rightUpChunk;
        float2 sampleWeight;
        GetNewBilinearUV(localUV, chunk, index.texelSize, rightUpUV, rightUpChunk, sampleWeight);
        float rightDown = SampleTerrainHeightPoint(float2(rightUpUV.x, localUV.y), int2(rightUpChunk.x, chunk.y), pointClampSampler);
        float leftUp = SampleTerrainHeightPoint(float2(localUV.x, rightUpUV.y), int2(chunk.x, rightUpChunk.y), pointClampSampler);
        float rightUp = SampleTerrainHeightPoint(rightUpUV, rightUpChunk, pointClampSampler);
        float down = lerp(leftDown, rightDown, sampleWeight.x);
        float up = lerp(leftUp, rightUp, sampleWeight.x);
        return lerp(down, up, sampleWeight.y) * _HeightScaleOffset.x + _HeightScaleOffset.y;
    }
    //Use Auto Bilinear
    else
    {
        return SampleTerrainHeightPoint(localUV, index, bilinearClampSampler) * _HeightScaleOffset.x + _HeightScaleOffset.y;
    }
}


float GetHeightMap(float2 localXZPos)
{
    float2 uv = localXZPos * _HeightmapUVScale + _HeightmapUVOffset;
    if((dot(uv < 0, 1) + dot(uv >= _HeightmapChunkSize, 1)) > 0.5)
    {
        return _HeightScaleOffset.y;
    }
    int2 chunk = (int2)uv;
    uv = frac(uv);
    return SampleTerrainHeight(uv, chunk);
}

float3 SampleTerrainNormal(float2 localXZPos)
{
 float2 uv = localXZPos * _HeightmapUVScale + _HeightmapUVOffset;
    if((dot(uv < 0, 1) + dot(uv >= _HeightmapChunkSize, 1)) > 0.5)
    {
        return float3(0, 1, 0);
    }
    int2 chunk = (int2)uv;
    uv = frac(uv);
    float2 xz = SampleTerrainNormal(uv, chunk);
    xz = xz * 2 - 1;
    float y = sqrt(1 - dot(xz, xz));
    return float3(xz.x, y, xz.y);
}



#endif
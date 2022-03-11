#pragma once

struct ExportTransform
{
    float3 position;
    float4 rotation;
    float3 localscale;
};

struct ExportLandscape
{
    vengine::string heightMap;
    int32_t weightMapCount;
    vengine::string* weightMaps;
};

struct ExportRenderer
{
    int32_t meshCount;
    float* meshDis;
    vengine::string* meshGUID;
    int32_t matCount;
    vengine::string* matGUID;
    int32_t mask;
};

/*
    当前与JsonRenderer结构一致，
    但它们是俩个组件，不宜耦合
    为了灵活性，可扩展性，将其分离
*/
struct ExportSkinnedRenderer
{
    int32_t meshCount;
    float* meshDis;
    vengine::string* meshGUID;
    int32_t matCount;
    vengine::string* matGUID;
    int32_t mask;
};

struct ExportAnimClip
{
    int32_t clipCount;
    vengine::string* clipName;
    vengine::string* clipGUID;
};

struct ExportLight
{
    int32_t lightType;
    float intensity;
    float shadowBias;
    float shadowNearPlane;
    float range;
    float angle;
    float4 color;
    float smallAngle;
    int32_t useShadow;
    int32_t shadowCache;
};

struct ExportReflectionProbe
{
    float blendDistance;
    float color;
    float3 localPosition;
    int32_t boxProjection;
    float3 localExtent;
    vengine::string cubemapGUID;
};
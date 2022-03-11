#ifndef _DECAL_INCLUDE__
#define _DECAL_INCLUDE__
#include "Defines.cginc"

#define DECAL_CBUFFER \
    float4 _CameraForward_Decal;\
	float4 _FrustumCorners[4];\
	uint2 _Resolution;\
	uint2 _TileSize;\
	uint _DecalCount;\
	uint _SRV_DecalCullResult;
    
struct Decal
{
    float3x4 localToWorldMat;
	float3x4 worldToLocal;
	float4 albedoScaleOffset;
	float4 normalScaleOffset;
	float4 specularScaleOffset;
	float3 minPosition;
	float3 maxPosition;
	float3 opacity;
	int3 texIndex;
	uint layer;
	float2 heightScaleOffset;
};
#ifdef GBUFFER_SHADER
void CalculateDecal(
	Texture3D<uint> _DecalTile,
	StructuredBuffer<Decal> _AllDecals,
	float2 _TileSize,
	float2 uv, 
	uint layer, 
	float3 worldPos,
	float height, 
	inout float3 color, 
	inout float3 normal, 
	inout float3 specular, 
	inout float smoothness, 
	inout float occlusion)
{
	uint2 id = (uint2)(uv * _TileSize);
    uint count = _DecalTile[uint3(id, 0)];
    [loop]
    for(uint i = 1; i < count; ++i)
    {
        Decal data = _AllDecals[_DecalTile[uint3(id, i)]];
        if((data.layer & layer) == 0) continue;
        float3 localPos = mul(data.worldToLocal, float4(worldPos, 1));
        float3 lp = localPos.xyz + 0.5;
		lp.y = 1-lp.y;
        if(dot(abs(lp - saturate(lp)), 1) > 1e-5) continue;
        float4 uv = lp.xyxy * float4(data.albedoScaleOffset.xy, data.normalScaleOffset.xy) + float4(data.albedoScaleOffset.zw, data.normalScaleOffset.zw);
        uv.yw = 1 - uv.yw;
        float4 col;
        float normalWeight;
        if(data.texIndex.x >= 0){
        	col = _MainTex[data.texIndex.x].Sample(anisotropicWrapSampler, uv.xy);
        	normalWeight = col.a;
        }
        else {
            col = 0;
            normalWeight = 1;
        }
        float heightMask = saturate(height * data.heightScaleOffset.x + data.heightScaleOffset.y);
        normalWeight *= heightMask; 
        col.a *= heightMask;
        if(data.texIndex.y >= 0){
        float2 texNorm = _MainTex[data.texIndex.y].Sample(anisotropicWrapSampler, uv.zw);
        float normZ = sqrt(abs(1 - dot(texNorm,texNorm)));
        normal = lerp(normal,  float3(texNorm, normZ), normalWeight * data.opacity.y);
        }
        if(data.texIndex.z >= 0)
        {
            float2 specUV = lp.xy * data.specularScaleOffset.xy + data.specularScaleOffset.zw;
            specUV.y = 1 - specUV.y;
            float4 specSmo = _MainTex[data.texIndex.z].Sample(anisotropicWrapSampler, specUV);
            float specWeight = normalWeight * data.opacity.z;
            float specValue = specSmo.a * 0.08;
            float3 spec = lerp(specValue, col.xyz, specSmo.g);
            col.xyz *= lerp(1 - specValue, 0, specSmo.g);
            specular = lerp(specular, spec, specWeight);
            smoothness = lerp(smoothness, specSmo.r, specWeight);
            occlusion = lerp(occlusion, specSmo.b, specWeight);
        }
        color = lerp(color, col.rgb, col.a * data.opacity.x);
	}
}
#endif
#endif
#ifndef __VOXELLIGHT_INCLUDE__
#define __VOXELLIGHT_INCLUDE__
#include "Common.hlsl"
#include "Random.cginc"
#include "Montcalo_Library.hlsl"
#include "BSDF_Library.hlsl"
#include "ShadingModel.hlsl"
#include "AreaLight.hlsl"
#include "Defines.cginc"
static const float _ShadowSampler = 12.0;

struct LightCommand{
	float4x4 spotVPMatrix;
   	float3 direction;
	int shadowmapIndex;
	float3 lightColor;
	uint lightType;
	float3 position;
	float spotAngle;
	float shadowBias;
	float shadowNearPlane;
	float range;
	float spotRadius;
	float spotSmallAngle;
};

struct ReflectionProbe
{
	float3 position;
	float3 minExtent;
	float3 maxExtent;
	uint cubemapIndex;
};

uint GetIndex(uint3 id, uint3 size, uint multiply){
	uint3 multiValue = uint3(1, size.x, size.x * size.y) * multiply;
    return (uint)dot(id, multiValue);
}

#ifdef GBUFFER_SHADER
float3 CalculateLocalLight(
	float3 shadowNormal,
	float2 screenUV, 
	float3 WorldPos, 
	float linearDepth, 
	float3 WorldNormal,
	float3 ViewDir, 
	float cameraNear, 
	float lightFar,
	float3 albedo,
	float3 specular,
	float roughness,
	StructuredBuffer<uint> lightIndexBuffer,
	StructuredBuffer<LightCommand> lightBuffer)
{
	float3 ShadingColor = 0;
	float rate = pow((linearDepth - cameraNear) /(lightFar - cameraNear), 1.0 / CLUSTERRATE);
    if(isinf(rate) || isnan(rate) || rate <= 0 || rate >= 0.9999) return 0;
	
	uint3 voxelValue = uint3((uint2)(screenUV * float2(XRES, YRES)), (uint)(rate * ZRES));
	uint sb = GetIndex(voxelValue, VOXELSIZE, (MAXLIGHTPERCLUSTER + 1));
	if(sb > (XRES * YRES * ZRES - 1) *  (MAXLIGHTPERCLUSTER + 1)) return 0;
	uint c;

	float2 JitterSpot = screenUV;
	float3 JitterPoint = float3(1 - screenUV, linearDepth);
	uint2 LightIndex = uint2(sb + 1, lightIndexBuffer[sb]);	// = uint2(sb + 1, _PointLightIndexBuffer[sb]);
	BSDFContext LightData = (BSDFContext)0;
	InitGeoData(LightData, WorldNormal, ViewDir);
	if(LightIndex.y - LightIndex.x > MAXLIGHTPERCLUSTER) return float3(10000, 0, 10000);
	[loop]
	for (c = LightIndex.x; c < LightIndex.y; c++)
	{
	//	return lightIndexBuffer[c] * 1000;//
		LightCommand lightCommand = lightBuffer[lightIndexBuffer[c]];
		//Point Light
		if(lightCommand.lightType == 0)
		{
			float LightRange = lightCommand.range;
			float3 LightPos = lightCommand.position;
			float3 LightColor = lightCommand.lightColor;
			float3 Un_LightDir = LightPos - WorldPos.xyz;
			float Length_LightDir = length(Un_LightDir);
			float3 LightDir = Un_LightDir / Length_LightDir;
			float3 halfDir = normalize(ViewDir + LightDir);
			InitLightingData(LightData, WorldNormal, ViewDir, LightDir, halfDir);
			float3 Energy = Point_Energy(Un_LightDir, LightColor, 1 / LightRange);
			if(dot(Energy, 1) < 1e-5) continue;
			const float ShadowResolution = 128;
			//Calculate Shadowmap
			float shadowValue = 1;
			if(lightCommand.shadowmapIndex >= 0)
			{
				float3 shadowWorldPos = WorldPos.xyz + shadowNormal * lightCommand.shadowBias;
				float3 Shadow_Un_LightDir = LightPos - shadowWorldPos;
				float3 Shadow_Length_LightDir = length(Shadow_Un_LightDir);
				float3 Shadow_LightDir = Shadow_Un_LightDir / Shadow_Length_LightDir;

				float shadowSampleDist = Shadow_Length_LightDir / LightRange;
				TextureCube<float> shadowmap = _GreyCubemap[lightCommand.shadowmapIndex];
				float softValue = ShadowResolution / lerp(0, 1, pow(LightData.NoL, 0.33333333333));
				shadowValue = 0;
				[loop]
				for(int i = 0; i < _ShadowSampler; ++i)
				{
					JitterPoint = MNoise(JitterPoint, _RandomSeed) * 2 - 1;
					shadowValue += shadowmap.SampleCmpLevelZero( cubemapShadowSampler, -(Shadow_LightDir + ( JitterPoint /  softValue)), shadowSampleDist);
				}
				shadowValue /= _ShadowSampler;
			}	
			ShadingColor += max(0, Defult_Lit(LightData, Energy, albedo, specular, roughness)) * shadowValue;
		}
		//Spot Light
		else
		{
			float LightRange = lightCommand.range;
			float3 LightPos = lightCommand.position;
			float3 LightColor = lightCommand.lightColor;
			//int iesIndex = Light.iesIndex;
			float LightAngle = cos(lightCommand.spotAngle);
			float3 LightForward = lightCommand.direction;
			float3 Un_LightDir = LightPos - WorldPos.xyz;
			float lightDirLen = length(Un_LightDir);
			float3 LightDir = Un_LightDir / lightDirLen;
			float3 halfDir = normalize(ViewDir + LightDir);
			float ldh = -dot(LightDir, lightCommand.direction);
			float isNear = dot(-Un_LightDir, lightCommand.direction) > lightCommand.shadowNearPlane;
			InitLightingData(LightData, WorldNormal, ViewDir, LightDir, halfDir);
			float3 Energy = Spot_Energy(ldh, lightDirLen, LightColor, cos(lightCommand.spotSmallAngle), LightAngle, 1.0 / LightRange) * isNear;
			if(dot(Energy, 1) < 1e-5) continue;
			float shadowValue = 1;
			if(lightCommand.shadowmapIndex >= 0)
			{
				float3 shadowWorldPos = WorldPos.xyz + shadowNormal * lightCommand.shadowBias;
				float3 Shadow_LightDir = normalize(LightPos - shadowWorldPos);

				Texture2D<float> shadowmap = _GreyTex[lightCommand.shadowmapIndex];
				float4 shadowPos = mul(lightCommand.spotVPMatrix, float4(shadowWorldPos, 1));
				shadowPos /= float4(shadowPos.w, -shadowPos.w, shadowPos.ww);
				shadowPos.xy = shadowPos.xy * 0.5 + 0.5;
				shadowValue = 0;
				const float ShadowResolution = 1.0 / 256.0;
				float softValue = ShadowResolution * lerp(0, 1, pow(LightData.NoL, 0.33333333333333));
				shadowPos.z = clamp(shadowPos.z, 1e-5, 0.9999);
				[loop]
				for(int i = 0; i < _ShadowSampler; ++i)
				{
					JitterSpot = MNoise(JitterSpot, _RandomSeed);
					float2 shadowOffset = UniformSampleDiskConcentricApprox(JitterSpot);
					shadowValue += shadowmap.SampleCmpLevelZero(linearShadowSampler, shadowPos.xy + (shadowOffset * softValue), shadowPos.z);
				}
				shadowValue /= _ShadowSampler;
			}
			ShadingColor += max(0, Defult_Lit(LightData, Energy, albedo, specular, roughness) * shadowValue);
		}
	}
	return ShadingColor;
}

float3 CalculateSunLight_NoShadow(float3 N, float3 V, float3 L, float3 col, float3 AlbedoColor, float3 SpecularColor, float3 Roughness, inout BSDFContext LightData)
{
	float3 H = normalize(V + L);
	InitGeoData(LightData, N, V);
	InitLightingData(LightData, N, V, L, H);
	return Defult_Lit(LightData, col,  AlbedoColor, SpecularColor, Roughness);

}
#endif
#ifdef FROXEL
/*
float3 RayleighScatter(float3 ray, float lengthRay, float HeightFalloff, float RayleighScale)
{	
	float Falloff = exp( -HeightFalloff * abs(worldSpaceCameraPos.y) );
	float densityIntegral = lengthRay * Falloff;
    [flatten]
	if (abs(ray.y) > 0.001)
	{
		float t = HeightFalloff * ray.y;
		t = abs(t) < 0.00001 ? 0.00001 : t;
		densityIntegral *= ( 1.0 - exp(-t) ) / t;
	}

	return  ( RayleighScale * float3(0.00116, 0.0027, 0.00662) ) * densityIntegral * (1.0 / 4.0 * 3.14);
}*/
#define MieScattering(cosAngle, g) g.w * (g.x / (pow(g.y - g.z * cosAngle, 1.25)))
// x: 1 - g^2, y: 1 + g^2, z: 2*g, w: 1/4pi   g = 0.36
static const float4 _MieG = float4(0.8704, 1.1296,0.72,0.7853981633974483);

float3 CalculateFroxelLight(
	float2 screenUV, 
	float3 WorldPos, 
	float linearDepth,
	float3 ViewDir, 
	float cameraNear, 
	float lightFar,
	StructuredBuffer<uint> lightIndexBuffer,
	StructuredBuffer<LightCommand> lightBuffer)
{
	float3 color = 0;
	screenUV.y = 1 - screenUV.y;
	float3 ShadingColor = 0;
	float rate = pow((linearDepth - cameraNear) /(lightFar - cameraNear), 1.0 / CLUSTERRATE);
    if(isinf(rate) || isnan(rate) || rate <= 0 || rate >= 0.9999) return 0;
	
	uint3 voxelValue = uint3((uint2)(screenUV * float2(XRES, YRES)), (uint)(rate * ZRES));
	uint sb = GetIndex(voxelValue, VOXELSIZE, (MAXLIGHTPERCLUSTER + 1));
	if(sb > (XRES * YRES * ZRES - 1) *  (MAXLIGHTPERCLUSTER + 1)) return 0;
	uint c;

	float2 JitterSpot = screenUV;
	uint2 LightIndex = uint2(sb + 1, lightIndexBuffer[sb]);
	if(LightIndex.y - LightIndex.x > MAXLIGHTPERCLUSTER) return float3(10000, 0, 10000);
	[loop]
	for (c = LightIndex.x; c < LightIndex.y; c++)
	{
		LightCommand lightCommand = lightBuffer[lightIndexBuffer[c]];
		//Point Light
		if(lightCommand.lightType == 0)
		{
			float3 lightDir = lightCommand.position - WorldPos;
			float lenOfLightDir = length(lightDir);
			if(lenOfLightDir > lightCommand.range) continue;
			//TODO IES
			float3 currentCol = DistanceFalloff(lightDir, (1 / lightCommand.range)) * MieScattering(dot(lightDir / lenOfLightDir, ViewDir), _MieG) * lightCommand.lightColor;
			if(dot(currentCol, 1) < 1e-5) continue;
			//TODO
			//Shadowmap
			float shadowValue = 1;
			if(lightCommand.shadowmapIndex >= 0)
			{
				float shadowSampleDist = (lenOfLightDir / lightCommand.range);
				TextureCube<float> shadowmap = _GreyCubemap[lightCommand.shadowmapIndex];
				shadowValue = shadowmap.SampleCmpLevelZero(cubemapShadowSampler, -(lightDir), shadowSampleDist);
			}
			color.rgb += currentCol * shadowValue;
		}
		//Spot Light
		else
		{
			float LightRange = lightCommand.range;
            float3 LightPos = lightCommand.position;
            float LightAngle = lightCommand.spotAngle;
            float3 LightForward = lightCommand.direction;
            float3 Un_LightDir = LightPos - WorldPos;
            float lightDirLength = length(Un_LightDir);
            float3 lightDir = Un_LightDir / lightDirLength;
            float ldf = -dot(lightDir, LightForward);
            float2 SpotConeAngle = float2(cos(LightAngle), cos(lightCommand.spotSmallAngle));
            if(ldf < SpotConeAngle.x || lightCommand.range / ldf < lightDirLength) continue;
            float lightAngleScale = 1 / max ( 0.001, (SpotConeAngle.y - SpotConeAngle.x) );
            float lightAngleOffset = -SpotConeAngle.x * lightAngleScale;
            float SpotFalloff = AngleFalloff(ldf, lightAngleScale, lightAngleOffset);
			//TODO IES
			float isNear =  dot(-Un_LightDir, lightCommand.direction) > lightCommand.shadowNearPlane;
            float ShadowTrem = 1;
			if(lightCommand.shadowmapIndex >= 0)
			{
				Texture2D<float> shadowmap = _GreyTex[lightCommand.shadowmapIndex];
				float4 shadowPos = mul(lightCommand.spotVPMatrix, float4(WorldPos, 1));
				shadowPos /= float4(shadowPos.w, -shadowPos.w, shadowPos.ww);
				shadowPos.xy = shadowPos.xy * 0.5 + 0.5;
				ShadowTrem = shadowmap.SampleCmpLevelZero(linearShadowSampler, shadowPos.xy, shadowPos.z);
			}
            float3 spotColor = SpotFalloff * DistanceFalloff(Un_LightDir, (1 / LightRange)) * MieScattering(-dot(lightDir, ViewDir), _MieG) * lightCommand.lightColor * isNear * ShadowTrem;
			color.rgb += spotColor;
		}
	}
	return color;
}

#endif

#endif
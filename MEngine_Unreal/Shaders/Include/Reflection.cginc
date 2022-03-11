#ifndef REFLECTION_INCLUDE
#define REFLECTION_INCLUDE
#define REFLECTION_LOD 6
struct ReflectionData
{
    float3 position;
    float blendDistance;
    float3 minExtent;
    uint boxProjection;
    float4 hdr;
    float3 maxExtent;
    uint texIndex;
};

struct ProbeResult
{
    float3 worldPos;
    float3 worldViewDir;
    float4 boxMin;

    float4 boxMax;
    float4 probePosition;
    // HDR cubemap properties, use to decompress HDR texture
    float4 probeHDR;
};

inline float3 BoxProjectedCubemapDirection (float3 worldRefl, float3 worldPos, float4 cubemapCenter, float4 boxMin, float4 boxMax)
{

        float3 nrdir = normalize(worldRefl);

        #if 1
            float3 rbmax = (boxMax.xyz - worldPos) / nrdir;
            float3 rbmin = (boxMin.xyz - worldPos) / nrdir;

            float3 rbminmax = (nrdir > 0.0f) ? rbmax : rbmin;

        #else // Optimized version
            float3 rbmax = (boxMax.xyz - worldPos);
            float3 rbmin = (boxMin.xyz - worldPos);

            float3 select = step (float3(0,0,0), nrdir);
            float3 rbminmax = lerp (rbmax, rbmin, select);
            rbminmax /= nrdir;
        #endif

        float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);

        worldPos -= cubemapCenter.xyz;
        worldRefl = worldPos + nrdir * fa;
    
    return worldRefl;
}

float3 VEngine_IndirectSpecular_Deferred(ProbeResult data, float3 reflDir, ReflectionData reflData, float lod, TextureCube<float4> targetTex)
{
    if(reflData.boxProjection)
    {
        reflDir = BoxProjectedCubemapDirection (reflDir, data.worldPos, data.probePosition, data.boxMin, data.boxMax);
    }
    return targetTex.SampleLevel(trilinearWrapSampler, reflDir, lod).xyz * reflData.hdr;
}
void VEngine_IndirectSpecularDiffuse_Deferred(ProbeResult data, float3 reflDir, ReflectionData reflData, float lod, TextureCube<float4> targetTex, out float3 diffuse, out float3 specular)
{
    if(reflData.boxProjection)
    {
        reflDir = BoxProjectedCubemapDirection (reflDir, data.worldPos, data.probePosition, data.boxMin, data.boxMax);
    }
    specular = targetTex.SampleLevel(trilinearWrapSampler, reflDir, lod).xyz * reflData.hdr;
    diffuse = targetTex.SampleLevel(bilinearWrapSampler, reflDir, REFLECTION_LOD) * reflData.hdr;
}

bool IsCubemapInRange(
    ReflectionData data,
    float3 worldPos
){
    float3 worldToPoint = worldPos - data.position;
	
    if (dot((abs(worldToPoint) - data.maxExtent) > 0, 1) > 0.5) 
    {
        return false;
    }
    return true;
}

float roughnessToMip(float roughness)
{
    float m = roughness * roughness; // m is the real roughness parameter
    const float fEps = 1.192092896e-07F;        // smallest such that 1.0+FLT_EPSILON != 1.0  (+1e-4h is NOT good here. is visibly very wrong)
    float n =  (2.0/max(fEps, m*m))-2.0;        // remap to spec power. See eq. 21 in --> https://dl.dropboxusercontent.com/u/55891920/papers/mm_brdf.pdf
    n /= 4;                                     // remap from n_dot_h formulatino to n_dot_r. See section "Pre-convolved Cube Maps vs Path Tracers" --> https://s3.amazonaws.com/docs.knaldtech.com/knald/1.0.0/lys_power_drops.html
    float perceptualRoughness = pow( 2/(n+2), 0.25);      // remap back to square root of real roughness (0.25 include both the sqrt root of the conversion and sqrt for going from roughness to perceptualRoughness)
    //float perceptualRoughness = roughness * (1.7 - 0.7 * roughness);
    float lodLevel = perceptualRoughness * REFLECTION_LOD;
    return lodLevel;
}

float4 CalculateReflection(
    ReflectionData data,
    float3 worldPos,
    float3 viewDir,
    float3 worldNormal,
    float3 specular,
    float smoothness)
{
    TextureCube<float4> cubemap = _Cubemap[data.texIndex];
    float roughness = 1 - smoothness;
    float lodLevel = roughnessToMip(roughness);
    ProbeResult d;
    d.worldPos = worldPos.xyz;
	d.worldViewDir = -viewDir;
    d.probeHDR = data.hdr;
	if (data.boxProjection)
	{
		d.probePosition = float4(data.position, 1);
		d.boxMin.xyz = data.position - data.maxExtent;
		d.boxMax.xyz = (data.position + data.maxExtent);
	}
    float3 specColor = VEngine_IndirectSpecular_Deferred(d, -reflect(-viewDir, worldNormal), data, lodLevel, cubemap);
    float3 distanceToMin = saturate((abs(worldPos.xyz - data.position) - data.minExtent) / data.blendDistance);
    float lerpValue = max(distanceToMin.x, max(distanceToMin.y, distanceToMin.z));
    return float4(specColor * specular,1 - lerpValue);
}

void CalculateReflection_DiffuseGI(
    ReflectionData data,
    float3 worldPos,
    float3 viewDir,
    float3 albedo,
    float3 worldNormal,
    float3 specular,
    float smoothness,
    out float4 albedoResult,
    out float4 specularResult)
{
    TextureCube<float4> cubemap = _Cubemap[data.texIndex];
    float roughness = 1 - smoothness;
    float lodLevel = roughnessToMip(roughness);
    ProbeResult d;
    d.worldPos = worldPos.xyz;
	d.worldViewDir = -viewDir;
    d.probeHDR = data.hdr;
	if (data.boxProjection)
	{
		d.probePosition = float4(data.position, 1);
		d.boxMin.xyz = data.position - data.maxExtent;
		d.boxMax.xyz = (data.position + data.maxExtent);
	}
    float3 specColor, diffColor;
    VEngine_IndirectSpecularDiffuse_Deferred(d, -reflect(-viewDir, worldNormal), data, lodLevel, cubemap, diffColor, specColor);
    float3 distanceToMin = saturate((abs(worldPos.xyz - data.position) - data.minExtent) / data.blendDistance);
    float lerpValue = max(distanceToMin.x, max(distanceToMin.y, distanceToMin.z));
    lerpValue = 1 - lerpValue;
    specularResult = float4(specColor * specular,lerpValue);
    albedoResult = float4(diffColor * albedo, lerpValue);
}


float3 CalculateSkyboxReflection(
    float3 viewDir,
    float3 worldNormal,
    float3 specular,
    float smoothness,
    TextureCube<float4> cubemap)
    {
    float roughness = 1 - smoothness;
    float lodLevel = roughnessToMip(roughness);
    float3 reflDir = -reflect(-viewDir, worldNormal);
    return cubemap.SampleLevel(trilinearWrapSampler, reflDir, lodLevel) * specular;
    
    }

void CalculateSkyboxReflection_Diffuse(
    float3 viewDir,
    float3 worldNormal,
    float3 albedo,
    float3 specular,
    float smoothness,
    TextureCube<float4> cubemap,
    out float3 diffResult,
    out float3 specResult)
    {
    float roughness = 1 - smoothness;
    float lodLevel = roughnessToMip(roughness);
    float3 reflDir = -reflect(-viewDir, worldNormal);
    specResult = cubemap.SampleLevel(trilinearWrapSampler, reflDir, lodLevel) * specular;
    diffResult = cubemap.SampleLevel(bilinearWrapSampler, reflDir, REFLECTION_LOD) * albedo;    
}

#endif
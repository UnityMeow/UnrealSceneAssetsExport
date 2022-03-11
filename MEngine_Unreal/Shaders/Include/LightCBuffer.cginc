#ifndef LIGHT_CBUFFER_INCLUDE__
#define LIGHT_CBUFFER_INCLUDE__

#define LIGHT_CBUFFER\
    float4 _CameraNearPos;\
	float4 _CameraFarPos;\
	float3 _CameraForward;\
	uint _LightCount;\
	float3 _SunColor;\
	uint _SunEnabled;\
	float3 _SunDir;\
	uint _SunShadowEnabled;\
	uint4 _ShadowmapIndices;\
	float4 _CascadeDistance;\
	float4x4 _ShadowMatrix[4];\
	float4 _ShadowSoftValue;\
	float4 _ShadowOffset;\
	uint _ReflectionProbeCount;
	
#endif
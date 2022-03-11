#ifndef __PER_CAMERA_BUFFER_INCLUDE
#define __PER_CAMERA_BUFFER_INCLUDE

#define PER_CAMERA_BUFFER \
	float4x4 _WorldToCamera; \
	float4x4 _InverseWorldToCamera; \
	float4x4 _Proj; \
	float4x4 _InvProj; \
	float4x4 _VP; \
	float4x4 _InvVP; \
	float4x4 _NonJitterVP;\
	float4x4 _NonJitterInverseVP;\
	float4x4 _LastVP;\
	float4x4 _InverseLastVP;\
	float4x4 _FlipProj; \
	float4x4 _FlipInvProj; \
	float4x4 _FlipVP; \
	float4x4 _FlipInvVP; \
	float4x4 _FlipNonJitterVP; \
	float4x4 _FlipNonJitterInverseVP; \
	float4x4 _FlipLastVP; \
	float4x4 _FlipInverseLastVP; \
	float4 _ZBufferParams; \
	float4 _RandomSeed; \
	float3 worldSpaceCameraPos; \
	float _NearZ; \
	float _FarZ;
#endif
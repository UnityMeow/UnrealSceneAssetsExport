#pragma once
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "../DLL.h"
using namespace DirectX;
inline float XM_CALLCONV CombineVector4(const XMVECTOR& V2)
{
	XMVECTOR vTemp2 = _mm_shuffle_ps(V2, V2, _MM_SHUFFLE(1, 0, 0, 0)); // Copy X to the Z position and Y to the W position
	vTemp2 = _mm_add_ps(vTemp2, V2);          // Add Z = X+Z; W = Y+W;
	return vTemp2.m128_f32[2] + vTemp2.m128_f32[3];
}

inline float XM_CALLCONV dot(const Math::Vector4& vec, const Math::Vector4& vec1)
{
	return CombineVector4(vec * vec1);
}

inline void XM_CALLCONV Float4x4ToFloat4x3(const float4x4& f, float4x3& result)
{
	memcpy(&result._11, &f._11, sizeof(float3));
	memcpy(&result._21, &f._21, sizeof(float3));
	memcpy(&result._31, &f._31, sizeof(float3));
	memcpy(&result._41, &f._41, sizeof(float3));
}

inline void XM_CALLCONV Float4x3ToFloat4x4(const float4x3& f, float4x4& result)
{
	memcpy(&result._11, &f._11, sizeof(float3));
	memcpy(&result._21, &f._21, sizeof(float3));
	memcpy(&result._31, &f._31, sizeof(float3));
	memcpy(&result._41, &f._41, sizeof(float3));
	result._14 = 0;
	result._24 = 0;
	result._34 = 0;
	result._44 = 1;
}

inline float4 MatroxToQuaternion(float4x4 const& m)
{
	float tr = m.m[0][0] + m.m[1][1] + m.m[2][2];
	float4 q = float4(0, 0, 0, 0);

	if (tr > 0)
	{
		float s = sqrt(tr + 1.0) * 2; // S=4*qw 
		q.w = 0.25 * s;
		q.x = (m.m[2][1] - m.m[1][2]) / s;
		q.y = (m.m[0][2] - m.m[2][0]) / s;
		q.z = (m.m[1][0] - m.m[0][1]) / s;
}
	else if ((m.m[0][0] > m.m[1][1]) && (m.m[0][0] > m.m[2][2]))
	{
		float s = sqrt(1.0 + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2; // S=4*qx 
		q.w = (m.m[2][1] - m.m[1][2]) / s;
		q.x = 0.25 * s;
		q.y = (m.m[0][1] + m.m[1][0]) / s;
		q.z = (m.m[0][2] + m.m[2][0]) / s;
	}
	else if (m.m[1][1] > m.m[2][2])
	{
		float s = sqrt(1.0 + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2; // S=4*qy
		q.w = (m.m[0][2] - m.m[2][0]) / s;
		q.x = (m.m[0][1] + m.m[1][0]) / s;
		q.y = 0.25 * s;
		q.z = (m.m[1][2] + m.m[2][1]) / s;
	}
	else
	{
		float s = sqrt(1.0 + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2; // S=4*qz
		q.w = (m.m[1][0] - m.m[0][1]) / s;
		q.x = (m.m[0][2] + m.m[2][0]) / s;
		q.y = (m.m[1][2] + m.m[2][1]) / s;
		q.z = 0.25 * s;
	}

	return q;
}

inline Math::Vector4 XM_CALLCONV abs(const Math::Vector4& vec)
{
	XMVECTOR& V = (XMVECTOR&)vec;
#if defined(_XM_NO_INTRINSICS_)
	XMVECTORF32 vResult = { { {
			fabsf(V.vector4_f32[0]),
			fabsf(V.vector4_f32[1]),
			fabsf(V.vector4_f32[2]),
			fabsf(V.vector4_f32[3])
		} } };
	return vResult.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
	return vabsq_f32(V);
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR vResult = _mm_setzero_ps();
	vResult = _mm_sub_ps(vResult, V);
	vResult = _mm_max_ps(vResult, V);
	return vResult;
#endif
}

inline Math::Vector3 XM_CALLCONV abs(const Math::Vector3& vec)
{
	XMVECTOR& V = (XMVECTOR&)vec;
#if defined(_XM_NO_INTRINSICS_)
	XMVECTORF32 vResult = { { {
			fabsf(V.vector4_f32[0]),
			fabsf(V.vector4_f32[1]),
			fabsf(V.vector4_f32[2]),
			fabsf(V.vector4_f32[3])
		} } };
	return vResult.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
	return vabsq_f32(V);
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR vResult = _mm_setzero_ps();
	vResult = _mm_sub_ps(vResult, V);
	vResult = _mm_max_ps(vResult, V);
	return vResult;
#endif
}

inline Math::Vector4 XM_CALLCONV clamp(
	const Math::Vector4& v,
	const Math::Vector4& min,
	const Math::Vector4& max)
{
	FXMVECTOR& V = (FXMVECTOR&)v;
	FXMVECTOR& Min = (FXMVECTOR&)min;
	FXMVECTOR& Max = (FXMVECTOR&)max;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTOR Result;
	Result = XMVectorMax(Min, V);
	Result = XMVectorMin(Max, Result);
	return Result;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTOR vResult;
	vResult = vmaxq_f32(Min, V);
	vResult = vminq_f32(Max, vResult);
	return vResult;
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR vResult;
	vResult = _mm_max_ps(Min, V);
	vResult = _mm_min_ps(Max, vResult);
	return vResult;
#endif
}

inline Math::Vector3 XM_CALLCONV clamp(
	const Math::Vector3& v,
	const Math::Vector3& min,
	const Math::Vector3& max)
{
	FXMVECTOR& V = (FXMVECTOR&)v;
	FXMVECTOR& Min = (FXMVECTOR&)min;
	FXMVECTOR& Max = (FXMVECTOR&)max;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTOR Result;
	Result = XMVectorMax(Min, V);
	Result = XMVectorMin(Max, Result);
	return Result;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTOR vResult;
	vResult = vmaxq_f32(Min, V);
	vResult = vminq_f32(Max, vResult);
	return vResult;
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR vResult;
	vResult = _mm_max_ps(Min, V);
	vResult = _mm_min_ps(Max, vResult);
	return vResult;
#endif
}

inline Math::Vector4 XM_CALLCONV lerp(const Math::Vector4& aa, const Math::Vector4& bb, float t)
{
	XMVECTOR& V0 = (XMVECTOR&)aa;
	XMVECTOR& V1 = (XMVECTOR&)bb;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTOR Scale = XMVectorReplicate(t);
	XMVECTOR Length = XMVectorSubtract(V1, V0);
	return XMVectorMultiplyAdd(Length, Scale, V0);

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTOR L = vsubq_f32(V1, V0);
	return vmlaq_n_f32(V0, L, t);
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR L = _mm_sub_ps(V1, V0);
	XMVECTOR S = _mm_set_ps1(t);
	XMVECTOR Result = _mm_mul_ps(L, S);
	return _mm_add_ps(Result, V0);
#endif
}
inline Math::Vector3 XM_CALLCONV lerp(const Math::Vector3& aa, const Math::Vector3& bb, float t)
{
	XMVECTOR& V0 = (XMVECTOR&)aa;
	XMVECTOR& V1 = (XMVECTOR&)bb;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTOR Scale = XMVectorReplicate(t);
	XMVECTOR Length = XMVectorSubtract(V1, V0);
	return XMVectorMultiplyAdd(Length, Scale, V0);

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTOR L = vsubq_f32(V1, V0);
	return vmlaq_n_f32(V0, L, t);
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR L = _mm_sub_ps(V1, V0);
	XMVECTOR S = _mm_set_ps1(t);
	XMVECTOR Result = _mm_mul_ps(L, S);
	return _mm_add_ps(Result, V0);
#endif
}

inline Math::Vector4 XM_CALLCONV lerp(const Math::Vector4& aa, const Math::Vector4& bb, const Math::Vector4& t)
{
	XMVECTOR& V0 = (XMVECTOR&)aa;
	XMVECTOR& V1 = (XMVECTOR&)bb;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTOR Scale = (XMVECTOR&)t;
	XMVECTOR Length = XMVectorSubtract(V1, V0);
	return XMVectorMultiplyAdd(Length, Scale, V0);

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTOR L = vsubq_f32(V1, V0);
	return vmlaq_n_f32(V0, L, t);
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR L = _mm_sub_ps(V1, V0);
	XMVECTOR& S = (XMVECTOR&)t;
	XMVECTOR Result = _mm_mul_ps(L, S);
	return _mm_add_ps(Result, V0);
#endif
}
inline float XM_CALLCONV lerp(float aa, float bb, float cc)
{
	float l = bb - aa;
	return aa + l * cc;
}
inline Math::Vector3 XM_CALLCONV lerp(const Math::Vector3& aa, const Math::Vector3& bb, const Math::Vector3& t)
{
	XMVECTOR& V0 = (XMVECTOR&)aa;
	XMVECTOR& V1 = (XMVECTOR&)bb;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTOR Scale = (XMVECTOR&)t;
	XMVECTOR Length = XMVectorSubtract(V1, V0);
	return XMVectorMultiplyAdd(Length, Scale, V0);

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTOR L = vsubq_f32(V1, V0);
	return vmlaq_n_f32(V0, L, t);
#elif defined(_XM_SSE_INTRINSICS_)
	XMVECTOR L = _mm_sub_ps(V1, V0);
	XMVECTOR& S = (XMVECTOR&)t;
	XMVECTOR Result = _mm_mul_ps(L, S);
	return _mm_add_ps(Result, V0);
#endif
}

inline float XM_CALLCONV CombineVector3(const XMVECTOR& v)
{
	return v.m128_f32[0] + v.m128_f32[1] + v.m128_f32[2];
}

inline float XM_CALLCONV dot(const Math::Vector3& vec, const Math::Vector3& vec1)
{
	return CombineVector3(vec * vec1);
}
inline int2 mul(const int2& a, const int2& b)
{
	return int2(a.x * b.x, a.y * b.y);
}

inline uint2 mul(const uint2& a, const uint2& b)
{
	return { a.x * b.x, a.y * b.y };
}

inline int3 mul(const int3& a, const int3& b)
{
	return { a.x * b.x, a.y * b.y , a.z * b.z };
}

inline uint3 mul(const uint3& a, const uint3& b)
{
	return { a.x * b.x, a.y * b.y ,a.z * b.z };
}

inline int4 mul(const int4& a, const int4& b)
{
	return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}

inline uint4 mul(const uint4& a, const uint4& b)
{
	return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}

inline float2 mul(const float2& a, const float2& b)
{
	return { a.x * b.x, a.y * b.y };
}


inline float3 mul(const float3& a, const float3& b)
{
	return { a.x * b.x, a.y * b.y , a.z * b.z };
}



inline float4 mul(const float4& a, const float4& b)
{
	return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}



inline Math::Vector4 XM_CALLCONV mul(const Math::Matrix4& m, const Math::Vector4& vec)
{
	Math::Matrix4& mat = (Math::Matrix4&)m;
	return {
		dot(mat[0], vec),
		dot(mat[1], vec),
		dot(mat[2], vec),
		dot(mat[3], vec)
	};
}

inline Math::Vector3 XM_CALLCONV mul(const Math::Matrix3& m, const  Math::Vector3& vec)
{
	Math::Matrix4& mat = (Math::Matrix4&)m;
	return {
		CombineVector3(mat[0] * vec),
		CombineVector3(mat[1] * vec),
		CombineVector3(mat[2] * vec)
	};
}

inline Math::Vector3 XM_CALLCONV sqrt(const Math::Vector3& vec)
{
	return _mm_sqrt_ps((XMVECTOR&)vec);
}

inline Math::Vector4 XM_CALLCONV sqrt(const Math::Vector4& vec)
{
	return _mm_sqrt_ps((XMVECTOR&)vec);
}

inline float XM_CALLCONV length(const Math::Vector3& vec1)
{
	Math::Vector3 diff = vec1 * vec1;
	float dotValue = CombineVector3((const XMVECTOR&)diff);
	return sqrt(dotValue);
}

inline float XM_CALLCONV lengthsq(const Math::Vector3& vec1)
{
	Math::Vector3 diff = vec1 * vec1;
	return  CombineVector3((const XMVECTOR&)diff);
}

inline float XM_CALLCONV lengthsq(const Math::Vector4& vec1)
{
	Math::Vector4 diff = vec1 * vec1;
	return  CombineVector4((const XMVECTOR&)diff);
}

inline float XM_CALLCONV distance(const Math::Vector3& vec1, const Math::Vector3& vec2)
{
	Math::Vector3 diff = vec1 - vec2;
	diff *= diff;
	float dotValue = CombineVector3((const XMVECTOR&)diff);
	return sqrt(dotValue);
}

inline float XM_CALLCONV length(const Math::Vector4& vec1)
{
	Math::Vector4 diff = vec1 * vec1;
	float dotValue = CombineVector4((const XMVECTOR&)diff);
	return sqrt(dotValue);
}

inline float XM_CALLCONV distance(const Math::Vector4& vec1, const Math::Vector4& vec2)
{
	Math::Vector4 diff = vec1 - vec2;
	diff *= diff;
	float dotValue = CombineVector4((const XMVECTOR&)diff);
	return sqrt(dotValue);
}

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif


DLL_CLASS Math::Matrix4 XM_CALLCONV mul
(
	const Math::Matrix4& m1,
	const Math::Matrix4& m2);

template <typename T>
constexpr inline T Max(const T& a, const T& b)
{
	return (((a) > (b)) ? (a) : (b));
}
template <typename T>
constexpr inline T Min(const T& a, const T& b)
{
	return (((a) < (b)) ? (a) : (b));
}
template <>
inline Math::Vector3 Max<Math::Vector3>(const Math::Vector3& vec1, const Math::Vector3& vec2)
{
	return _mm_max_ps((XMVECTOR&)vec1, (XMVECTOR&)vec2);
}
template <>
inline Math::Vector4 Max<Math::Vector4>(const Math::Vector4& vec1, const Math::Vector4& vec2)
{
	return _mm_max_ps((XMVECTOR&)vec1, (XMVECTOR&)vec2);
}
template <>
inline Math::Vector3 Min<Math::Vector3>(const Math::Vector3& vec1, const Math::Vector3& vec2)
{
	return _mm_min_ps((XMVECTOR&)vec1, (XMVECTOR&)vec2);
}
template <>
inline Math::Vector4 Min<Math::Vector4>(const Math::Vector4& vec1, const Math::Vector4& vec2)
{
	return _mm_min_ps((XMVECTOR&)vec1, (XMVECTOR&)vec2);
}

inline Math::Vector4 XM_CALLCONV floor(const Math::Vector4& c)
{
	XMVECTOR& V = (XMVECTOR&)c;
#if defined(_XM_NO_INTRINSICS_)
	XMVECTORF32 Result = { { {
			floorf(V.vector4_f32[0]),
			floorf(V.vector4_f32[1]),
			floorf(V.vector4_f32[2]),
			floorf(V.vector4_f32[3])
		} } };
	return Result.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
	return vrndmq_f32(V);
#else
	float32x4_t vTest = vabsq_f32(V);
	vTest = vcltq_f32(vTest, g_XMNoFraction);
	// Truncate
	int32x4_t vInt = vcvtq_s32_f32(V);
	XMVECTOR vResult = vcvtq_f32_s32(vInt);
	XMVECTOR vLarger = vcgtq_f32(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vLarger = vcvtq_f32_s32(vLarger);
	vResult = vaddq_f32(vResult, vLarger);
	// All numbers less than 8388608 will use the round to int
	// All others, use the ORIGINAL value
	return vbslq_f32(vTest, vResult, V);
#endif
#elif defined(_XM_SSE4_INTRINSICS_)
	return _mm_floor_ps(V);
#elif defined(_XM_SSE_INTRINSICS_)
	// To handle NAN, INF and numbers greater than 8388608, use masking
	__m128i vTest = _mm_and_si128(_mm_castps_si128(V), g_XMAbsMask);
	vTest = _mm_cmplt_epi32(vTest, g_XMNoFraction);
	// Truncate
	__m128i vInt = _mm_cvttps_epi32(V);
	XMVECTOR vResult = _mm_cvtepi32_ps(vInt);
	__m128 vLarger = _mm_cmpgt_ps(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vLarger = _mm_cvtepi32_ps(_mm_castps_si128(vLarger));
	vResult = _mm_add_ps(vResult, vLarger);
	// All numbers less than 8388608 will use the round to int
	vResult = _mm_and_ps(vResult, _mm_castsi128_ps(vTest));
	// All others, use the ORIGINAL value
	vTest = _mm_andnot_si128(vTest, _mm_castps_si128(V));
	vResult = _mm_or_ps(vResult, _mm_castsi128_ps(vTest));
	return vResult;
#endif
}
inline Math::Vector3 XM_CALLCONV floor(const Math::Vector3& c)
{
	XMVECTOR& V = (XMVECTOR&)c;
#if defined(_XM_NO_INTRINSICS_)
	XMVECTORF32 Result = { { {
			floorf(V.vector4_f32[0]),
			floorf(V.vector4_f32[1]),
			floorf(V.vector4_f32[2]),
			floorf(V.vector4_f32[3])
		} } };
	return Result.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
	return vrndmq_f32(V);
#else
	float32x4_t vTest = vabsq_f32(V);
	vTest = vcltq_f32(vTest, g_XMNoFraction);
	// Truncate
	int32x4_t vInt = vcvtq_s32_f32(V);
	XMVECTOR vResult = vcvtq_f32_s32(vInt);
	XMVECTOR vLarger = vcgtq_f32(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vLarger = vcvtq_f32_s32(vLarger);
	vResult = vaddq_f32(vResult, vLarger);
	// All numbers less than 8388608 will use the round to int
	// All others, use the ORIGINAL value
	return vbslq_f32(vTest, vResult, V);
#endif
#elif defined(_XM_SSE4_INTRINSICS_)
	return _mm_floor_ps(V);
#elif defined(_XM_SSE_INTRINSICS_)
	// To handle NAN, INF and numbers greater than 8388608, use masking
	__m128i vTest = _mm_and_si128(_mm_castps_si128(V), g_XMAbsMask);
	vTest = _mm_cmplt_epi32(vTest, g_XMNoFraction);
	// Truncate
	__m128i vInt = _mm_cvttps_epi32(V);
	XMVECTOR vResult = _mm_cvtepi32_ps(vInt);
	__m128 vLarger = _mm_cmpgt_ps(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vLarger = _mm_cvtepi32_ps(_mm_castps_si128(vLarger));
	vResult = _mm_add_ps(vResult, vLarger);
	// All numbers less than 8388608 will use the round to int
	vResult = _mm_and_ps(vResult, _mm_castsi128_ps(vTest));
	// All others, use the ORIGINAL value
	vTest = _mm_andnot_si128(vTest, _mm_castps_si128(V));
	vResult = _mm_or_ps(vResult, _mm_castsi128_ps(vTest));
	return vResult;
#endif
}
inline Math::Vector4 XM_CALLCONV ceil
(
	const Math::Vector4& c
)
{
	XMVECTOR& V = (XMVECTOR&)c;
#if defined(_XM_NO_INTRINSICS_)
	XMVECTORF32 Result = { { {
			ceilf(V.vector4_f32[0]),
			ceilf(V.vector4_f32[1]),
			ceilf(V.vector4_f32[2]),
			ceilf(V.vector4_f32[3])
		} } };
	return Result.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
	return vrndpq_f32(V);
#else
	float32x4_t vTest = vabsq_f32(V);
	vTest = vcltq_f32(vTest, g_XMNoFraction);
	// Truncate
	int32x4_t vInt = vcvtq_s32_f32(V);
	XMVECTOR vResult = vcvtq_f32_s32(vInt);
	XMVECTOR vSmaller = vcltq_f32(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vSmaller = vcvtq_f32_s32(vSmaller);
	vResult = vsubq_f32(vResult, vSmaller);
	// All numbers less than 8388608 will use the round to int
	// All others, use the ORIGINAL value
	return vbslq_f32(vTest, vResult, V);
#endif
#elif defined(_XM_SSE4_INTRINSICS_)
	return _mm_ceil_ps(V);
#elif defined(_XM_SSE_INTRINSICS_)
	// To handle NAN, INF and numbers greater than 8388608, use masking
	__m128i vTest = _mm_and_si128(_mm_castps_si128(V), g_XMAbsMask);
	vTest = _mm_cmplt_epi32(vTest, g_XMNoFraction);
	// Truncate
	__m128i vInt = _mm_cvttps_epi32(V);
	XMVECTOR vResult = _mm_cvtepi32_ps(vInt);
	__m128 vSmaller = _mm_cmplt_ps(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vSmaller = _mm_cvtepi32_ps(_mm_castps_si128(vSmaller));
	vResult = _mm_sub_ps(vResult, vSmaller);
	// All numbers less than 8388608 will use the round to int
	vResult = _mm_and_ps(vResult, _mm_castsi128_ps(vTest));
	// All others, use the ORIGINAL value
	vTest = _mm_andnot_si128(vTest, _mm_castps_si128(V));
	vResult = _mm_or_ps(vResult, _mm_castsi128_ps(vTest));
	return vResult;
#endif
}
inline Math::Vector3 XM_CALLCONV ceil
(
	const Math::Vector3& c
)
{
	XMVECTOR& V = (XMVECTOR&)c;
#if defined(_XM_NO_INTRINSICS_)
	XMVECTORF32 Result = { { {
			ceilf(V.vector4_f32[0]),
			ceilf(V.vector4_f32[1]),
			ceilf(V.vector4_f32[2]),
			ceilf(V.vector4_f32[3])
		} } };
	return Result.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
	return vrndpq_f32(V);
#else
	float32x4_t vTest = vabsq_f32(V);
	vTest = vcltq_f32(vTest, g_XMNoFraction);
	// Truncate
	int32x4_t vInt = vcvtq_s32_f32(V);
	XMVECTOR vResult = vcvtq_f32_s32(vInt);
	XMVECTOR vSmaller = vcltq_f32(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vSmaller = vcvtq_f32_s32(vSmaller);
	vResult = vsubq_f32(vResult, vSmaller);
	// All numbers less than 8388608 will use the round to int
	// All others, use the ORIGINAL value
	return vbslq_f32(vTest, vResult, V);
#endif
#elif defined(_XM_SSE4_INTRINSICS_)
	return _mm_ceil_ps(V);
#elif defined(_XM_SSE_INTRINSICS_)
	// To handle NAN, INF and numbers greater than 8388608, use masking
	__m128i vTest = _mm_and_si128(_mm_castps_si128(V), g_XMAbsMask);
	vTest = _mm_cmplt_epi32(vTest, g_XMNoFraction);
	// Truncate
	__m128i vInt = _mm_cvttps_epi32(V);
	XMVECTOR vResult = _mm_cvtepi32_ps(vInt);
	__m128 vSmaller = _mm_cmplt_ps(vResult, V);
	// 0 -> 0, 0xffffffff -> -1.0f
	vSmaller = _mm_cvtepi32_ps(_mm_castps_si128(vSmaller));
	vResult = _mm_sub_ps(vResult, vSmaller);
	// All numbers less than 8388608 will use the round to int
	vResult = _mm_and_ps(vResult, _mm_castsi128_ps(vTest));
	// All others, use the ORIGINAL value
	vTest = _mm_andnot_si128(vTest, _mm_castps_si128(V));
	vResult = _mm_or_ps(vResult, _mm_castsi128_ps(vTest));
	return vResult;
#endif
}


DLL_CLASS Math::Matrix4 XM_CALLCONV transpose(const Math::Matrix4& m);

inline Math::Vector3 XM_CALLCONV pow(const Math::Vector3& v1, const Math::Vector3& v2)
{
	XMVECTOR& V1 = (XMVECTOR&)v1;
	XMVECTOR& V2 = (XMVECTOR&)v2;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTORF32 Result = { { {
			powf(V1.vector4_f32[0], V2.vector4_f32[0]),
			powf(V1.vector4_f32[1], V2.vector4_f32[1]),
			powf(V1.vector4_f32[2], V2.vector4_f32[2]),
			powf(V1.vector4_f32[3], V2.vector4_f32[3])
		} } };
	return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTORF32 vResult = { { {
			powf(vgetq_lane_f32(V1, 0), vgetq_lane_f32(V2, 0)),
			powf(vgetq_lane_f32(V1, 1), vgetq_lane_f32(V2, 1)),
			powf(vgetq_lane_f32(V1, 2), vgetq_lane_f32(V2, 2)),
			powf(vgetq_lane_f32(V1, 3), vgetq_lane_f32(V2, 3))
		} } };
	return vResult.v;
#elif defined(_XM_SSE_INTRINSICS_)
	__declspec(align(16)) float a[4];
	__declspec(align(16)) float b[4];
	_mm_store_ps(a, V1);
	_mm_store_ps(b, V2);
	XMVECTOR vResult = _mm_setr_ps(
		powf(a[0], b[0]),
		powf(a[1], b[1]),
		powf(a[2], b[2]),
		powf(a[3], b[3]));
	return vResult;
#endif
}

inline Math::Vector3 XM_CALLCONV pow(const Math::Vector3& v1, float v2)
{
	XMVECTOR& V1 = (XMVECTOR&)v1;
	XMVECTOR V2 = XMVectorReplicate(v2);
	//XMVECTOR& V2 = (XMVECTOR&)v2;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTORF32 Result = { { {
			powf(V1.vector4_f32[0], V2.vector4_f32[0]),
			powf(V1.vector4_f32[1], V2.vector4_f32[1]),
			powf(V1.vector4_f32[2], V2.vector4_f32[2]),
			powf(V1.vector4_f32[3], V2.vector4_f32[3])
		} } };
	return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTORF32 vResult = { { {
			powf(vgetq_lane_f32(V1, 0), vgetq_lane_f32(V2, 0)),
			powf(vgetq_lane_f32(V1, 1), vgetq_lane_f32(V2, 1)),
			powf(vgetq_lane_f32(V1, 2), vgetq_lane_f32(V2, 2)),
			powf(vgetq_lane_f32(V1, 3), vgetq_lane_f32(V2, 3))
		} } };
	return vResult.v;
#elif defined(_XM_SSE_INTRINSICS_)
	__declspec(align(16)) float a[4];
	__declspec(align(16)) float b[4];
	_mm_store_ps(a, V1);
	_mm_store_ps(b, V2);
	XMVECTOR vResult = _mm_setr_ps(
		powf(a[0], b[0]),
		powf(a[1], b[1]),
		powf(a[2], b[2]),
		powf(a[3], b[3]));
	return vResult;
#endif
}

inline Math::Vector4 XM_CALLCONV pow(const Math::Vector4& v1, const Math::Vector4& v2)
{
	XMVECTOR& V1 = (XMVECTOR&)v1;
	XMVECTOR& V2 = (XMVECTOR&)v2;
#if defined(_XM_NO_INTRINSICS_)

	XMVECTORF32 Result = { { {
			powf(V1.vector4_f32[0], V2.vector4_f32[0]),
			powf(V1.vector4_f32[1], V2.vector4_f32[1]),
			powf(V1.vector4_f32[2], V2.vector4_f32[2]),
			powf(V1.vector4_f32[3], V2.vector4_f32[3])
		} } };
	return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTORF32 vResult = { { {
			powf(vgetq_lane_f32(V1, 0), vgetq_lane_f32(V2, 0)),
			powf(vgetq_lane_f32(V1, 1), vgetq_lane_f32(V2, 1)),
			powf(vgetq_lane_f32(V1, 2), vgetq_lane_f32(V2, 2)),
			powf(vgetq_lane_f32(V1, 3), vgetq_lane_f32(V2, 3))
		} } };
	return vResult.v;
#elif defined(_XM_SSE_INTRINSICS_)
	__declspec(align(16)) float a[4];
	__declspec(align(16)) float b[4];
	_mm_store_ps(a, V1);
	_mm_store_ps(b, V2);
	XMVECTOR vResult = _mm_setr_ps(
		powf(a[0], b[0]),
		powf(a[1], b[1]),
		powf(a[2], b[2]),
		powf(a[3], b[3]));
	return vResult;
#endif
}

inline Math::Vector4 XM_CALLCONV pow(const Math::Vector4& v1, float v2)
{
	XMVECTOR& V1 = (XMVECTOR&)v1;
	XMVECTOR V2 = XMVectorReplicate(v2);
#if defined(_XM_NO_INTRINSICS_)

	XMVECTORF32 Result = { { {
			powf(V1.vector4_f32[0], V2.vector4_f32[0]),
			powf(V1.vector4_f32[1], V2.vector4_f32[1]),
			powf(V1.vector4_f32[2], V2.vector4_f32[2]),
			powf(V1.vector4_f32[3], V2.vector4_f32[3])
		} } };
	return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
	XMVECTORF32 vResult = { { {
			powf(vgetq_lane_f32(V1, 0), vgetq_lane_f32(V2, 0)),
			powf(vgetq_lane_f32(V1, 1), vgetq_lane_f32(V2, 1)),
			powf(vgetq_lane_f32(V1, 2), vgetq_lane_f32(V2, 2)),
			powf(vgetq_lane_f32(V1, 3), vgetq_lane_f32(V2, 3))
		} } };
	return vResult.v;
#elif defined(_XM_SSE_INTRINSICS_)
	__declspec(align(16)) float a[4];
	__declspec(align(16)) float b[4];
	_mm_store_ps(a, V1);
	_mm_store_ps(b, V2);
	XMVECTOR vResult = _mm_setr_ps(
		powf(a[0], b[0]),
		powf(a[1], b[1]),
		powf(a[2], b[2]),
		powf(a[3], b[3]));
	return vResult;
#endif
}
DLL_CLASS Math::Matrix3 XM_CALLCONV transpose(const Math::Matrix3& m);

DLL_CLASS Math::Matrix4 XM_CALLCONV inverse(const Math::Matrix4& m);

DLL_CLASS Math::Matrix4 XM_CALLCONV QuaternionToMatrix(const Math::Vector4& q);

DLL_CLASS Math::Matrix4 XM_CALLCONV GetTransformMatrix(const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& forward, const Math::Vector3& position);
DLL_CLASS Math::Matrix4 XM_CALLCONV GetTransposedTransformMatrix(const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& forward, const Math::Vector3& position);
DLL_CLASS Math::Vector4 XM_CALLCONV cross(const Math::Vector4& v1, const Math::Vector4& v2, const Math::Vector4& v3);
DLL_CLASS Math::Vector3 XM_CALLCONV cross(const Math::Vector3& v1, const Math::Vector3& v2);
DLL_CLASS Math::Vector4 XM_CALLCONV normalize(const Math::Vector4& v);
DLL_CLASS Math::Vector3 XM_CALLCONV normalize(const Math::Vector3& v);
DLL_CLASS Math::Matrix4 XM_CALLCONV GetInverseTransformMatrix(const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& forward, const Math::Vector3& position);
struct double2
{
	double x;
	double y;
	double2(double x, double y) :
		x(x), y(y) {}
	double2() : x(0), y(0) {}
	double2(double x) :
		x(x), y(x) {}
};

struct double3
{
	double x, y, z;
	double3(double x, double y, double z) :
		x(x), y(y), z(z) {}
	double3() :x(0), y(0), z(0)
	{

	}
	double3(double x) : x(x), y(x), z(x) {}
};

struct double4
{
	double x, y, z, w;
	double4(double x, double y, double z, double w) :
		x(x), y(y), z(z), w(w) {}
	double4(double x) :
		x(x), y(x), z(x), w(x) {}
	double4() : x(0), y(0), z(0), w(0)
	{

	}
};

inline float2 operator+(const float2& a, const float2& b)
{
	return float2(a.x + b.x, a.y + b.y);
}
inline float3 operator+(const float3& a, const float3& b)
{
	return float3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline float4 operator+(const float4& a, const float4& b)
{
	return float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
inline double2 operator+(const double2& a, const double2& b)
{
	return double2(a.x + b.x, a.y + b.y);
}
inline double3 operator+(const double3& a, const double3& b)
{
	return double3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline double4 operator+(const double4& a, const double4& b)
{
	return double4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline int2 operator+(const int2& a, const int2& b)
{
	return int2(a.x + b.x, a.y + b.y);
}
inline int3 operator+(const int3& a, const int3& b)
{
	return int3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline int4 operator+(const int4& a, const int4& b)
{
	return int4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline uint2 operator+(const uint2& a, const uint2& b)
{
	return uint2(a.x + b.x, a.y + b.y);
}
inline uint3 operator+(const uint3& a, const uint3& b)
{
	return uint3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline uint4 operator+(const uint4& a, const uint4& b)
{
	return uint4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline float2 operator-(const float2& a, const float2& b)
{
	return float2(a.x - b.x, a.y - b.y);
}
inline float3 operator-(const float3& a, const float3& b)
{
	return float3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline float4 operator-(const float4& a, const float4& b)
{
	return float4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}
inline double2 operator-(const double2& a, const double2& b)
{
	return double2(a.x - b.x, a.y - b.y);
}
inline double3 operator-(const double3& a, const double3& b)
{
	return double3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline double4 operator-(const double4& a, const double4& b)
{
	return double4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline int2 operator-(const int2& a, const int2& b)
{
	return int2(a.x - b.x, a.y - b.y);
}
inline int3 operator-(const int3& a, const int3& b)
{
	return int3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline int4 operator-(const int4& a, const int4& b)
{
	return int4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline uint2 operator-(const uint2& a, const uint2& b)
{
	return uint2(a.x - b.x, a.y - b.y);
}
inline uint3 operator-(const uint3& a, const uint3& b)
{
	return uint3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline uint4 operator-(const uint4& a, const uint4& b)
{
	return uint4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline float2 operator*(const float2& a, const float2& b)
{
	return float2(a.x * b.x, a.y * b.y);
}
inline float3 operator*(const float3& a, const float3& b)
{
	return float3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline float4 operator*(const float4& a, const float4& b)
{
	return float4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}
inline double2 operator*(const double2& a, const double2& b)
{
	return double2(a.x * b.x, a.y * b.y);
}
inline double3 operator*(const double3& a, const double3& b)
{
	return double3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline double4 operator*(const double4& a, const double4& b)
{
	return double4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

inline int2 operator*(const int2& a, const int2& b)
{
	return int2(a.x * b.x, a.y * b.y);
}
inline int3 operator*(const int3& a, const int3& b)
{
	return int3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline int4 operator*(const int4& a, const int4& b)
{
	return int4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

inline uint2 operator*(const uint2& a, const uint2& b)
{
	return uint2(a.x * b.x, a.y * b.y);
}
inline uint3 operator*(const uint3& a, const uint3& b)
{
	return uint3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline uint4 operator*(const uint4& a, const uint4& b)
{
	return uint4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}
inline float2 operator/(const float2& a, const float2& b)
{
	return float2(a.x / b.x, a.y / b.y);
}
inline float3 operator/(const float3& a, const float3& b)
{
	return float3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline float4 operator/(const float4& a, const float4& b)
{
	return float4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}
inline double2 operator/(const double2& a, const double2& b)
{
	return double2(a.x / b.x, a.y / b.y);
}
inline double3 operator/(const double3& a, const double3& b)
{
	return double3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline double4 operator/(const double4& a, const double4& b)
{
	return double4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

inline int2 operator/(const int2& a, const int2& b)
{
	return int2(a.x / b.x, a.y / b.y);
}
inline int3 operator/(const int3& a, const int3& b)
{
	return int3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline int4 operator/(const int4& a, const int4& b)
{
	return int4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

inline uint2 operator/(const uint2& a, const uint2& b)
{
	return uint2(a.x / b.x, a.y / b.y);
}
inline uint3 operator/(const uint3& a, const uint3& b)
{
	return uint3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline uint4 operator/(const uint4& a, const uint4& b)
{
	return uint4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}


inline float2& operator+=(float2& a, const float2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
inline float3& operator+=(float3& a, const float3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}
inline float4& operator+=(float4& a, const float4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

inline double2& operator+=(double2& a, const double2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
inline double3& operator+=(double3& a, const double3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}
inline double4& operator+=(double4& a, const double4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}
inline uint2& operator+=(uint2& a, const uint2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
inline uint3& operator+=(uint3& a, const uint3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}
inline uint4& operator+=(uint4& a, const uint4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

inline int2& operator+=(int2& a, const int2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
inline int3& operator+=(int3& a, const int3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}
inline int4& operator+=(int4& a, const int4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}


inline float2& operator-=(float2& a, const float2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
inline float3& operator-=(float3& a, const float3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
inline float4& operator-=(float4& a, const float4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

inline double2& operator-=(double2& a, const double2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
inline double3& operator-=(double3& a, const double3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
inline double4& operator-=(double4& a, const double4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}
inline uint2& operator-=(uint2& a, const uint2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
inline uint3& operator-=(uint3& a, const uint3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
inline uint4& operator-=(uint4& a, const uint4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

inline int2& operator-=(int2& a, const int2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
inline int3& operator-=(int3& a, const int3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
inline int4& operator-=(int4& a, const int4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}


inline float2& operator*=(float2& a, const float2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}
inline float3& operator*=(float3& a, const float3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}
inline float4& operator*=(float4& a, const float4& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}

inline double2& operator*=(double2& a, const double2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}
inline double3& operator*=(double3& a, const double3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}
inline double4& operator*=(double4& a, const double4& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}
inline uint2& operator*=(uint2& a, const uint2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}
inline uint3& operator*=(uint3& a, const uint3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}
inline uint4& operator*=(uint4& a, const uint4& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}

inline int2& operator*=(int2& a, const int2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}
inline int3& operator*=(int3& a, const int3& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}
inline int4& operator*=(int4& a, const int4& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}
inline float2& operator/=(float2& a, const float2& b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}
inline float3& operator/=(float3& a, const float3& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}
inline float4& operator/=(float4& a, const float4& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}

inline double2& operator/=(double2& a, const double2& b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}
inline double3& operator/=(double3& a, const double3& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}
inline double4& operator/=(double4& a, const double4& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}
inline uint2& operator/=(uint2& a, const uint2& b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}
inline uint3& operator/=(uint3& a, const uint3& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}
inline uint4& operator/=(uint4& a, const uint4& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}

inline int2& operator/=(int2& a, const int2& b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}
inline int3& operator/=(int3& a, const int3& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	return a;
}
inline int4& operator/=(int4& a, const int4& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}
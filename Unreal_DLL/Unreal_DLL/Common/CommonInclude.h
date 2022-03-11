#pragma once
#define NOMINMAX
#include <DirectXMath.h>
#include <memory>
//#include <algorithm>
#include "HashMap.h"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include "vstring.h"
#include "DLL.h"
#include "Runnable.h"
#include "TypeWiper.h"
#include "HashMultiMap.h"
#include "BitVector.h"
#include "BitArray.h"
typedef DirectX::XMFLOAT2 float2;
typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;
typedef DirectX::XMUINT2 uint2;
typedef DirectX::XMUINT3 uint3;
typedef DirectX::XMUINT4 uint4;
typedef DirectX::XMINT2 int2;
typedef DirectX::XMINT3 int3;
typedef DirectX::XMINT4 int4;
typedef uint32_t uint;
typedef DirectX::XMFLOAT4X4 float4x4;
typedef DirectX::XMFLOAT3X3 float3x3;
typedef DirectX::XMFLOAT3X4 float3x4;
typedef DirectX::XMFLOAT4X3 float4x3;
using uint64 = uint64_t;
using int64 = int64_t;
using int32 = int32_t;
#include "DXMath/DXMath.h"
namespace vengine
{
	template <>
	struct hash<uint2>
	{
		inline uint64_t operator()(uint2 const& value) const noexcept
		{
			uint32_t const* ptr = (uint32_t const*)&value;
			return Hash::Int32ArrayHash(ptr, ptr + 2);
		}
	};
	template <>
	struct hash<uint3>
	{
		inline uint64_t operator()(uint3 const& value) const noexcept
		{
			uint32_t const* ptr = (uint32_t const*)&value;
			return Hash::Int32ArrayHash(ptr, ptr + 3);
		}
	};
	template <>
	struct hash<uint4>
	{
		inline uint64_t operator()(uint4 const& value) const noexcept
		{
			uint32_t const* ptr = (uint32_t const*)&value;
			return Hash::Int32ArrayHash(ptr, ptr + 4);
		}
	};
	template <>
	struct hash<int2>
	{
		inline uint64_t operator()(int2  const& value) const noexcept
		{
			uint32_t const* ptr = (uint32_t const*)&value;
			return Hash::Int32ArrayHash(ptr, ptr + 2);
		}
	};
	template <>
	struct hash<int3>
	{
		inline uint64_t operator()(int3 const& value) const noexcept
		{
			uint32_t const* ptr = (uint32_t const*)&value;
			return Hash::Int32ArrayHash(ptr, ptr + 3);
		}
	};
	template <>
	struct hash<int4>
	{
		inline uint64_t operator()(int4 const& value) const noexcept
		{
			uint32_t const* ptr = (uint32_t const*)&value;
			return Hash::Int32ArrayHash(ptr, ptr + 4);
		}
	};
}
namespace std
{
	template <>
	struct equal_to<uint2>
	{
		inline bool operator()(uint2 const& a, uint2 const& b) const noexcept
		{
			return a.x == b.x && a.y == b.y;
		}
	};
	template <>
	struct equal_to<uint3>
	{
		inline bool operator()(uint3 const& a, uint3 const& b) const noexcept
		{
			return a.x == b.x && a.y == b.y && a.z == b.z;
		}
	};
	template <>
	struct equal_to<uint4>
	{
		inline bool operator()(uint4 const& a, uint4 const& b) const noexcept
		{
			return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
		}
	};
	template <>
	struct equal_to<int2>
	{
		inline bool operator()(int2 const& a, int2 const& b) const noexcept
		{
			return a.x == b.x && a.y == b.y;
		}
	};
	template <>
	struct equal_to<int3>
	{
		inline bool operator()(int3 const& a, int3 const& b) const noexcept
		{
			return a.x == b.x && a.y == b.y && a.z == b.z;
		}
	};
	template <>
	struct equal_to<int4>
	{
		inline bool operator()(int4 const& a, int4 const& b) const noexcept
		{
			return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
		}
	};
}
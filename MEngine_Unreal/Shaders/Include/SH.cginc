#ifndef VRP_SH_CGINC
	#define VRP_SH_CGINC
	#include "Montcalo_Library.hlsl"
	#ifndef Pi
		#define Pi 3.1415926535897932
	#endif
	uint2 GetUShortFromUInt(uint value)
	{
		uint2 result;
		result.y = value >> 16;
		result.x = value & 65535;
		return result;
	}
	struct SH9
	{
		float c[9];
	};

	struct SH9Color
	{
		float3 c[9];
	};

	SH9 SHCosineLobe(float3 normal)
	{
		//	normal = normalize(normal);
		float x = normal.x; float y = normal.y; float z = normal.z;
		float x2 = x * x; float y2 = y * y; float z2 = z * z;
		SH9 sh;
		sh.c[0] = 0.28209478;							//1/2*sqrt(1/Pi)
		sh.c[1] = 0.48860251 * y;						//sqrt(3/(4Pi))
		sh.c[2] = 0.48860251 * z;
		sh.c[3] = 0.48860251 * x;
		sh.c[4] = 1.09254843 * x * y;					//1/2*sqrt(15/Pi)
		sh.c[5] = 1.09254843 * y * z;					
		sh.c[6] = 0.31539156 * (-x2 - y2 + 2 * z2);		//1/4*sqrt(5/Pi)
		sh.c[7] = 1.09254843 * z * x;
		sh.c[8] = 0.54627422 * (x2 - y2);				//1/4*sqrt(15/Pi)
		return sh;
	}

	SH9Color GetSHColor(SH9 sh, float3 color)
	{
		SH9Color c;
		#define FUNC(i) c.c[i] = sh.c[i] * color;
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return c;
	}

	SH9Color GetSHColor(SH9 sh, SH9Color color)
	{
		SH9Color c;
		#define FUNC(i) c.c[i] = sh.c[i] * color.c[i];
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return c;
	}

	SH9 SHSet(float value)
	{
		SH9 sh;
		#define FUNC(i) sh.c[i] = value;
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return sh;
	}

	SH9Color SHColorSet(float3 value)
	{
		SH9Color sh;
		#define FUNC(i) sh.c[i] = value;
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return sh;
	}

	SH9 SHLerp(SH9 first, SH9 second, float v)
	{
		#define FUNC(i) first.c[i] = lerp(first.c[i], second.c[i], v);
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return first;
	}
	SH9Color SHColorLerp(SH9Color first, SH9Color second, float3 v)
	{
		#define FUNC(i) first.c[i] = lerp(first.c[i], second.c[i], v);
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return first;
	}

	SH9 SHMul(SH9 sh, float value)
	{
		#define FUNC(i) sh.c[i] *= value;
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		
		return sh;
	}

	SH9 SHMul(SH9 sh, SH9 value)
	{
		#define FUNC(i) sh.c[i] *= value.c[i];
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return sh;
	}

	SH9Color SHColorMul(SH9Color sh, float3 value)
	{
		#define FUNC(i) sh.c[i] *= value;
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return sh;
	}

	SH9Color SHColorMul(SH9Color sh, SH9Color value)
	{
		#define FUNC(i) sh.c[i] *= value.c[i];
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return sh;
	}

	SH9 SHDivide(SH9 sh, float value)
	{
		#define FUNC(i) sh.c[i] /= value;
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return sh;
	}

	SH9Color SHColorDivide(SH9Color sh, float value)
	{
		#define FUNC(i) sh.c[i] /= value;
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC

		return sh;
	}

	SH9 SHAdd(SH9 a, SH9 b)
	{
		#define FUNC(i) a.c[i] += b.c[i];
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return a;
	}

	SH9Color SHColorAdd(SH9Color a, SH9Color b)
	{
		#define FUNC(i) a.c[i] += b.c[i];
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return a;
	}

	SH9 SHSub(SH9 a, SH9 b)
	{
		#define FUNC(i) a.c[i] -= b.c[i];
		FUNC(0)		
		FUNC(1)		
		FUNC(2)		
		FUNC(3)		
		FUNC(4)		
		FUNC(5)		
		FUNC(6)		
		FUNC(7)		
		FUNC(8)		
		#undef FUNC
		return a;
	}

	SH9 GetSHSample(float3 dir, uint sampleCount, TextureCube<float> sampleTex, SamplerState sampleTexSampler)
	{
		uint3 p1 = ReverseBits32(asuint(dir.xy));
		uint2 p2 = (abs(dir.z) > abs(dir.y)) ? p1.zx : p1.zy;
		p2 = (p2 << 16) | (p2 >> 16);
		p2 = ((p2 & 0x33333333) << 2) | ((p2 & 0xcccccccc) >> 2);
		p2 = ((p2 & 0x0f0f0f0f) << 4) | ((p2 & 0xf0f0f0f0) >> 4);
		uint2 p = p1.xy + p2;

		float3 tangent = 0; 
		if (abs(dir.y) > 0.7) {
			tangent = cross(float3(0, 0, 1), dir);
		}
		else {
			tangent = cross(float3(0, 1, 0), dir);
		}
		tangent = normalize(tangent);
		float3 bi = normalize(cross(tangent, dir));

		float3x3 TangentToWorld = float3x3(tangent, bi, dir);
		SH9 result = SHSet(0);
		
		float color = sampleTex.SampleLevel(sampleTexSampler, dir, 0);
		[loop]
		for (int i = 0; i < sampleCount; i++)
		{
			float2 random = Hammersley(i, sampleCount, p);
			float3 v = normalize(mul(UniformSampleHemisphere(random).xyz, TangentToWorld));
			SH9 shValue = SHCosineLobe(v);
			result = SHAdd(result, SHMul(shValue, color));
		}
		result = SHDivide(result, sampleCount);
		return result;
	}
	SH9 GetSHSample(float3 dir, uint sampleCount, SH9 color)
	{
		uint3 p1 = ReverseBits32(asuint(dir.xy));
		uint2 p2 = (abs(dir.z) > abs(dir.y)) ? p1.zx : p1.zy;
		p2 = (p2 << 16) | (p2 >> 16);
		p2 = ((p2 & 0x33333333) << 2) | ((p2 & 0xcccccccc) >> 2);
		p2 = ((p2 & 0x0f0f0f0f) << 4) | ((p2 & 0xf0f0f0f0) >> 4);
		uint2 p = p1.xy + p2;

		float3 tangent = 0; 
		if (abs(dir.y) > 0.7) {
			tangent = cross(float3(0, 0, 1), dir);
		}
		else {
			tangent = cross(float3(0, 1, 0), dir);
		}
		tangent = normalize(tangent);
		float3 bi = normalize(cross(tangent, dir));

		float3x3 TangentToWorld = float3x3(tangent, bi, dir);
		SH9 result = SHSet(0);
		
		[loop]
		for (int i = 0; i < sampleCount; i++)
		{
			float2 random = Hammersley(i, sampleCount, p);
			float3 v = normalize(mul(UniformSampleHemisphere(random).xyz, TangentToWorld));
			SH9 shValue = SHCosineLobe(v);
			result = SHAdd(result, SHMul(shValue, color));
		}
		result = SHDivide(result, sampleCount);
		return result;
	}
	SH9Color GetSHSampleColor(float3 dir, uint sampleCount, float3 color)
	{
		uint3 p1 = ReverseBits32(asuint(dir.xy));
		uint2 p2 = (abs(dir.z) > abs(dir.y)) ? p1.zx : p1.zy;
		p2 = (p2 << 16) | (p2 >> 16);
		p2 = ((p2 & 0x33333333) << 2) | ((p2 & 0xcccccccc) >> 2);
		p2 = ((p2 & 0x0f0f0f0f) << 4) | ((p2 & 0xf0f0f0f0) >> 4);
		uint2 p = p1.xy + p2;

		float3 tangent = 0; 
		if (abs(dir.y) > 0.7) {
			tangent = cross(float3(0, 0, 1), dir);
		}
		else {
			tangent = cross(float3(0, 1, 0), dir);
		}
		tangent = normalize(tangent);
		float3 bi = normalize(cross(tangent, dir));

		float3x3 TangentToWorld = float3x3(tangent, bi, dir);
		SH9Color result = SHColorSet(0);
		[loop]
		for (int i = 0; i < sampleCount; i++)
		{
			float2 random = Hammersley(i, sampleCount, p);
			float3 v = normalize(mul(UniformSampleHemisphere(random).xyz, TangentToWorld));
			SH9 shValue = SHCosineLobe(v);
			result = SHColorAdd(result, GetSHColor(shValue, color));
		}
		result = SHColorDivide(result, sampleCount);
		return result;
	}
	SH9Color GetSHColorFromTexture(
	Texture3D<float4> texs0,
	Texture3D<float4> texs1,
	Texture3D<float4> texs2,
	Texture3D<float4> texs3,
	Texture3D<float4> texs4,
	Texture3D<float4> texs5,
	Texture3D<float4> texs6,
	SamplerState samp, float3 probeUV)
	{
		SH9Color shColor;
		float4 giColor;
		giColor = texs0.SampleLevel(samp, probeUV, 0);
		shColor.c[0] = giColor.xyz;
		shColor.c[1].x = giColor.w;
		giColor = texs1.SampleLevel(samp, probeUV, 0);
		shColor.c[1].yz = giColor.xy;
		shColor.c[2].xy = giColor.zw;
		giColor = texs2.SampleLevel(samp, probeUV, 0);
		shColor.c[2].z = giColor.x;
		shColor.c[3].xyz = giColor.yzw;
		
		giColor = texs3.SampleLevel(samp, probeUV, 0);
		shColor.c[4] = giColor.xyz;
		shColor.c[5].x = giColor.w;
		giColor = texs4.SampleLevel(samp, probeUV, 0);
		shColor.c[5].yz = giColor.xy;
		shColor.c[6].xy = giColor.zw;
		giColor = texs5.SampleLevel(samp, probeUV, 0);
		shColor.c[6].z = giColor.x;
		shColor.c[7].xyz = giColor.yzw;
		shColor.c[8] = texs6.SampleLevel(samp, probeUV, 0).xyz;
		return shColor;
	}
	void SetSHColorToTexture(
	RWTexture3D<float4> texs0,
	RWTexture3D<float4> texs1,
	RWTexture3D<float4> texs2,
	RWTexture3D<float4> texs3,
	RWTexture3D<float4> texs4,
	RWTexture3D<float4> texs5,
	RWTexture3D<float4> texs6,
	uint3 id, SH9Color sh)
	{
		texs0[id] = float4(sh.c[0].xyz, sh.c[1].x);
		texs1[id] = float4(sh.c[1].yz, sh.c[2].xy);
		texs2[id] = float4(sh.c[2].z, sh.c[3].xyz);
		texs3[id] = float4(sh.c[4].xyz, sh.c[5].x);
		texs4[id] = float4(sh.c[5].yz, sh.c[6].xy);
		texs5[id] = float4(sh.c[6].z, sh.c[7].xyz);
		texs6[id] = float4(sh.c[8].xyz, 1);
	}
	float Stencil_BilinearWeight(float originLerp, float leftWeight, float rightWeight)
	{
		return saturate(rightWeight - (1 - originLerp) * leftWeight);
	}
	float2 Stencil_BilinearWeight(float originLerp, float2 leftWeight, float2 rightWeight)
	{
		return saturate(rightWeight - (1 - originLerp) * leftWeight);
	}
	float4 Stencil_BilinearWeight(float originLerp, float4 leftWeight, float4 rightWeight)
	{
		return saturate(rightWeight - (1 - originLerp) * leftWeight);
	}
#endif
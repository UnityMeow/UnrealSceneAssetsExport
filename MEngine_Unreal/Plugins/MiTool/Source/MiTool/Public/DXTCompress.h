// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VEngine_Include.h"

class DXTCompress
{
	using PCalcFun = void(*)(float4*,byte*);
	using PCalcFun_1 = void(*)(byte*,float4*,uint);
	using PCalcFun_2 = void(*)(byte*,float4*,float,uint);
	static std::atomic<DXTCompress*> instance;
	static std::atomic<uint> isWorkingOnLoading;
	//static std::mutex mtx;
	DXTCompress();
	~DXTCompress();
	DXTCompress(const DXTCompress& that);

	void Load();

	void* Handle;
	PCalcFun _D3DXDecodeBC1;
	PCalcFun _D3DXDecodeBC2;
	PCalcFun _D3DXDecodeBC3;
	PCalcFun _D3DXDecodeBC4U;
	PCalcFun _D3DXDecodeBC4S;
	PCalcFun _D3DXDecodeBC5U;
	PCalcFun _D3DXDecodeBC5S;
	PCalcFun _D3DXDecodeBC6HU;
	PCalcFun _D3DXDecodeBC6HS;
	PCalcFun _D3DXDecodeBC7;
	PCalcFun_2 _D3DXEncodeBC1;
	PCalcFun_1 _D3DXEncodeBC2;
	PCalcFun_1 _D3DXEncodeBC3;
	PCalcFun_1 _D3DXEncodeBC4U;
	PCalcFun_1 _D3DXEncodeBC4S;
	PCalcFun_1 _D3DXEncodeBC5U;
	PCalcFun_1 _D3DXEncodeBC5S;
	PCalcFun_1 _D3DXEncodeBC6HU;
	PCalcFun_1 _D3DXEncodeBC6HS;
	PCalcFun_1 _D3DXEncodeBC7;
public:
	static DXTCompress* GetInstance();
	void D3DXDecodeBC1(float4* pColor, byte* pBC);
	void D3DXDecodeBC2(float4* pColor, byte* pBC);
	void D3DXDecodeBC3(float4* pColor, byte* pBC);
	void D3DXDecodeBC4U(float4* pColor, byte* pBC);
	void D3DXDecodeBC4S(float4* pColor, byte* pBC);
	void D3DXDecodeBC5U(float4* pColor, byte* pBC);
	void D3DXDecodeBC5S(float4* pColor, byte* pBC);
	void D3DXDecodeBC6HU(float4* pColor, byte* pBC);
	void D3DXDecodeBC6HS(float4* pColor, byte* pBC);
	void D3DXDecodeBC7(float4* pColor, byte* pBC);
	
	void D3DXEncodeBC1(byte* pBC, float4* pColor, float threshold, uint flags);
	
	void D3DXEncodeBC2(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC3(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC4U(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC4S(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC5U(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC5S(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC6HU(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC6HS(byte* pBC, float4* pColor, uint flags);
	void D3DXEncodeBC7(byte* pBC, float4* pColor, uint flags);
};

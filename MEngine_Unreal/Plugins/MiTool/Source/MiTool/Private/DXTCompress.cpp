// Fill out your copyright notice in the Description page of Project Settings.


#include "MiTool/Public/DXTCompress.h"

#include "Interfaces/IPluginManager.h"
std::atomic<DXTCompress*> DXTCompress::instance = nullptr;
//std::mutex DXTCompress::mtx;
std::atomic<uint> DXTCompress::isWorkingOnLoading = 0;

DXTCompress::DXTCompress()
{
}

DXTCompress::~DXTCompress()
{
    isWorkingOnLoading = 0;
    instance = nullptr;
}

DXTCompress::DXTCompress(const DXTCompress& that)
{
}

DXTCompress* DXTCompress::GetInstance()
{
    if (instance == nullptr)
    {
       // std::lock_guard<std::mutex> lck(mtx);
        //if (atomicInstance == nullptr)
        if(!(isWorkingOnLoading++))
        {
            //atomicInstance= new DXTCompress;
            //atomicInstance->Load();
            auto tmp = new DXTCompress;
            tmp->Load();
            instance = tmp;
        }
        else
        {
            while(!instance)
            {}
        }
    }
    return instance;
}

void DXTCompress::Load()
{
    const FString InPath = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("VEngineDLL"))->GetBaseDir(),
                                           TEXT("Source/VEngineDLL/ThirdParty/Dll1.dll"));
    if (!FPaths::FileExists(InPath))
    {
        UE_LOG(LogTemp, Display, TEXT("Load Dll failed, Reason: Dll in Path \"%s\" is not exist!!!"), *InPath);
        return;
    }
    Handle = FPlatformProcess::GetDllHandle(*InPath);
    if (Handle == nullptr)
    {
        UE_LOG(LogTemp, Display, TEXT("Load Dll failed, Reason: Unknown error, Path: %s"), *InPath);
        return;
    }

    _D3DXDecodeBC1 = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC1"));
    _D3DXDecodeBC2 = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC2"));
    _D3DXDecodeBC3 = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC3"));
    _D3DXDecodeBC4U = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC4U"));
    _D3DXDecodeBC4S = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC4S"));
    _D3DXDecodeBC5U = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC5U"));
    _D3DXDecodeBC5S = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC5S"));
    _D3DXDecodeBC6HU = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC6HU"));
    _D3DXDecodeBC6HS = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC6HS"));
    _D3DXDecodeBC7 = (PCalcFun)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXDecodeBC7"));
    _D3DXEncodeBC1 = (PCalcFun_2)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC1"));
    _D3DXEncodeBC2 = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC2"));
    _D3DXEncodeBC3 = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC3"));
    _D3DXEncodeBC4U = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC4U"));
    _D3DXEncodeBC4S = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC4S"));
    _D3DXEncodeBC5U = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC5U"));
    _D3DXEncodeBC5S = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC5S"));
    _D3DXEncodeBC6HU = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC6HU"));
    _D3DXEncodeBC6HS = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC6HS"));
    _D3DXEncodeBC7 = (PCalcFun_1)FPlatformProcess::GetDllExport(Handle,TEXT("D3DXEncodeBC7"));

    if (_D3DXEncodeBC5U == nullptr)
    {
        UE_LOG(LogTemp, Display, L"null");
    }
}

void DXTCompress::D3DXDecodeBC1(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC1)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC2(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC2)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC3(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC3)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC4U(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC4U)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC4S(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC4S)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC5U(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC5U)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC5S(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC5S)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC6HU(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC6HU)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC6HS(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC6HS)(pColor, pBC);
}

void DXTCompress::D3DXDecodeBC7(float4* pColor, byte* pBC)
{
    (*_D3DXDecodeBC7)(pColor, pBC);
}

void DXTCompress::D3DXEncodeBC2(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC2)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC3(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC3)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC4U(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC4U)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC4S(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC4S)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC5U(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC5U)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC5S(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC5S)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC6HU(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC6HU)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC6HS(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC6HS)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC7(byte* pBC, float4* pColor, uint flags)
{
    (*_D3DXEncodeBC7)(pBC, pColor, flags);
}

void DXTCompress::D3DXEncodeBC1(byte* pBC, float4* pColor, float threshold, uint flags)
{
    (*_D3DXEncodeBC1)(pBC, pColor, threshold, flags);
}

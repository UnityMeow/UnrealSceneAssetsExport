// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <openexr/Deploy/OpenEXR-2.3.0/OpenEXR/include/ImathMath.h>

#include "VEngine_Include.h"
#include "SceneExport/MiTextureFileExport.h"
#include "MiDLL.h"
#include "Interfaces/IPluginManager.h"

enum LoadFormat
{
	LoadFormat_R8G8B8A8_UNorm = 0,
    LoadFormat_R16G16B16A16_UNorm = 1,
    LoadFormat_R16G16B16A16_SFloat = 2,
    LoadFormat_R32G32B32A32_SFloat = 3,
    LoadFormat_R16G16_SFloat = 4,
    LoadFormat_R16G16_UNorm = 5,
    LoadFormat_BC7 = 6,
    LoadFormat_BC6H = 7,
    LoadFormat_R32_UINT = 8,
    LoadFormat_R32G32_UINT = 9,
    LoadFormat_R32G32B32A32_UINT = 10,
    LoadFormat_R16_UNorm = 11,
    LoadFormat_BC5U = 12,
    LoadFormat_BC5S = 13,
    LoadFormat_R16_UINT = 14,
    LoadFormat_R16G16_UINT = 15,
    LoadFormat_R16G16B16A16_UINT = 16,
    LoadFormat_R8_UINT = 17,
    LoadFormat_R8G8_UINT = 18,
    LoadFormat_R8G8B8A8_UINT = 19,
    LoadFormat_R32_SFloat = 20,
    LoadFormat_BC4U = 21,
    LoadFormat_BC4S = 22,
};

enum LoadFormat3D
{
	LoadFormat3D_R8G8B8A8_UNorm = LoadFormat::LoadFormat_R8G8B8A8_UNorm,
    LoadFormat3D_R16G16B16A16_UNorm = LoadFormat::LoadFormat_R16G16B16A16_UNorm,
    LoadFormat3D_R16G16B16A16_SFloat = LoadFormat::LoadFormat_R16G16B16A16_SFloat,
    LoadFormat3D_R32G32B32A32_SFloat = LoadFormat::LoadFormat_R32G32B32A32_SFloat,
    LoadFormat3D_R16G16_SFloat = LoadFormat::LoadFormat_R16G16_SFloat,
    LoadFormat3D_R16G16_UNorm = LoadFormat::LoadFormat_R16G16_UNorm,
    LoadFormat3D_R16_UNorm = LoadFormat::LoadFormat_R16_UNorm,
    LoadFormat3D_R32_SFloat = LoadFormat::LoadFormat_R32_SFloat
};

enum TextureDimension
{
	Tex2D = 0,
	Tex3D = 1,
	Cube = 2,
};

struct TextureData
{
	uint Width;
	uint Height;
	// 纹理深度
	uint Depth;
	// 纹理类型
	TextureDimension Dimension;
	// 输出层数
	uint MipCount;
	// 纹理加载格式
	LoadFormat Format;
};

enum TextureExportType
{
	RGB = 0,
    RGBA = 1,
    Normal = 2,
    SingleChannel = 3,
};

struct TextureExportSetting
{
	// 要输出的纹理贴图
	UTexture* Texture;
	// 输出路径
	vengine::string Path;
	// 压缩是否包涵透明通道
	bool UseAlphaInCompress;
	// 是否开启MipMap选项
	bool UseMipMap = true;
	// 是否将法线转为正的
	bool IsNormal = false;
	// 纹理加载方式
	// LoadFormat Format = LoadFormat_R16G16B16A16_SFloat;
	// 导出类型
	TextureExportType ExportType;
	TextureExportSetting()
	{
		
	}
	~TextureExportSetting()
	{
		
	}
};

inline uint GetIndex(uint2 uv, uint width)
{
	return uv.y * width + uv.x;
}

struct SRGBJob
{
	vengine::vector<float4>* data;
	void Execute(uint kernel)
	{
		float4 a = data->data()[kernel];
		data->data()[kernel] = pow(a,2.2f);
	}
};

struct CompressJob
{
	LoadFormat compressType;
	uint2* dest;
	float4* source;
	int64 width;
	vengine::string path;
	CompressJob()
	{
		FString InPath = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("VEngineDLL"))->GetBaseDir(),
                                   TEXT("Source/VEngineDLL/ThirdParty/Dll1.dll"));
		wchar_t* wstr = (wchar_t*)GetData(InPath);
		path = StringUtil::WcharToVString(wstr);
	}
	void Execute(uint kernel)
	{
		float4 b[4 * 4] = {};
		int y = kernel / (width / 4);
		int x = kernel % (width / 4);
		for (uint i = 0, yy = 0; yy < 4; ++yy)
		{
			for (uint xx = 0; xx < 4; ++xx)
			{
				b[i] = source[GetIndex(uint2((uint)(x * 4 + xx), (uint)(y * 4 + yy)), (uint)width)];
				i++;
			}
		}
		// auto a = DXTCompress::GetInstance();
		// a->D3DXEncodeBC5U((byte*)(dest + kernel * 2), b, 0);
		
		auto D3DXEncodeBC5U = MiDLL::GetInstance()->Use<void(byte*,float4*,uint)>(path,"D3DXEncodeBC5U");
		auto D3DXEncodeBC5S = MiDLL::GetInstance()->Use<void(byte*,float4*,uint)>(path,"D3DXEncodeBC5S");
		auto D3DXEncodeBC4U = MiDLL::GetInstance()->Use<void(byte*,float4*,uint)>(path,"D3DXEncodeBC4U");
		auto D3DXEncodeBC6HU = MiDLL::GetInstance()->Use<void(byte*,float4*,uint)>(path,"D3DXEncodeBC6HU");
		auto D3DXEncodeBC4S = MiDLL::GetInstance()->Use<void(byte*,float4*,uint)>(path,"D3DXEncodeBC4S");
		switch (compressType)
		{
		case LoadFormat::LoadFormat_BC5U:
			D3DXEncodeBC5U((byte*)(dest + kernel * 2), b, 0);
			break;
		case LoadFormat::LoadFormat_BC5S:
            D3DXEncodeBC5S((byte*)(dest + kernel * 2), b, 0);
			break;
		case LoadFormat::LoadFormat_BC6H:
            D3DXEncodeBC6HU((byte*)(dest + kernel * 2), b, 0);
			break;
		case LoadFormat::LoadFormat_BC4U:
            D3DXEncodeBC4U((byte*)(dest + kernel), b, 0);
			break;
		case LoadFormat::LoadFormat_BC4S:
            D3DXEncodeBC4S((byte*)(dest + kernel), b, 0);
			break;
		}
		
	}
};

class TextureExport
{
	MiTextureFileExport fileExport;
	uint2 size;
	uint pixelSize;
	TextureExportSetting setting;
	void Export2D();
	void Export3D();
	void ExportCube();
	uint GetMipCount(LoadFormat type);
	void GetTexture2DPixel(vengine::vector<float4>& data,UTexture2D* Texture);
	void ExportFile(UTexture2D* Texture,TextureFileExportDataType dataType,uint passCount,uint mipmapcount);
	void ExportFileJob(UTexture2D* Texture,LoadFormat dataType,uint passCount, uint tmp,uint mipmapcount);
public:
	TextureExport(TextureExportSetting&& Setting);
	~TextureExport();
};
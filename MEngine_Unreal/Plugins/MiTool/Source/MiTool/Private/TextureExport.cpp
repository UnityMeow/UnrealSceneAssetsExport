// Fill out your copyright notice in the Description page of Project Settings.

#include "MiTool/Public/TextureExport.h"
#include "Engine/TextureCube.h"
#include "Engine/VolumeTexture.h"
#include "Engine/Texture2D.h"
#include "SceneExport/MiTextureFileExport.h"
#include "IRendererBase.h"
#include "MiTool/Public/MiTextureCompress.h"

TextureExport::TextureExport(TextureExportSetting&& Setting)
{
    setting = Setting;
    if (setting.Texture->IsA<UTexture2D>())
    {
        Export2D();
    }
    else if (setting.Texture->IsA<UTextureCube>())
    {
        ExportCube();
    }
    else if (setting.Texture->IsA<UVolumeTexture>())
    {
        Export3D();
    }
}

TextureExport::~TextureExport()
{
}

void TextureExport::Export2D()
{
    UTexture2D* Texture = Cast<UTexture2D>(setting.Texture);
    TextureData data;
    // ---- Format ----
    data.Format = LoadFormat::LoadFormat_R16G16B16A16_SFloat;
    switch (setting.ExportType)
    {
    case TextureExportType::RGB:
    case TextureExportType::RGBA:
        data.Format = LoadFormat::LoadFormat_BC7;
        break;
    case TextureExportType::Normal:
        data.Format = LoadFormat::LoadFormat_BC5U;
        break;
    case TextureExportType::SingleChannel:
        data.Format = LoadFormat::LoadFormat_BC4U;
        break;
    }
    // ---- w h size ----
    data.Width = Texture->GetSizeX();
    data.Height = Texture->GetSizeY();
    size = uint2(data.Width, data.Height);
    pixelSize = size.x * size.y;
    // ---- other ----
    data.MipCount = GetMipCount(data.Format);
    data.Depth = 1;
    data.Dimension = TextureDimension::Tex2D;
    if (data.Format != LoadFormat::LoadFormat_BC6H && data.Format != LoadFormat::LoadFormat_BC7)
    {
        fileExport.DataInit((uint64)sizeof(TextureData), (uint64)(pixelSize * 16), (byte*)&data);
    }
    vengine::vector<float4> teData;
    switch (data.Format)
    {
        // ------------------------------------------------
        {
        case LoadFormat::LoadFormat_BC6H:
            {
                GetTexture2DPixel(teData, Texture);
                MiTextureCompress::GetInstance()->RendererBase->AddTextureCompressTask(
                    TexFormat::BC6H, setting.UseAlphaInCompress ? 1 : 0, data.Width,
                    data.Height, data.MipCount, setting.Path.c_str(), teData.data());
            }
            break;
            // isHDR = true
        case LoadFormat::LoadFormat_BC7:
            {
                GetTexture2DPixel(teData, Texture);
                MiTextureCompress::GetInstance()->RendererBase->AddTextureCompressTask(
                    TexFormat::BC7, 0, data.Width, data.Height, data.MipCount,
                    setting.Path.c_str(), teData.data());
            }
            break;
            // isHDR = false
            // bit32 passCount = 4
            // dataType = TextureFileExportDataType::Bit32;
            // passCount = 4;
        }
        // ------------------------------------------------
        {
        case LoadFormat::LoadFormat_R32_UINT:
        case LoadFormat::LoadFormat_R32G32_UINT:
        case LoadFormat::LoadFormat_R32G32B32A32_UINT:
            {
                // dataType = TextureFileExportDataType::Bit32;
                // passCount = 2;
            }
            // pass = 2
            // bit32
            // arraySize = size
        case LoadFormat::LoadFormat_R16_UINT:
        case LoadFormat::LoadFormat_R16G16_UINT:
        case LoadFormat::LoadFormat_R16G16B16A16_UINT:
            {
                // dataType = TextureFileExportDataType::Bit16;
                // passCount = 3;
            }
            // pass = 3
            // bit16
            // arraySize = size * 2
        case LoadFormat::LoadFormat_R8_UINT:
        case LoadFormat::LoadFormat_R8G8_UINT:
        case LoadFormat::LoadFormat_R8G8B8A8_UINT:
            {
                // dataType = TextureFileExportDataType::Bit8;
                // passCount = 4;
            }
            // pass = 4
            // bit8
            // arraySize = size * 4

            // TODO:
            break;
        }
        // ------------------------------------------------
        {
        case LoadFormat::LoadFormat_R16G16B16A16_SFloat:
            ExportFile(Texture, TextureFileExportDataType::Half, 4, data.MipCount);
            break;
        case LoadFormat::LoadFormat_R16G16_SFloat:
            ExportFile(Texture, TextureFileExportDataType::Half, 2, data.MipCount);
            break;
        case LoadFormat::LoadFormat_R32G32B32A32_SFloat:
            ExportFile(Texture, TextureFileExportDataType::Bit32, 4, data.MipCount);
            break;
        case LoadFormat::LoadFormat_R16G16B16A16_UNorm:
            ExportFile(Texture, TextureFileExportDataType::Bit16, 4, data.MipCount);
        case LoadFormat::LoadFormat_R16G16_UNorm:
            ExportFile(Texture, TextureFileExportDataType::Bit16, 2, data.MipCount);
            break;
        case LoadFormat::LoadFormat_R8G8B8A8_UNorm:
            ExportFile(Texture, TextureFileExportDataType::Bit8, 4, data.MipCount);
            break;
        case LoadFormat::LoadFormat_R16_UNorm:
            ExportFile(Texture, TextureFileExportDataType::Bit16, 1, data.MipCount);
            break;
        case LoadFormat::LoadFormat_R32_SFloat:
            ExportFile(Texture, TextureFileExportDataType::Bit32, 1, data.MipCount);
            break;

            // -------------------------------------

        case LoadFormat::LoadFormat_BC5U:
            ExportFileJob(Texture, LoadFormat::LoadFormat_BC5U, 4, 1, data.MipCount);
            break;
        case LoadFormat::LoadFormat_BC5S:
            ExportFileJob(Texture, LoadFormat::LoadFormat_BC5S, 4, 1, data.MipCount);
            break;
        case LoadFormat::LoadFormat_BC4S:
            ExportFileJob(Texture, LoadFormat::LoadFormat_BC4S, 2, 2, data.MipCount);
            break;
        case LoadFormat::LoadFormat_BC4U:
            ExportFileJob(Texture, LoadFormat::LoadFormat_BC4U, 2, 2, data.MipCount);
            break;
        }
    }

    MiTextureCompress::GetInstance()->RendererBase->ExecuteAllTask();
    MiTextureCompress::GetInstance()->RendererBase->WaitAllTask();
}

void TextureExport::Export3D()
{
}

void TextureExport::ExportCube()
{
}

uint TextureExport::GetMipCount(LoadFormat type)
{
    uint2 sizeTmp = size;
    int32 mipCount = 0;
    int64 fileSize = 0;

    while (true)
    {
        uint pixel = sizeTmp.x * sizeTmp.y;
        switch (type)
        {
        case LoadFormat::LoadFormat_R8_UINT:
        case LoadFormat::LoadFormat_BC5S:
        case LoadFormat::LoadFormat_BC5U:
        case LoadFormat::LoadFormat_BC6H:
        case LoadFormat::LoadFormat_BC7:
            fileSize = pixel;
            break;
        case LoadFormat::LoadFormat_BC4S:
        case LoadFormat::LoadFormat_BC4U:
            fileSize = pixel / 2;
            break;
        case LoadFormat::LoadFormat_R16_UNorm:
        case LoadFormat::LoadFormat_R16_UINT:
        case LoadFormat::LoadFormat_R8G8_UINT:
            fileSize = pixel * 2;
            break;
        case LoadFormat::LoadFormat_R32_SFloat:
        case LoadFormat::LoadFormat_R8G8B8A8_UINT:
        case LoadFormat::LoadFormat_R16G16_UINT:
        case LoadFormat::LoadFormat_R32_UINT:
        case LoadFormat::LoadFormat_R16G16_SFloat:
        case LoadFormat::LoadFormat_R16G16_UNorm:
        case LoadFormat::LoadFormat_R8G8B8A8_UNorm:
            fileSize = pixel * 4;
            break;
        case LoadFormat::LoadFormat_R16G16B16A16_UNorm:
        case LoadFormat::LoadFormat_R16G16B16A16_SFloat:
        case LoadFormat::LoadFormat_R32G32_UINT:
        case LoadFormat::LoadFormat_R16G16B16A16_UINT:
            fileSize = pixel * 8;
            break;
        case LoadFormat::LoadFormat_R32G32B32A32_UINT:
        case LoadFormat::LoadFormat_R32G32B32A32_SFloat:
            fileSize = pixel * 16;
            break;
        }
        if (fileSize < 65536)
            return Max(mipCount, 1);
        mipCount++;
        sizeTmp.x /= 2;
        sizeTmp.y /= 2;
        sizeTmp.x = Max(sizeTmp.x, (uint)1);
        sizeTmp.y = Max(sizeTmp.y, (uint)1);
    }
}

// void TextureExport::GetTexture2DPixel(vengine::vector<float4>& data, UTexture2D* Texture)
// {
//     TextureCompressionSettings OldCompressionSettings = Texture->CompressionSettings;
//     TextureMipGenSettings OldMipGenSettings = Texture->MipGenSettings;
//     bool OldSRGB = Texture->SRGB;
//     Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
//     Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
//     Texture->SRGB = false;
//     Texture->UpdateResource();
//
//     const FColor* ImageData = static_cast<const FColor*>(Texture->PlatformData->Mips[0].BulkData.LockReadOnly());
//
//     if (ImageData == nullptr)
//     {
//         UE_LOG(LogTemp, Display, TEXT("像素数据读取失败：%s"), *Texture->GetName());
//         return;
//     }
//     for (int32 X = 0; X < Texture->GetSizeX(); X++)
//     {
//         for (int32 Y = 0; Y < Texture->GetSizeY(); Y++)
//         {
//             FLinearColor PixelColor = (ImageData[Y * Texture->GetSizeX() + X]).ReinterpretAsLinear();
//             data.push_back(float4(PixelColor.R, PixelColor.G, PixelColor.B, PixelColor.A));
//         }
//     }
//
//     Texture->PlatformData->Mips[0].BulkData.Unlock();
//     Texture->CompressionSettings = OldCompressionSettings;
//     Texture->MipGenSettings = OldMipGenSettings;
//     Texture->SRGB = OldSRGB;
//     Texture->UpdateResource();
// }


void TextureExport::GetTexture2DPixel(vengine::vector<float4>& data, UTexture2D* Texture)
{
    EPixelFormat texType = Texture->GetPixelFormat();
    FByteBulkData* ImageData = &Texture->PlatformData->Mips[0].BulkData;

    if ((ImageData->IsBulkDataLoaded() && ImageData->GetBulkDataSize() > 0)
            &&	(texType == PF_B8G8R8A8 || texType == PF_FloatRGBA))
    {
        if(texType == PF_B8G8R8A8)
        {
            FColor* colorData = static_cast<FColor*>(ImageData->Lock(LOCK_READ_ONLY));
            for (int32 X = 0; X < Texture->GetSizeX(); X++)
            {
                for (int32 Y = 0; Y < Texture->GetSizeY(); Y++)
                {
                    FLinearColor PixelColor = colorData[Y * Texture->GetSizeX() + X].ReinterpretAsLinear();
                    data.push_back(float4(PixelColor.R, PixelColor.G, PixelColor.B, PixelColor.A));
                }
            }
            ImageData->Unlock();
        }
        else if(texType == PF_FloatRGBA)
        {
            FFloat16Color* colorData = static_cast<FFloat16Color*>(ImageData->Lock(LOCK_READ_ONLY));
            for (int32 X = 0; X < Texture->GetSizeX(); X++)
            {
                for (int32 Y = 0; Y < Texture->GetSizeY(); Y++)
                {
                    FFloat16Color PixelColor = colorData[Y * Texture->GetSizeX() + X];
                    data.push_back(float4(PixelColor.R, PixelColor.G, PixelColor.B, PixelColor.A));
                }
            }
            ImageData->Unlock();
        }
    }
    else
    {
        FTextureSource& TextureSource = Texture->Source;
        
        TArray64<uint8> SourceData;
        Texture->Source.GetMipData(SourceData, 0);
        
        ETextureSourceFormat SourceFormat = TextureSource.GetFormat();
        for (int32 X = 0; X < Texture->GetSizeX(); X++)
        {
            for (int32 Y = 0; Y < Texture->GetSizeY(); Y++)
            {
                int32 Index = ((X * Texture->GetSizeY()) + Y) * TextureSource.GetBytesPerPixel();

                if ((SourceFormat == TSF_BGRA8 || SourceFormat == TSF_BGRE8))
                {
                    const uint8* PixelPtr = SourceData.GetData() + Index;
                    FLinearColor PixelColor = (*((FColor*)PixelPtr)).ReinterpretAsLinear();
                    data.push_back(float4(PixelColor.R, PixelColor.G, PixelColor.B, PixelColor.A));
                }
                else if ((SourceFormat == TSF_RGBA16 || SourceFormat == TSF_RGBA16F))
                {
                    const uint8* PixelPtr = SourceData.GetData() + Index;
                    FFloat16Color PixelColor = *((FFloat16Color*)PixelPtr);
                    data.push_back(float4(float(PixelColor.R), float(PixelColor.G), float(PixelColor.B), float(PixelColor.A)));
                }
                else if (SourceFormat == TSF_G8)
                {
                    const uint8* PixelPtr = SourceData.GetData() + Index;
                    const uint8 PixelColor = *PixelPtr;
                    data.push_back(float4(float(PixelColor), 0, 0, 0));
                }
            }
        }
    }

    if(Texture->SRGB)
    {
        // for (size_t i = 0; i < data.size(); ++i)
        // {
        //     data[i] = pow(data[i],2.2f);
        // }
        // UE_LOG(LogTemp, Display, TEXT("Test1 x:%d y:%d z:%d"), data[0].x,data[0].y,data[0].z);
        SRGBJob job;
        job.data = &data;
        uint threadMaxCount = std::thread::hardware_concurrency();
        JobSystem system(threadMaxCount);
        JobBucket* bucket = system.GetJobBucket();
        JobHandle parallelHandle = bucket->GetParallelTask({}, data.size(), threadMaxCount,
                                                           [&](uint kernel)-> void
                                                           {
                                                               job.Execute(kernel);
                                                           });
        system.ExecuteBucket(bucket, 1);
        system.Wait();
        // UE_LOG(LogTemp, Display, TEXT("Test2 x:%d y:%d z:%d"), data[0].x,data[0].y,data[0].z);
    }
    
}


void TextureExport::ExportFile(UTexture2D* Texture, TextureFileExportDataType dataType, uint passCount,
                               uint mipmapcount)
{
    vengine::vector<float4> teData;

    // get all mipmap size
    uint64_t byteSize = 0;
    uint2 tmp = size;
    for (uint i = 0; i < mipmapcount; ++i)
    {
        byteSize += tmp.x * tmp.y;
        tmp.x /= 2;
        tmp.y /= 2;
        tmp.x = Max(tmp.x, (uint)1);
        tmp.y = Max(tmp.y, (uint)1);
    }

    teData.reserve(byteSize);
    teData.clear();

    GetTexture2DPixel(teData, Texture);
    // DLL Import
    MiTextureCompress::GetInstance()->RendererBase->AddTextureMipTask(size.x,size.y,mipmapcount,teData.data());
    MiTextureCompress::GetInstance()->RendererBase->ExecuteAllTask();
    MiTextureCompress::GetInstance()->RendererBase->WaitAllTask();

    teData.resize(byteSize);

    float4* ptr = teData.data();
    for (uint i = 0; i < mipmapcount; ++i)
    {
        fileExport.DataTransfer(size, (byte*)ptr, dataType, passCount);
        ptr += size.x * size.y;
        size.x /= 2;
        size.y /= 2;
        size.x = Max(size.x, (uint)1);
        size.y = Max(size.y, (uint)1);
    }
    fileExport.ExportFile(setting.Path);
}

void TextureExport::ExportFileJob(UTexture2D* Texture, LoadFormat dataType, uint passCount, uint tmp, uint mipmapcount)
{
    uint64_t byteSize = 0;
    uint2 ttmp = size;
    vengine::vector<float4> teData;
    for (uint i = 0; i < mipmapcount; ++i)
    {
        byteSize += ttmp.x * ttmp.y;
        ttmp.x /= 2;
        ttmp.y /= 2;
        ttmp.x = Max(ttmp.x, (uint)1);
        ttmp.y = Max(ttmp.y, (uint)1);
    }

    teData.reserve(byteSize);
    teData.clear();
    auto ptr = teData.data();
    GetTexture2DPixel(teData, Texture);
    auto renderBase = MiTextureCompress::GetInstance()->RendererBase;
    renderBase->AddTextureMipTask(
        size.x,
        size.y,
        mipmapcount,
        ptr
    );
    renderBase->ExecuteAllTask();
    renderBase->WaitAllTask();
    std::vector<byte> compressedData;
    for (uint i = 0; i < mipmapcount; ++i)
    {
        uint cum = size.x * size.y;
        compressedData.clear();
        compressedData.resize(cum / tmp);
        CompressJob job;
        job.compressType = dataType;
        job.dest = (uint2*)compressedData.data();

        job.source = ptr;
        job.width = size.x;
        uint threadMaxCount = std::thread::hardware_concurrency();
        JobSystem system(threadMaxCount);
        JobBucket* bucket = system.GetJobBucket();
        JobHandle parallelHandle = bucket->GetParallelTask({}, cum / 16, threadMaxCount,
                                                           [&](uint kernel)-> void
                                                           {
                                                               job.Execute(kernel);
                                                           });
        system.ExecuteBucket(bucket, 1);
        system.Wait();
        uint2 finalSize;
        finalSize.x = Max((uint)1, (size.x + 3) >> 2);
        finalSize.y = Max((uint)1, (size.y + 3) >> 2);
        fileExport.DataTransfer(finalSize, (byte*)job.dest, TextureFileExportDataType::Bit32, passCount);
        ptr += size.x * size.y;
        size.x /= 2;
        size.y /= 2;
        size.x = Max(size.x, (uint)1);
        size.y = Max(size.y, (uint)1);
    }
    fileExport.ExportFile(setting.Path);
}
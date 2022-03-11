// Fill out your copyright notice in the Description page of Project Settings.

#include "MiSceneExport.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "ImageUtils.h"
#include "LandscapeComponent.h"
#include "MiSceneExportDatas.h"
#include "MiShaderExport.h"
#include "Common/MObject.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "EditorFramework/AssetImportData.h"
#include "MiTool/Public/MeshExport.h"
#include "MiTool/Public/TextureExport.h"
#include "SceneExport/ExportType.h"
#include "SceneExport/MiJson.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/FileHelper.h"
#include "Rendering/SkeletalMeshModel.h"

//DynamicDLL* UMiSceneExport::dynamicLoadedDLL = nullptr;
UMiSceneExportDatas* UMiSceneExport::DataPtr = nullptr;
std::unique_ptr<MiJson> UMiSceneExport::assetsDataJson;
std::unique_ptr<MiJson> UMiSceneExport::scenesDataJson;
std::unique_ptr<MiJson> UMiSceneExport::xXSceneDataJson;

void UMiSceneExport::MiExport()
{
    DataPtr = GEditor->GetEditorSubsystem<UMiSceneExportDatas>();

    UE_LOG(LogTemp, Display, TEXT("开始进行场景导出"));

    vengine::string databaseExportPath = ToVString(
        DataPtr->EnginePath + '/' + DataPtr->JSONPath + "/AssetsDatabase.json");
    vengine::string sceneDataExportPath = ToVString(DataPtr->EnginePath + '/' + DataPtr->JSONPath + "/ScenesData.json");
    assetsDataJson = std::unique_ptr<MiJson>(new MiJson(databaseExportPath));
    scenesDataJson = std::unique_ptr<MiJson>(new MiJson(sceneDataExportPath));


    {
        xXSceneDataJson = std::unique_ptr<MiJson>(new MiJson());
        vengine::string jsonPath = ToVString(DataPtr->EnginePath + '/' + DataPtr->JSONPath + "/TestLevel.json");

        ExportAllComponent();

        xXSceneDataJson->ExportFile(jsonPath);
        xXSceneDataJson->ExportFileTest(jsonPath+".meowtest");
        scenesDataJson->Delete("TestLevel");
        scenesDataJson->Add("TestLevel", SplitPath(jsonPath));
        xXSceneDataJson = nullptr;
    }

    assetsDataJson->ExportFile(databaseExportPath);
    assetsDataJson->ExportFileTest(databaseExportPath+".meowtest");
    scenesDataJson->ExportFile(sceneDataExportPath);
    scenesDataJson->ExportFileTest(sceneDataExportPath+".meowtest");
    MiShaderExport::Export(ToVString(DataPtr->EnginePath + '/' + DataPtr->JSONPath + '/'));

    assetsDataJson = nullptr;
    scenesDataJson = nullptr;
    DataPtr->ExporterAsset.Clear();


    UE_LOG(LogTemp, Display, TEXT("场景导出成功"));
}

FString UMiSceneExport::MiOpenFile(const FString& InTitle, const FString& InDefaultPath, const FString& InDefaultFile,
                                   const FString& InFileType)
{
    FString OutPath;
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    const void* ParentWindow = FSlateApplication::Get()
                               .GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();

    if (ParentWindow == nullptr)
        return OutPath;
    if (DesktopPlatform == nullptr)
        return OutPath;

    TArray<FString> Paths;
    DesktopPlatform->OpenFileDialog(ParentWindow, InTitle, InDefaultPath, InDefaultFile
                                    , InFileType, EFileDialogFlags::None, Paths);

    if (Paths.Num() > 0)
        OutPath = Paths[0];
    GetData(OutPath);
    return OutPath;
}

FString UMiSceneExport::MiOpenDictionary(const FString& InTitle, const FString& InDefaultPath)
{
    FString OutPath;
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    const void* ParentWindow = FSlateApplication::Get()
                               .GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();

    if (ParentWindow == nullptr)
        return OutPath;
    if (DesktopPlatform == nullptr)
        return OutPath;

    DesktopPlatform->OpenDirectoryDialog(ParentWindow, InTitle, InDefaultPath, OutPath);
    return OutPath;
}

bool UMiSceneExport::ExportUTexture2D(UTexture2D* Img, const FString& fileDestination)
{
    FTexture2DMipMap& mipmaps = Img->PlatformData->Mips[0];
    unsigned char* Data = (unsigned char *)mipmaps.BulkData.Lock(LOCK_READ_WRITE);
    int texturex = Img->PlatformData->SizeX;
    int texturey = Img->PlatformData->SizeY;
    TArray<FColor> nColors;
    float bPerUV_v = 1.0 / texturey;
    for (int32 y = 0; y < texturey; y++)
    {
        for (int32 x = 0; x < texturex; x++)
        {
            FColor bColor;
            bColor.B = Data[(y*texturex + x) * 4 + 0];//B
            bColor.G = Data[(y*texturex + x) * 4 + 1];//G
            bColor.R = Data[(y*texturex + x) * 4 + 2];//R
            bColor.A = Data[(y*texturex + x) * 4 + 3];//A 0:全透明；255:全不透明
            nColors.Add(bColor);
        }
    }
    //int32 strwide = (int32)(sizeof(uint8) * 4);
    //FMemory::Memcpy(Data, colors.GetData(), strwide*texturey*texturex);
    mipmaps.BulkData.Unlock();
    Img->UpdateResource();
        
    //保存，思路，拿到Color的数据，然后利用FImageUtils转换图片数据，最后写入
    TArray<uint8> ImgData;
    FImageUtils::CompressImageArray(texturex, texturey, nColors, ImgData);
    bool re= FFileHelper::SaveArrayToFile(ImgData, *fileDestination);
    return re;
}

FString UMiSceneExport::GetLandscapePath(const FString& path)
{
    FString result = "/Landscape" + path + ".temp";
    return result.Replace(TEXT(":"),TEXT("_"));
}

float4x4 UMiSceneExport::GetWorldMatrix(const float4& quat,const float3& pos)
{
    float4x4 m = float4x4(
        0, 0, 0, pos.x,
        0, 0, 0, pos.y,
        0, 0, 0, pos.z, 
        0, 0, 0, 0);

    float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    
    m._11 = 1.0 - (yy + zz);
    m._12 = xy + wz;
    m._13 = xz - wy;

    m._21 = xy - wz;
    m._22 = 1.0 - (xx + zz);
    m._23 = yz + wx;

    m._31 = xz + wy;
    m._32 = yz - wx;
    m._33 = 1.0 - (xx + yy);

    m._44 = 1.0;

    return m;
}


void UMiSceneExport::ExportAllComponent()
{
    for (TActorIterator<AActor> It(GWorld); It; ++It)
    {
        FString ActorName = It->GetName();

        TArray<UActorComponent*> ActorComps;
        It->GetComponents(USceneComponent::StaticClass(), ActorComps);
        for (UActorComponent* Comp : ActorComps)
        {
            USceneComponent* AsScene = Cast<USceneComponent>(Comp);
            UStaticMeshComponent* AsStaticMesh = Cast<UStaticMeshComponent>(Comp);
            USkeletalMeshComponent* AsSkeletalMesh = Cast<USkeletalMeshComponent>(Comp);
            UInstancedStaticMeshComponent* AsInstancedMesh = Cast<UInstancedStaticMeshComponent>(Comp);
            ULandscapeComponent* AsLandscape = Cast<ULandscapeComponent>(Comp);

            if (AsStaticMesh == nullptr && AsSkeletalMesh == nullptr
                && AsInstancedMesh == nullptr && AsLandscape == nullptr)
                continue;
            // GameObject Name
            MiJson jsonData;
            if (Comp->GetName() == "SkySphereMesh")
                continue;
            jsonData.Add("name", ToVString(Comp->GetName()));

            // Transform导出
            const FTransform& CompTransform = AsScene->GetComponentTransform();
            
            // FBX WorldMatrix
            float4x4 WorldMatrix;
            // mesh 原始文件类型
            FString meshImportType = "default";
            if (AsStaticMesh != nullptr)
            {
                UStaticMesh* StaticMesh = AsStaticMesh->GetStaticMesh();

                FAssetImportInfo tmpData = StaticMesh->AssetImportData->GetSourceData();
                for(size_t i = 0; i < tmpData.SourceFiles.Num(); ++i)
                {
                    meshImportType = FPaths::GetExtension(tmpData.SourceFiles[0].RelativeFilename);
                    // WorldMatrix = GetWorldMatrix(float4(CompTransform.GetRotation().X,-CompTransform.GetRotation().Y,CompTransform.GetRotation().Z,CompTransform.GetRotation().W),
                    // float3(CompTransform.TransformPosition(FVector(0,0,0)).X,CompTransform.TransformPosition(FVector(0,0,0)).Y,CompTransform.TransformPosition(FVector(0,0,0)).Z));
                }
            }
            
            
            ExportTransform tr;
            tr.position = float3(-CompTransform.GetLocation().X * 0.01f, CompTransform.GetLocation().Z * 0.01f,
                                 CompTransform.GetLocation().Y * 0.01f);
            tr.rotation = float4(-CompTransform.GetRotation().X, CompTransform.GetRotation().Z,
                                 CompTransform.GetRotation().Y, CompTransform.GetRotation().W);
            // if(meshImportType == "FBX")
            // {
            //     tr.localscale = float3(CompTransform.GetScale3D().X, CompTransform.GetScale3D().Z,
            //                            CompTransform.GetScale3D().Y);
            // }
            // else
            // {
            //     tr.localscale = float3(CompTransform.GetScale3D().X, CompTransform.GetScale3D().Z,
            //                            CompTransform.GetScale3D().Y);
            // }
            jsonData.Add(&tr);     

            if(AsLandscape != nullptr)
            {
                UTexture2D* heightMap = AsLandscape->GetHeightmap();
                LoadAsset(heightMap, ToVString(heightMap->GetPathName()), ToVString(GetLandscapePath(heightMap->GetPathName())), TextureExportType::SingleChannel);
                TArray<vengine::string> weightMapsGUID;
                TArray<UTexture2D*> weightMaps = AsLandscape->GetWeightmapTextures();
                for(size_t i = 0; i < weightMaps.Num(); ++i)
                {
                    weightMapsGUID.Add(ToVString(weightMaps[i]->GetPathName()));
                    ExportUTexture2D(weightMaps[i],"C:\\Users\\wenjie.li\\Desktop\\2333" + GetLandscapePath(heightMap->GetPathName()) + ".png");
                }
                ExportLandscape Landscape;
                Landscape.heightMap = ToVString(heightMap->GetPathName());
                Landscape.weightMapCount = weightMapsGUID.Num();
                Landscape.weightMaps = weightMapsGUID.GetData();
                jsonData.Add(&Landscape);
            }
            else if (AsStaticMesh != nullptr)
            {
                UStaticMesh* StaticMesh = AsStaticMesh->GetStaticMesh();

                if (StaticMesh)
                {
                    TArray<float> meshDises;
                    TArray<vengine::string> meshGUID;
                    TArray<vengine::string> matGUID;

                    auto Materials = AsStaticMesh->GetMaterials();
                    for (int32 i = 0; i < Materials.Num(); ++i)
                    {
                        auto Material = Materials[i];
                        if (ToVString(Material->GetBaseMaterial()->GetPathName()) == "/Game/Meshes/BaseMat.BaseMat")
                        {
                            MiShaderExport::Import("BaseShader");
                            LoadAssetBase(Material, ToVString(Material->GetPathName()));
                        }
                        else if (ToVString(Material->GetBaseMaterial()->GetPathName()) ==
                            "/Game/Meshes/m_Horizon_rock.m_horizon_rock")
                        {
                            MiShaderExport::Import("RockShader");
                            LoadAssetRock(Material, ToVString(Material->GetPathName()));
                        }
                        matGUID.Add(ToVString(Material->GetPathName()));
                    }


                    // LOD数量
                    int32 LODCount = StaticMesh->GetNumLODs();
                    for (int32 i = 0; i < LODCount; i++)
                    {
                        vengine::string GUID;
                        const FStaticMeshLODResources& RenderMesh = StaticMesh->GetLODForExport(i);
                        if (LODCount > 1)
                        {
                            FString s = ChangeFileName(StaticMesh->GetPathName(), FString::FromInt(i));
                            GUID = ToVString(s);
                        }
                        else
                        {
                            GUID = ToVString(StaticMesh->GetPathName());
                        }
                        meshGUID.Add(GUID);
                        // TODO:待替换为距离
                        meshDises.Add(StaticMesh->GetSourceModel(i).ScreenSize.Default);
                        // Mesh资源导出
                        LoadAsset(RenderMesh, GUID, meshImportType, WorldMatrix);
                    }

                    // TODO: Export StaticMesh
                    ExportRenderer renderer;
                    renderer.meshCount = LODCount;
                    renderer.meshDis = meshDises.GetData();
                    renderer.meshGUID = meshGUID.GetData();
                    renderer.matCount = matGUID.Num();
                    renderer.matGUID = matGUID.GetData();
                    // TODO: 待与Unity同步
                    renderer.mask = (int)-1;
                    jsonData.Add(&renderer);
                }
            }
            else if (AsSkeletalMesh != nullptr)
            {
                USkeletalMesh* SkeletalMesh = AsSkeletalMesh->SkeletalMesh;
                if (SkeletalMesh)
                {
                    TArray<float> meshDises;
                    TArray<vengine::string> meshGUID;
                    TArray<vengine::string> matGUID;

                    auto Materials = AsSkeletalMesh->GetMaterials();
                    for (int32 i = 0; i < Materials.Num(); ++i)
                    {
                        auto Material = Materials[i];
                        // TODO: Material资源导出
                        if (ToVString(Material->GetBaseMaterial()->GetPathName()) == "/Game/Meshes/BaseMat.BaseMat")
                        {
                            LoadAssetBase(Material, ToVString(Material->GetPathName()));
                        }
                        else if (ToVString(Material->GetBaseMaterial()->GetPathName()) ==
                            "/Game/Meshes/m_Horizon_rock.m_horizon_rock")
                        {
                            LoadAssetRock(Material, ToVString(Material->GetPathName()));
                        }
                        matGUID.Add(ToVString(Material->GetPathName()));
                    }

                    // LOD数量
                    int32 LODCount = SkeletalMesh->GetLODNum();
                    const FSkeletalMeshModel* SkelMeshResource = SkeletalMesh->GetImportedModel();
                    for (int32 i = 0; i < LODCount; i++)
                    {
                        vengine::string GUID;
                        // 获取RenderMesh
                        const FSkeletalMeshLODModel& RenderMesh = SkelMeshResource->LODModels[i];
                        if (LODCount > 1)
                        {
                            FString s = ChangeFileName(SkeletalMesh->GetPathName(), FString::FromInt(i));
                            GUID = ToVString(s);
                        }
                        else
                        {
                            GUID = ToVString(SkeletalMesh->GetPathName());
                        }
                        meshGUID.Add(GUID);
                        // TODO:待替换为距离
                        meshDises.Add(SkeletalMesh->GetLODInfo(i)->ScreenSize.Default);
                        // Mesh资源导出
                        LoadAsset(RenderMesh, GUID);
                    }

                    // TODO: Export SkinnedRenderer
                    ExportSkinnedRenderer renderer;
                    renderer.meshCount = LODCount;
                    renderer.meshDis = meshDises.GetData();
                    renderer.meshGUID = meshGUID.GetData();
                    renderer.matCount = matGUID.Num();
                    renderer.matGUID = matGUID.GetData();
                    // TODO: 待与Unity同步
                    renderer.mask = (int)-1;
                    jsonData.Add(&renderer);
                }
            }
            else if (AsInstancedMesh != nullptr)
            {
                UStaticMesh* StaticMesh = AsInstancedMesh->GetStaticMesh();
                TArray<UMaterialInterface*> Materials = AsInstancedMesh->GetMaterials();
                int32 InstanceCount = AsInstancedMesh->GetInstanceCount();
                if (StaticMesh)
                {
                    for (int32 i = 0; i < InstanceCount; ++i)
                    {
                        FTransform InstanceTransform;
                        AsInstancedMesh->GetInstanceTransform(i, InstanceTransform, true);

                        // TODO: Export Static Mesh

                        // TODO: Export Materials 
                    }
                }
            }
            xXSceneDataJson->Add((CJsonObject*)jsonData.GetJsonPtr());
        }
    }
}

vengine::string UMiSceneExport::ToVString(const FString& str)
{
    wchar_t* wstr = (wchar_t*)GetData(str);
    return StringUtil::WcharToVString(wstr);
}

FString UMiSceneExport::ToFString(const vengine::string& str)
{
    return StringUtil::VStringToWchar(str);
}

vengine::string UMiSceneExport::SplitPath(const vengine::string& path)
{
    vengine::vector<vengine::string> result;
    StringUtil::Split(path, ToVString(DataPtr->EnginePath + '/'), result);
    return result[0];
}

void UMiSceneExport::LoadAsset(const FSkeletalMeshLODModel& RenderMesh, const vengine::string& GUID)
{
    if (DataPtr->ExporterAsset.Contains(GUID)
        || RenderMesh.Sections.Num() == 0)
        return;
    FString path = DataPtr->EnginePath + '/' + DataPtr->ExportPath + FPaths::ChangeExtension(ToFString(GUID), "vmesh");

    // 资源导出
    MeshExport::ExportMesh(RenderMesh, ToVString(path));

    DataPtr->ExporterAsset.Insert(GUID, ToVString(path));
    assetsDataJson->Delete(GUID);
    assetsDataJson->Add(GUID, SplitPath(ToVString(path)));
}

void UMiSceneExport::LoadAsset(const FStaticMeshLODResources& RenderMesh, const vengine::string& GUID, const FString& meshImportType,const float4x4& WorldMatrix)
{
    if (DataPtr->ExporterAsset.Contains(GUID)
        || RenderMesh.VertexBuffers.StaticMeshVertexBuffer.GetNumVertices() == 0
        || RenderMesh.Sections.Num() == 0)
        return;

    FString path = DataPtr->EnginePath + '/' + DataPtr->ExportPath + FPaths::ChangeExtension(ToFString(GUID), "vmesh");

    // 资源导出
    MeshExport::ExportMesh(RenderMesh, ToVString(path),meshImportType,WorldMatrix);

    DataPtr->ExporterAsset.Insert(GUID, ToVString(path));
    assetsDataJson->Delete(GUID);
    assetsDataJson->Add(GUID, SplitPath(ToVString(path)));
}

void UMiSceneExport::LoadAssetBase(UMaterialInterface* Material, const vengine::string& GUID)
{
    UTexture* BaseColor;
    UTexture* Emissive;
    UTexture* Normal;
    UTexture* OpacityMask;
    UTexture* SMO;

    //Material->GetAllScalarParameterInfo();

    GetTextureValue(Material,TEXT("BaseColor"),BaseColor);
    GetTextureValue(Material,TEXT("Emissive"),Emissive);
    GetTextureValue(Material,TEXT("Normal"),Normal);
    GetTextureValue(Material,TEXT("OpacityMask"),OpacityMask);
    GetTextureValue(Material,TEXT("SMO"),SMO);

    LoadAsset(BaseColor, ToVString(BaseColor->GetPathName()), TextureExportType::RGB);
    LoadAsset(Normal, ToVString(Normal->GetPathName()), TextureExportType::Normal);
    LoadAsset(SMO, ToVString(SMO->GetPathName()), TextureExportType::RGB);
    LoadAsset(OpacityMask, ToVString(OpacityMask->GetPathName()), TextureExportType::SingleChannel);
    LoadAsset(Emissive, ToVString(Emissive->GetPathName()), TextureExportType::RGB);


    FString path = DataPtr->EnginePath + '/' + DataPtr->ExportPath + FPaths::ChangeExtension(ToFString(GUID), "mat");
    // TODO: 待撤销
    //path = "Resource/res.mat";

    MaterialExportBase(Material, ToVString(path));

    DataPtr->ExporterAsset.Insert(GUID, ToVString(path));
    assetsDataJson->Delete(GUID);
    assetsDataJson->Add(GUID, SplitPath(ToVString(path)));
}


void UMiSceneExport::LoadAssetRock(UMaterialInterface* Material, const vengine::string& GUID)
{
    TArray<UTexture*> Textures;

    UTexture* NormalTex;
    UTexture* DetailNormalTex;
    UTexture* LUTColor;
    UTexture* Noise;
    UTexture* UVOC;
    UTexture* TCV;

    GetTextureValue(Material,TEXT("mesh_normalTex"), NormalTex);
    GetTextureValue(Material,TEXT("mesh_DetailnormalTex"), DetailNormalTex);
    GetTextureValue(Material,TEXT("LUTColor"), LUTColor);
    GetTextureValue(Material,TEXT("noise_tex"), Noise);
    GetTextureValue(Material,TEXT("UV_OC"), UVOC);
    GetTextureValue(Material,TEXT("Tilling_CV"), TCV);

    // bc5
    LoadAsset(NormalTex, ToVString(NormalTex->GetPathName()), TextureExportType::Normal);
    LoadAsset(DetailNormalTex, ToVString(DetailNormalTex->GetPathName()), TextureExportType::Normal);
    // bc7
    LoadAsset(LUTColor, ToVString(LUTColor->GetPathName()), TextureExportType::RGBA);
    LoadAsset(Noise, ToVString(Noise->GetPathName()), TextureExportType::RGBA);
    // bc5
    LoadAsset(UVOC, ToVString(UVOC->GetPathName()), TextureExportType::Normal);
    // bc4
    LoadAsset(TCV, ToVString(TCV->GetPathName()), TextureExportType::SingleChannel);

    FString path = DataPtr->EnginePath + '/' + DataPtr->ExportPath + FPaths::ChangeExtension(ToFString(GUID), "mat");

    MaterialExportRock(Material, ToVString(path));

    DataPtr->ExporterAsset.Insert(GUID, ToVString(path));
    assetsDataJson->Delete(GUID);
    assetsDataJson->Add(GUID, SplitPath(ToVString(path)));
}

void UMiSceneExport::LoadAsset(UTexture* Texture, const vengine::string& GUID, const vengine::string& tPath, TextureExportType ExportType)
{
    if(Texture == nullptr)
        return;
    FString path = DataPtr->EnginePath + '/' + DataPtr->ExportPath + FPaths::ChangeExtension(ToFString(tPath), "vtex");
    //UE_LOG(LogTemp, Display, TEXT("Landscape %s"),*path);
    TextureExportSetting ExportSetting;
    ExportSetting.Texture = Texture;
    ExportSetting.Path = ToVString(path);
    ExportSetting.ExportType = ExportType;
    ExportSetting.UseMipMap = true;
    ExportSetting.UseAlphaInCompress = ExportType == TextureExportType::RGBA ? true : false;

    // TODO：待重构
    TextureExport(std::move(ExportSetting));

    DataPtr->ExporterAsset.Insert(GUID, ToVString(path));
    assetsDataJson->Delete(GUID);
    assetsDataJson->Add(GUID, SplitPath(ToVString(path)));
}

void UMiSceneExport::LoadAsset(UTexture* Texture, const vengine::string& GUID, TextureExportType ExportType)
{
    LoadAsset(Texture,GUID,GUID,ExportType);
}


FString UMiSceneExport::ChangeFileName(const FString& InPath, const FString& AddNew)
{
    int32 Pos = INDEX_NONE;
    if (InPath.FindLastChar('.', Pos))
    {
        auto IsSlashOrBackslash = [](TCHAR C) { return C == TEXT('/') || C == TEXT('\\'); };
        const int32 PathEndPos = InPath.FindLastCharByPredicate(IsSlashOrBackslash);
        if (PathEndPos != INDEX_NONE && PathEndPos > Pos)
        {
            // The dot found was part of the path rather than the name
            Pos = INDEX_NONE;
        }
    }

    if (Pos != INDEX_NONE)
    {
        FString Result = InPath.Left(Pos);
        Result += AddNew;
        Result += "." + FPaths::GetExtension(InPath);

        return Result;
    }

    return InPath;
}

// 0 透明 1不透明 2半透明
void UMiSceneExport::MaterialExportBase(UMaterialInterface* Material, vengine::string Path)
{
    //  ==== Test
    // FString MarkupSource;
    // Material->GetBaseMaterial()->GetMaterialResource(ERHIFeatureLevel::SM5)->GetMaterialExpressionSource(MarkupSource);
    // FileUtility::ExportTextFile("C:\\Users\\wenjie.li\\Desktop\\2333\\Shader.hlsl", ToVString(MarkupSource));
    //  ==== Test

    UTexture* BaseColor;
    UTexture* Emissive;
    UTexture* Normal;
    UTexture* OpacityMask;
    UTexture* SMO;

    GetTextureValue(Material,TEXT("BaseColor"),BaseColor);
    GetTextureValue(Material,TEXT("Emissive"),Emissive);
    GetTextureValue(Material,TEXT("Normal"),Normal);
    GetTextureValue(Material,TEXT("OpacityMask"),OpacityMask);
    GetTextureValue(Material,TEXT("SMO"),SMO);

    float EmissionMultiplier;
    float Cutoff;
    float Glossiness;
    float MetallicIntensity;
    float Occlusion;
    Material->GetScalarParameterValue(TEXT("EmissionMultiplier"), EmissionMultiplier);
    Material->GetScalarParameterValue(TEXT("Cutoff"), Cutoff);
    Material->GetScalarParameterValue(TEXT("Glossiness"), Glossiness);
    Material->GetScalarParameterValue(TEXT("MetallicIntensity"), MetallicIntensity);
    Material->GetScalarParameterValue(TEXT("Occlusion"), Occlusion);

    FLinearColor Color;
    FLinearColor EmissiveColor;
    FLinearColor TileOffset;
    Material->GetVectorParameterValue(TEXT("Color"), Color);
    Material->GetVectorParameterValue(TEXT("EmissiveColor"), EmissiveColor);
    Material->GetVectorParameterValue(TEXT("TileOffset"), TileOffset);

    std::unique_ptr<MiJson> matJson = std::unique_ptr<MiJson>(new MiJson(Path));
    matJson->Add("uvScale", (float*)&TileOffset, 2);
    matJson->Add("uvOffset", ((float*)&TileOffset) + 2, 2);
    matJson->Add("albedo", (float*)&Color, 4);
    float useclip = 0;
    if (Material->GetBaseMaterial()->BlendMode == EBlendMode::BLEND_Masked)
    {
        useclip = 1;
        matJson->Add("clipalpha", &Cutoff, 1);
    }
    matJson->Add("useclip", &useclip, 1);
    matJson->Add("metallic", &MetallicIntensity, 1);
    matJson->Add("emission", (float*)&EmissiveColor, 4);
    matJson->Add("smoothness", &Glossiness, 1);
    matJson->Add("occlusion", &Occlusion, 1);

    matJson->Add("albedoTexIndex", BaseColor ? ToVString(BaseColor->GetPathName()) : "0");
    matJson->Add("specularTexIndex", SMO ? ToVString(SMO->GetPathName()) : "0");
    matJson->Add("normalTexIndex", Normal ? ToVString(Normal->GetPathName()) : "0");
    matJson->Add("emissionTexIndex", Emissive ? ToVString(Emissive->GetPathName()) : "0");
    matJson->Add("clipTexIndex", Material->GetBaseMaterial()->BlendMode == EBlendMode::BLEND_Masked
                                     ? ToVString(OpacityMask->GetPathName())
                                     : "0");

    MiJson exportJson(Path);
    exportJson.Add("BaseShader", (CJsonObject*)matJson->GetJsonPtr());
    exportJson.ExportFile(Path);
    exportJson.ExportFileTest(Path + ".meowtest");
    matJson = nullptr;
}

void UMiSceneExport::GetTextureValue(UMaterialInterface* Material,const TCHAR* name, UTexture*& tex)
{
    Material->GetTextureParameterValue(name, tex);
    if(tex)
    {
        UTexture* defaultTex;
        Material->GetTextureParameterDefaultValue(name, defaultTex);
        if(defaultTex == tex)
            tex = nullptr;
    }
}

void UMiSceneExport::MaterialExportRock(UMaterialInterface* Material, vengine::string Path)
{
    float DetailNormal_uv_scale;
    float Detailnormal_intesity;
    float DetailNormal_mask_intesity;
    float Normal_mask_intesity;
    float LUTcolorpower;
    float LUTselect;
    float dirtlerp;
    float roughness_max;
    float roughness_min;
    float roughness_mask_power_intesity;
    float AOlerp;
    float dirt_mask_power_intesity;
    float AO_mask_intesity;
    Material->GetScalarParameterValue(TEXT("DetailNormal_uv_scale"), DetailNormal_uv_scale);
    Material->GetScalarParameterValue(TEXT("Detailnormal_intesity"), Detailnormal_intesity);
    Material->GetScalarParameterValue(TEXT("DetailNormal_mask_intesity"), DetailNormal_mask_intesity);
    Material->GetScalarParameterValue(TEXT("Normal_mask_intesity"), Normal_mask_intesity);
    Material->GetScalarParameterValue(TEXT("LUT color power"), LUTcolorpower);
    Material->GetScalarParameterValue(TEXT("LUT select"), LUTselect);
    Material->GetScalarParameterValue(TEXT("dirt lerp"), dirtlerp);
    Material->GetScalarParameterValue(TEXT("roughness_max"), roughness_max);
    Material->GetScalarParameterValue(TEXT("roughness_min"), roughness_min);
    Material->GetScalarParameterValue(TEXT("roughness_mask_power_intesity"), roughness_mask_power_intesity);
    Material->GetScalarParameterValue(TEXT("AO lerp"), AOlerp);
    Material->GetScalarParameterValue(TEXT("dirt_mask_power_intesity"), dirt_mask_power_intesity);
    Material->GetScalarParameterValue(TEXT("AO_mask_intesity"), AO_mask_intesity);

    FLinearColor dirtColor;
    Material->GetVectorParameterValue(TEXT("dirt color tint"), dirtColor);

    std::unique_ptr<MiJson> matJson = std::unique_ptr<MiJson>(new MiJson(Path));
    matJson->Add("DetailNormal_uv_scale", &DetailNormal_uv_scale, 1);
    matJson->Add("Detailnormal_intesity", &Detailnormal_intesity, 1);
    matJson->Add("DetailNormal_mask_intesity", &DetailNormal_mask_intesity, 1);
    matJson->Add("Normal_mask_intesity", &Normal_mask_intesity, 1);
    matJson->Add("LUTcolorpower", &LUTcolorpower, 1);
    matJson->Add("LUTselect", &LUTselect, 1);
    matJson->Add("dirtlerp", &dirtlerp, 1);
    matJson->Add("roughness_max", &roughness_max, 1);
    matJson->Add("roughness_min", &roughness_min, 1);
    matJson->Add("roughness_mask_power_intesity", &roughness_mask_power_intesity, 1);
    matJson->Add("AOlerp", &AOlerp, 1);
    matJson->Add("dirt_mask_power_intesity", &dirt_mask_power_intesity, 1);
    matJson->Add("AO_mask_intesity", &AO_mask_intesity, 1);
    matJson->Add("dirtColor", (float*)&dirtColor, 4);

    UTexture* NormalTex;
    UTexture* DetailNormalTex;
    UTexture* LUTColor;
    UTexture* Noise;
    UTexture* UVOC;
    UTexture* TCV;

    GetTextureValue(Material,TEXT("mesh_normalTex"), NormalTex);
    GetTextureValue(Material,TEXT("mesh_DetailnormalTex"), DetailNormalTex);
    GetTextureValue(Material,TEXT("LUTColor"), LUTColor);
    GetTextureValue(Material,TEXT("noise_tex"), Noise);
    GetTextureValue(Material,TEXT("UV_OC"), UVOC);
    GetTextureValue(Material,TEXT("Tilling_CV"), TCV);

    matJson->Add("NormalTexIndex", NormalTex ? ToVString(NormalTex->GetPathName()) : "0");
    matJson->Add("DetailNormalTexIndex", DetailNormalTex ? ToVString(DetailNormalTex->GetPathName()) : "0");
    matJson->Add("LUTColorTexIndex", LUTColor ? ToVString(LUTColor->GetPathName()) : "0");
    matJson->Add("NoiseTexIndex", Noise ? ToVString(Noise->GetPathName()) : "0");
    matJson->Add("UVOCTexIndex", UVOC ? ToVString(UVOC->GetPathName()) : "0");
    matJson->Add("TCVTexIndex", TCV ? ToVString(TCV->GetPathName()) : "0");
    MiJson exportJson(Path);
    exportJson.Add("RockShader", (CJsonObject*)matJson->GetJsonPtr());
    exportJson.ExportFile(Path);
    exportJson.ExportFileTest(Path+".meowtest");
    matJson = nullptr;
}

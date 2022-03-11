// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MiSceneExportDatas.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MiTool/Public/TextureExport.h"
#include "SceneExport/MiJson.h"
#include "MiSceneExport.generated.h"
//class DynamicDLL;
/**
 * 
 */
enum MatType
{
	MatRock = 0,
    MatBase = 1,
};

UCLASS()
class MITOOL_API UMiSceneExport : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	// 所有资源信息Json
	static std::unique_ptr<MiJson> assetsDataJson;
	// 所有场景信息Json
	static std::unique_ptr<MiJson> scenesDataJson;
	// 单个场景信息Json
	static std::unique_ptr<MiJson> xXSceneDataJson;
	//static DynamicDLL* dynamicLoadedDLL; 
public:

    // 场景导出入口
    UFUNCTION(BlueprintCallable)
    static void MiExport();

	// 开文件
	UFUNCTION(BlueprintCallable)
    static FString MiOpenFile(const FString& InTitle, const FString& InDefaultPath, const FString& InDefaultFile, const FString& InFileType);

	// 开文件夹
	UFUNCTION(BlueprintCallable)
    static FString MiOpenDictionary(const FString& InTitle, const FString& InDefaultPath);

	// 导出当前关卡所有组件
	static void ExportAllComponent();

	// VString与FString的互相转换
	static vengine::string ToVString(const FString& str);
	static FString ToFString(const vengine::string& str);

	// 路径拆分
	static vengine::string SplitPath(const vengine::string& path);

	// 为了拆分LOD做的给文件名后面加序号
	static FString ChangeFileName(const FString& InPath, const FString& AddNew);

	// 处理Landscape height map path
	static FString GetLandscapePath(const FString& path);

	// 资源加载
	// 现阶段GUID是Asset路径，待生成GUID后要进行调整
	static void LoadAsset(const FStaticMeshLODResources& RenderMesh,const vengine::string& GUID,const FString& meshImportType,const float4x4& WorldMatrix);
	static void LoadAsset(const FSkeletalMeshLODModel& RenderMesh,const vengine::string& GUID);
	static void LoadAssetBase(UMaterialInterface* Material,const vengine::string& GUID);
	static void LoadAssetRock(UMaterialInterface* Material,const vengine::string& GUID);
	static void LoadAsset(UTexture* Texture, const vengine::string& GUID, const vengine::string& tPath, TextureExportType ExportType);
	static void LoadAsset(UTexture* Texture, const vengine::string& GUID, TextureExportType ExportType);
	// 场景导出相关数据指针
	static UMiSceneExportDatas* DataPtr;
	static void MaterialExportBase(UMaterialInterface* Material, vengine::string path);
	static void MaterialExportRock(UMaterialInterface* Material, vengine::string Path);
	static void GetTextureValue(UMaterialInterface* Material,const TCHAR* name, UTexture*& tex);
	static bool ExportUTexture2D(UTexture2D* Img, const FString& fileDestination);
	static float4x4 GetWorldMatrix(const float4& quat,const float3& pos);
};

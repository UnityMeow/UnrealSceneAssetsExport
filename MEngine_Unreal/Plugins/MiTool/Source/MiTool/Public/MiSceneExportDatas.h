// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "VEngine_Include.h"
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "MiSceneExportDatas.generated.h"


/**
 * 
 */
UCLASS()
class MITOOL_API UMiSceneExportDatas : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
    bool IsNormal = true;
	UPROPERTY(BlueprintReadWrite)
    bool IsTangent = true;
	UPROPERTY(BlueprintReadWrite)
    bool IsUV = true;
	UPROPERTY(BlueprintReadWrite)
    bool IsVertexColor = true;
	UPROPERTY(BlueprintReadWrite)
    bool IsBone = true;

	UPROPERTY(BlueprintReadWrite)
    FString EnginePath;
	
	FString ExportPath = L"Resource/Assets";
	FString JSONPath = L"Resource";

	HashMap<vengine::string,vengine::string> ExporterAsset;
};

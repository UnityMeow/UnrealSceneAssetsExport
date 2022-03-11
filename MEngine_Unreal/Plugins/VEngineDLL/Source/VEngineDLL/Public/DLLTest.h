// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VEngine_Include.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DLLTest.generated.h"

/**
 * 
 */
UCLASS()
class VENGINEDLL_API UDLLTest : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void Test();
};

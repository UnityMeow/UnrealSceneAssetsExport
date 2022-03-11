// Fill out your copyright notice in the Description page of Project Settings.


#include "DLLTest.h"

void UDLLTest::Test()
{
    vengine::vector<uint> a;
    a.resize((10));
    UE_LOG(LogTemp, Display, TEXT("开始进行场景导出%d"),a.size());
}

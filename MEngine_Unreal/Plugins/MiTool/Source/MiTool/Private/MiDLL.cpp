// Fill out your copyright notice in the Description page of Project Settings.


#include "MiTool/Public/MiDLL.h"


std::atomic<MiDLL*> MiDLL::instance = nullptr;
std::atomic<uint> MiDLL::isWorkingOnLoading = 0;

MiDLL::MiDLL()
{
}

MiDLL::~MiDLL()
{
}

MiDLL* MiDLL::GetInstance()
{
    if (instance == nullptr)
    {
        if(!isWorkingOnLoading++)
        {
            auto tmp = new MiDLL;
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
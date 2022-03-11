// Fill out your copyright notice in the Description page of Project Settings.
//#include "VEngine_Include.h"

#include "MiTool/Public/MiTextureCompress.h"


#include "MiDLL.h"

std::atomic<MiTextureCompress*> MiTextureCompress::instance = nullptr;
std::mutex MiTextureCompress::mtx;

MiTextureCompress::MiTextureCompress()
{
}

MiTextureCompress::~MiTextureCompress()
{
    this->DisposeVEngine(this->RendererBase);
}

MiTextureCompress* MiTextureCompress::GetInstance()
{
    if (instance == nullptr)
    {
        std::lock_guard<std::mutex> lck(mtx);
        if (instance == nullptr)
        {
            auto tmp = new MiTextureCompress;
            
            FString InPath = FPaths::ProjectDir();
            wchar_t* wstr = (wchar_t*)GetData(InPath);
            vengine::string filePath = StringUtil::WcharToVString(wstr) + "Vengine_Compute.dll";

            tmp->InstantiateVEngine = MiDLL::GetInstance()->Use<IRendererBase*(const char*)>(filePath,"InstantiateVEngine");
            tmp->DisposeVEngine = MiDLL::GetInstance()->Use<void(IRendererBase*)>(filePath,"DisposeVEngine");
	
            tmp->RendererBase = tmp->InstantiateVEngine(StringUtil::WcharToVString(wstr).c_str());
            
            instance = tmp;
        } 
    }
    return instance;
}
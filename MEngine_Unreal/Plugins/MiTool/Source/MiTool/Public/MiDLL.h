// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "VEngine_Include.h"

class MiDLL
{
	// --------------------------------------------------------------
	
	MiDLL();
	~MiDLL();
	// 单例实例
	static std::atomic<MiDLL*> instance;
	// 自选锁
	static std::atomic<uint> isWorkingOnLoading;

	// --------------------------------------------------------------

	// 声明函数指针类型
	template <typename T>
    struct FunctionPtr;
	// 类型分离
	template <typename Ret, typename ... Args>
    struct FunctionPtr<Ret(Args...)>
	{
		using Ptr = typename Ret(*)(Args...);
	};
	// 拿到函数指针类型
	template <typename T>
    using FunctionPtrType = typename FunctionPtr<T>::Ptr;

	// --------------------------------------------------------------

	// DLL数据
	struct DLLData
	{
		void* Handle;
		HashMap<vengine::string, void*> Functions;
		
		DLLData()
		{
			
		}
		DLLData(const DLLData& that)
		{
			//throw "Fucked";
			this->Handle = that.Handle;
		}
		DLLData& operator=(const DLLData& that)
		{
			this->Handle = that.Handle;
			return *this;
			//throw "Fucked";
		}
	};
	// 所有加载过的DLL
	HashMap<vengine::string,DLLData> DLLs;

	// --------------------------------------------------------------
public:
	static MiDLL* GetInstance();

	// 传入函数指针类型，返回该类型函数指针
	template <typename T>
	FunctionPtrType<T> Use(const vengine::string& DLLPath, const vengine::string& FunctionName)
	{	
		FunctionPtrType<T> fptr = nullptr;
		if(!DLLs.Contains(DLLPath))
		{
			// AddDLL
			DLLData data;
			data.Handle = FPlatformProcess::GetDllHandle(StringUtil::VStringToWchar(DLLPath));
			if(data.Handle == nullptr)
			{
				UE_LOG(LogTemp, Display, TEXT("DLL加载失败 Path: %s"), StringUtil::VStringToWchar(DLLPath));
			}
			DLLs.Insert(DLLPath,data);
		}
		auto iterDLL = DLLs.Find(DLLPath);
		if(!iterDLL.Value().Functions.Contains(FunctionName))
		{
			// AddFunction
			void* func = FPlatformProcess::GetDllExport(iterDLL.Value().Handle,StringUtil::VStringToWchar(FunctionName));
			iterDLL.Value().Functions.Insert(FunctionName,func);
		}
		fptr = static_cast<FunctionPtrType<T>>(iterDLL.Value().Functions.Find(FunctionName).Value());
		return fptr;
	}
};

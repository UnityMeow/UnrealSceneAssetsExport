// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <mutex>
#include "IRendererBase.h"


class MiTextureCompress
{
	MiTextureCompress();
	~MiTextureCompress();
	// 单例实例
	static std::atomic<MiTextureCompress*> instance;
	// 互斥锁
	static std::mutex mtx;

	using FunInstantiateVEngine = IRendererBase*(*)(const char*);
	using FunDisposeVEngine = void(*)(IRendererBase*);

	
public:
	static MiTextureCompress* GetInstance();
	FunInstantiateVEngine InstantiateVEngine;
	FunDisposeVEngine DisposeVEngine;
	IRendererBase* RendererBase;
	
};

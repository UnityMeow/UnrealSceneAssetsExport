#pragma once
#include "vstring.h"
#include "Memory.h"
#include "DLL.h"
#include <Windows.h>
class DLL_CLASS DynamicDLL final
{
	HINSTANCE inst = nullptr;
	template <typename T>
	struct GetFuncPtrFromDll;
	template <typename Ret, typename ... Args>
	struct GetFuncPtrFromDll <Ret(Args...)>
	{
		using FuncType = typename Ret(CDECL*)(Args...);
		static FuncType Run(HINSTANCE h, LPCSTR str) noexcept
		{
			if (!h) return nullptr;
			return (FuncType)GetProcAddress(h, str);
		}
	};
public:
	template <typename T>
	using FuncType = typename GetFuncPtrFromDll<T>::FuncType;
	DynamicDLL(vengine::string const& name);
	~DynamicDLL();
	template <typename T>
	FuncType<T> GetDLLFunc(vengine::string const& str)
	{
		return GetFuncPtrFromDll<T>::Run(inst, str.c_str());
	}
	operator bool() const noexcept
	{
		return inst;
	}
	bool avaliable() const noexcept
	{
		return inst;
	}
	DECLARE_VENGINE_OVERRIDE_OPERATOR_NEW
	KILL_COPY_CONSTRUCT(DynamicDLL)
};
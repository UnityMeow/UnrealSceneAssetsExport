#pragma once
#define REFLECTION_CLASS
#include "../Common/vstring.h"
#include "../Common/MetaLib.h"
#include "../Common/Runnable.h"
#define REF_VAR
#define REF_FUNC
struct Reflection
{
private:
	static size_t GetVariableOffset(Type clsType, Type varType, vengine::string const& name);
	static void* GetStaticFunction(Type clsType, Type funcType, vengine::string const& name);

public:
	static void Initialize();
	template <typename VarType>
	static VarType* GetVariablePtr(void* ptr, Type classType, vengine::string const& name)
	{
		size_t offset = GetVariableOffset(classType, typeid(VarType), name);
		if (offset == -1) return nullptr;
		return reinterpret_cast<VarType*>(reinterpret_cast<size_t>(ptr) + offset);
	}
	template <typename VarType>
	static VarType const* GetVariablePtr(void const* ptr, Type classType, vengine::string const& name)
	{
		size_t offset = GetVariableOffset(classType, typeid(VarType), name);
		if (offset == -1) return nullptr;
		return reinterpret_cast<VarType const*>(reinterpret_cast<size_t>(ptr) + offset);
	}
	
	template <typename FuncType>
	static funcPtr_t<FuncType> GetStaticFunction(Type clsType, vengine::string const& name)
	{
		return reinterpret_cast<funcPtr_t<FuncType>>(GetStaticFunction(
			clsType,
			typeid(funcPtr_t<FuncType>),
			name
		));
	}

};
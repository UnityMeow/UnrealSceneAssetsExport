#pragma once
#include "../Common/CommonInclude.h"
#include "../Common/RandomVector.h"
class ReflectionCompileData;
class ICodeTrigger;
class IPreProcessor;
class CodeCompiler
{
	RandomVector<ICodeTrigger*, true> enabledCodeTrigger;
	vengine::vector<ICodeTrigger*> disabledCodeTrigger;
	HashMap<Type, ICodeTrigger*> triggerInType;
	vengine::string str;
	char const* curPtr;
	char const* endPtr;
	size_t frame;
public:
	char const* GetStartPtr() const { return str.data(); }
	char const* GetEndPtr() const { return str.data() + str.size(); }
	vengine::vector<IPreProcessor*> preprocessors;
	ReflectionCompileData* data;
	vengine::string errorCode;
	char const* GetNext(char c, char const* end = nullptr);
	char const* GetNext(funcPtr_t<bool(char c)> c, char const* end = nullptr);
	char const* GetNextString(char const* sign);
	char const* GetNextString(std::initializer_list<char const*> const& sign, uint* fitStringIndex);

	char const* GetNext(char const* begin, char c, char const* end = nullptr);
	char const* GetNext(char const* begin, funcPtr_t<bool(char c)> c, char const* end = nullptr);
	char const* GetNextString(char const* begin, char const* sign);
	char const* GetNextString(char const* begin, std::initializer_list<char const*> const& sign, uint* fitStringIndex);

	ICodeTrigger* GetTrigger(Type t);
	CodeCompiler(vengine::string const& path);
	void Compile();

	bool AddTrigger(std::initializer_list<ICodeTrigger*> const& dependedType, ICodeTrigger* trigger);
	void EnableTrigger(ICodeTrigger* trigger);
	void DisableTrigger(ICodeTrigger* trigger);
	void EnableTrigger(Type tp);
	void DisableTrigger(Type tp);
};
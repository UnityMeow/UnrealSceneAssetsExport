#pragma once
#include "../../Common/CommonInclude.h"
#include "../../Common/BitVector.h"
class CodeCompiler;
class ReflectionCompileData
{
private:
	HashMap<vengine::string, bool> funcKeywords;
public:
	struct ClassData
	{
		vengine::vector< vengine::string> funcs;
		vengine::vector< vengine::string> vars;
		vengine::string className;
	};
	CodeCompiler* compiler;
	vengine::string namespaceName;
	ClassData* currentClass;
	vengine::vector<ClassData> allClassDatas;
	BitVector inStringState;
	HashMap<vengine::string, vengine::vector<int>> ranges;
	
	enum MemberState
	{
		MemberState_Private,
		MemberState_Public,
		MemberState_Protected
	};
	MemberState memberState;
	ReflectionCompileData();
	~ReflectionCompileData();
	bool IsInGlobalArea(char const* ptr);
	bool IsInString(char const* ptr);
	int GetRange(vengine::string const& str, char const* ptr);
	void Clear();
};
#pragma once
#include "../ICodeTrigger.h"
#include "ReflectionCompileData.h"
#include "../../Utility/StringUtility.h"
#include "../CodeCompiler.h"
class NamespaceEvent final : public ICodeTrigger
{
	char last;
	bool inNamespace = false;
public:
	NamespaceEvent()
	{
	}
	virtual size_t GetNextChar(char const* ptr, char const* end) override
	{
		if (compiler->data->IsInString(ptr)) 
			return 1;
		bool isGlobal = compiler->data->IsInGlobalArea(ptr);
		int sz = compiler->data->GetRange("{}",ptr);
		if (isGlobal)
		{
			if (!inNamespace)
			{
				if (StringUtil::CompareCharArray("namespace", ptr, end))
				{
					char const* nameStart = ptr + strlen("namespace");
					char const* next = compiler->GetNext([](char c)->bool
						{
							return StringUtil::IsCharAvaliableCode(c) && !(
								StringUtil::IsCharCharacter(c) ||
								StringUtil::IsCharNumber(c) ||
								c == '_');
						});
					if (next && *next == '{')
					{
						compiler->data->namespaceName.clear();
						StringUtil::CullCodeSpace(
							vengine::string(
								nameStart, next
							),
							compiler->data->namespaceName

						);
						inNamespace = true;
						return next - ptr - 1;
					}
				}
				return 1;
			}
			else if (*ptr == '}' && isGlobal)
			{
				compiler->data->namespaceName.clear();
				inNamespace = false;
			}
		}
		last = *ptr;
		return 1;
	}
};
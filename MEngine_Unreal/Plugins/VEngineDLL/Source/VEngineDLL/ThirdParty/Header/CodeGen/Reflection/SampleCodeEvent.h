#pragma once
#include "../ICodeTrigger.h"
#include "../../Utility/StringUtility.h"
#include "../CodeCompiler.h"
#include "ReflectionCompileData.h"
class SampleCodeEvent final : public ICodeTrigger
{
	char const* GetFunction(char const* ptr, char const* end)
	{
		int range = 0;
		char const* argStart = nullptr;	//'(
		char const* argEnd = nullptr;	//')
		for (char const* ite = ptr; ite < end; ++ite)
		{
			if (*ite == '<')
			{
				range++;
			}
			else if (*ite == '>')
			{
				range--;
			}
			if (range == 0)
			{
				if (*ite == '(')
				{
					argStart = ite;
					for (; ite < end; ++ite)
					{
						if (*ite == ')')
						{
							argEnd = ite;
						}
					}
				}

			}
		}
		if (!argEnd)
		{
			compiler->errorCode = "Invalid Function!";
			return nullptr;
		}
		vengine::vector<vengine::string> sepCache;
		vengine::string functionDeclare(ptr, argStart);
		StringUtil::SeparateString(
			functionDeclare, [](
				const char* ptr, const char* end
				)->bool
			{
				return StringUtil::IsCharCharacter(*ptr) ||
					StringUtil::IsCharNumber(*ptr) ||
					*ptr == '_';
			},
			sepCache
				);
		if (sepCache.size() < 2)
		{
			compiler->errorCode = "Invalide Function!";
			return nullptr;
		}
		 compiler->data->currentClass->funcs.emplace_back(*(sepCache.end() - 1));
		return argEnd + 1;
	}
	char const* GetVariable(char const* ptr, char const* end)
	{
		char const* argEnd = compiler->GetNext(ptr, ';');
		vengine::string functionDeclare(ptr, argEnd);
		vengine::vector<vengine::string> sepCache;
		StringUtil::SeparateString(
			functionDeclare, [](
				const char* ptr, const char* end
				)->bool
			{
				return StringUtil::IsCharCharacter(*ptr) ||
					StringUtil::IsCharNumber(*ptr) ||
					*ptr == '_';
			},
			sepCache);
		compiler->data->currentClass->vars.emplace_back(*(sepCache.end() - 1));
		return argEnd - 1;
	}
public:
	virtual size_t GetNextChar(char const* ptr, char const* end) override
	{
		if (compiler->data->IsInString(ptr)) return 1;
		if (StringUtil::CompareCharArray("REF_VAR", ptr, end))
		{
			auto finalPtr = GetVariable(
				ptr + strlen("REF_VAR"),
				end
			);
			if (!finalPtr) return -1;
			return finalPtr - ptr;
		}
		else if (StringUtil::CompareCharArray("REF_FUNC", ptr, end))
		{
			auto finalPtr = GetFunction(ptr + strlen("REF_FUNC"),
				end
			);
			if (!finalPtr) return -1;
			return finalPtr - ptr;
		}
		else
		{
			return 1;
		}
	}
};
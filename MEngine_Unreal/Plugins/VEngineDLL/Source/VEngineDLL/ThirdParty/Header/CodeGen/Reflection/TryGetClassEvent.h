#pragma once
#include "ReflectionCompileData.h"
#include "../CodeCompiler.h"
#include "../ICodeTrigger.h"
#include "SampleCodeEvent.h"
#include "../../Utility/StringUtility.h"
class TryGetClassNameEvent final : public ICodeTrigger
{
	virtual size_t GetNextChar(char const* ptr, char const* end) override
	{
		char const* origin = ptr;
		ptr = compiler->GetNext(StringUtil::IsCharCharacter);
		if (!ptr) {
			compiler->errorCode = "Invalid class declare";
			return -1;
		}
		char const* classEnd = compiler->GetNext(
			[](char c)->bool
			{
				return c == ';' || c == '{';
			}
		);
		if (!classEnd) {
			compiler->errorCode = "Invalid class end declare";
			return -1;
		}
		vengine::vector<vengine::string> codes;
		vengine::string str = vengine::string(ptr, classEnd);
		auto inheriteIndex = StringUtil::GetFirstIndexOf(str, ':');
		vengine::string inheriteStr;
		if (inheriteIndex > 0)
		{
			inheriteStr.push_back_all(str.data() + inheriteIndex + 1, str.size() - inheriteIndex - 1);
			str.resize(inheriteIndex - 1);
		}
		else if (inheriteIndex == 0)
		{
			compiler->errorCode = "Invalid class end declare";
			return -1;
		}
		StringUtil::SeparateString(str, [](const char* pp, const char* endpp)->bool
			{
				return StringUtil::IsCharAvaliableCode(*pp);
			}, codes);
		vengine::string const& lastWord = *(codes.end() - 1);
		if (lastWord == "final")
		{
			codes.erase_last();
		}
		if (codes.empty())
		{
			compiler->errorCode = "Invalid class end declare";
			return -1;
		}
		ptr = classEnd + 1;
		compiler->data->currentClass = &compiler->data->allClassDatas.emplace_back();
		if (!compiler->data->namespaceName.empty())
			compiler->data->currentClass->className = compiler->data->namespaceName + "::";
		else
			compiler->data->currentClass->className.clear();
		compiler->data->currentClass->className += *(codes.end() - 1);
		compiler->EnableTrigger(typeid(SampleCodeEvent));
		compiler->DisableTrigger(typeid(TryGetClassNameEvent));
		return ptr - origin;
	}
};
class TryGetClassEvent final : public ICodeTrigger
{
	virtual size_t GetNextChar(char const* ptr, char const* end) override
	{
		char const* origin = ptr;
		char const* st = compiler->GetNext(StringUtil::IsCharCharacter);
		if (!st)
		{
			compiler->errorCode = "Not a Class!";
			return -1;
		}
		ptr = st;
		if (StringUtil::CompareCharArray(
			"class",
			ptr,
			end
		))
		{
			compiler->data->memberState = ReflectionCompileData::MemberState_Private;
			ptr += strlen("class");
			compiler->EnableTrigger(typeid(TryGetClassNameEvent));
		}
		else if (StringUtil::CompareCharArray(
			"struct",
			ptr,
			end
		))
		{
			compiler->data->memberState = ReflectionCompileData::MemberState_Public;
			ptr += strlen("struct");
			compiler->EnableTrigger(typeid(TryGetClassNameEvent));
		}
		else
		{
			compiler->errorCode = "Invalid type semantic!";
			return -1;
		}
		compiler->DisableTrigger(typeid(TryGetClassEvent));//Disable Self
		return ptr - st;
	}
};
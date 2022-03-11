#pragma once
#include "../ICodeTrigger.h"
#include "../../Utility/StringUtility.h"
#include "TryGetClassEvent.h"
class HeaderSignEvent final : public ICodeTrigger
{
	virtual size_t GetNextChar(char const* ptr, char const* end) override
	{
		if (
			!compiler->data->IsInString(ptr) && 
			StringUtil::CompareCharArray("REFLECTION_CLASS", ptr, end))
		{
			compiler->EnableTrigger(typeid(TryGetClassEvent));
			return strlen("REFLECTION_CLASS");
		}
		return 1;
	}
};
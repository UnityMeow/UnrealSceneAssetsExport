#pragma once
#include "../Common/CommonInclude.h"
#include <objbase.h>
class DLL_CLASS MGuid
{
public:
	static vengine::string GetFormattedGUID();
	static vengine::string GetGUID();
private:
	static vengine::string GuidToFormattedString(const GUID& guid);
	static vengine::string GuidToString(const GUID& guid);
	MGuid() = delete;
	KILL_COPY_CONSTRUCT(MGuid)
};
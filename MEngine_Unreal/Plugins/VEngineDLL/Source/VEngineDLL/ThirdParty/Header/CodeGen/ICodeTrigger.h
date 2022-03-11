#pragma once
#include "../Common/CommonInclude.h"
class CodeCompiler;
class ICodeTrigger
{
	friend class CodeCompiler;
	uint index = -1;
	size_t enabledFrame;
	uint dependedCount = 0;
	vengine::vector<ICodeTrigger*> dependingEvents;
	vengine::vector<uint> dependingEventsIndex;
	RandomVector<ICodeTrigger*, true, true> dependedEvents;
	void ResetDependCount()
	{
		dependedCount = dependingEvents.size();
	}
protected:
	CodeCompiler* compiler;
public:
	virtual size_t GetNextChar(char const* ptr, char const* end) = 0;
	virtual void BeforeCompile() {}
	virtual void AfterCompile() {}
	virtual ~ICodeTrigger() {}
};
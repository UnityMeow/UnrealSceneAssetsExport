#pragma once
#include "../Common/vstring.h"
class CodeCompiler;
class IPreProcessor
{
	friend class CodeCompiler;
protected:
	CodeCompiler* compiler;
public:
	virtual void PreProcess(vengine::string& str) = 0;
};